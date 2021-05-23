#include "stdafx.h"
#include "State.h"
#include "Transaction.h"
#include "Animation.h"
#include "Object.h"


bool State::IsSatisfyTransaction(const char* strStateName)
{
	bool result = false;
	for (auto it = m_vecTransactions.begin(); it != m_vecTransactions.end(); it++) {
		if ((*it)->m_targetState->m_strStateName == strStateName && (*it)->Satisfy(m_pObject)) result = true;
	}
	return result;
}

void State::ChangeStateTo(const char* strStateName)
{
	LeaveState();
	m_pObject->SetState(strStateName);
}

void HumanoidState_Moving::Update(float fTimeElapsed)
{
	float fSpdLimit = walkSpd;
	XMFLOAT3 xmf3Velocity = m_pObject->GetVelocity();
	if (m_bKeyDown) {
		xmf3Velocity = Vector3::Add(xmf3Velocity, Vector3::Multiply(fTimeElapsed, Vector3::Normalize(m_xmf3InputDir)));
		Clamp(xmf3Velocity.x, -fSpdLimit, fSpdLimit);
		Clamp(xmf3Velocity.z, -fSpdLimit, fSpdLimit);

		m_pObject->SetVelocity(xmf3Velocity);
	}

	if (0 == m_xmf3InputDir.x) {
		if (xmf3Velocity.x > 0)	xmf3Velocity.x -= m_fDragFactor * fTimeElapsed;
		else					xmf3Velocity.x += m_fDragFactor * fTimeElapsed;
		if (m_fDragFactor * fTimeElapsed >= fabs(xmf3Velocity.x)) xmf3Velocity.x = 0;
	}
	if (0 == m_xmf3InputDir.z) {
		if (xmf3Velocity.z > 0)	xmf3Velocity.z -= m_fDragFactor * fTimeElapsed;
		else					xmf3Velocity.z += m_fDragFactor * fTimeElapsed;
		if (m_fDragFactor * fTimeElapsed >= fabs(xmf3Velocity.z)) xmf3Velocity.z = 0;
	}
	
	m_pObject->SetVelocity(xmf3Velocity);


	if (!m_bKeyDown && IsSatisfyTransaction("Humanoid_Idle")) { ChangeStateTo("Humanoid_Idle"); }
	m_pObject->Move(Vector3::Multiply(fTimeElapsed, m_pObject->GetVelocity()));

}

void HumanoidState_Moving::Input(UCHAR* pKeyBuffer)
{
	// 눌린 방향을 알려준다.
	m_xmf3InputDir = XMFLOAT3(0, 0, 0);
	if (IsKeyDown(pKeyBuffer, KeyCode::_W)) { m_xmf3InputDir.z += 1; m_bKeyDown = true; }
	if (IsKeyDown(pKeyBuffer, KeyCode::_A)) { m_xmf3InputDir.x -= 1; m_bKeyDown = true; }
	if (IsKeyDown(pKeyBuffer, KeyCode::_S)) { m_xmf3InputDir.z -= 1; m_bKeyDown = true; }
	if (IsKeyDown(pKeyBuffer, KeyCode::_D)) { m_xmf3InputDir.x += 1; m_bKeyDown = true; }
	if (IsKeyDown(pKeyBuffer, KeyCode::_Shift)) {m_bShiftDown = true; }
}

vector<pair<string, float>> HumanoidState_Moving::GetAnimClipNameList()
{
	vector<pair<string, float>> result;

	XMFLOAT3 xmf3Velocity = m_pObject->GetVelocity();
	float fVelocityLength = Vector3::Length(xmf3Velocity);
	
	float idleFactor = (walkSpd - fVelocityLength) / walkSpd;
	Clamp(idleFactor, 0, 1);
	// 삼각함수는~~~~ 45도일 때 둘이 합쳐서 1이 넘어요~~~~
	if (fVelocityLength) {
		XMFLOAT3 normalizedDir = Vector3::Normalize(xmf3Velocity);

		if (!normalizedDir.x) {
			if (0 < normalizedDir.z) result.push_back(pair<string, float>("Humanoid_WalkingForward", (1 - idleFactor) * 1));
			if (0 > normalizedDir.z) result.push_back(pair<string, float>("Humanoid_WalkingBackward", (1 - idleFactor) * 1));
		}
		else if (!normalizedDir.z) {
			if (0 < normalizedDir.x) result.push_back(pair<string, float>("Humanoid_WalkingRightStrafe", (1 - idleFactor) * 1));
			if (0 > normalizedDir.x) result.push_back(pair<string, float>("Humanoid_WalkingLeftStrafe", (1 - idleFactor) * 1));
		}
		else {
			float x = normalizedDir.x / (abs(normalizedDir.x) + abs(normalizedDir.z));
			float z = normalizedDir.z / (abs(normalizedDir.x) + abs(normalizedDir.z));

			if (0 < z) result.push_back(pair<string, float>("Humanoid_WalkingForward", (1 - idleFactor) * z));
			if (0 > z) result.push_back(pair<string, float>("Humanoid_WalkingBackward", (1 - idleFactor) * -z));
			
			float forward = normalizedDir.z;
			float strafe = 1 - abs(normalizedDir.z);
			float backward = -normalizedDir.z;

			Clamp(forward, 0, 1);
			Clamp(backward, 0, 1);

			if (0 < x) {
				if(forward) result.push_back(pair<string, float>("Humanoid_WalkingRightStrafeForward", (1 - idleFactor) * x * forward));
				if(strafe)	result.push_back(pair<string, float>("Humanoid_WalkingRightStrafe", (1 - idleFactor) * x * strafe));
				if(backward)result.push_back(pair<string, float>("Humanoid_WalkingLeftStrafeBack", (1 - idleFactor) * x * backward));
			}
			else if (0 > x) {
				if (forward) result.push_back(pair<string, float>("Humanoid_WalkingLeftStrafeForward", (1 - idleFactor) * -x * forward));
				if (strafe)	result.push_back(pair<string, float>("Humanoid_WalkingLeftStrafe", (1 - idleFactor) * -x * strafe));
				if (backward)result.push_back(pair<string, float>("Humanoid_WalkingRightStrafeBack", (1 - idleFactor) * -x * backward));
			}
		}
		result.push_back(pair<string, float>("Humanoid_Idle", idleFactor));
	}

	if (result.empty()) result.push_back(pair<string, float>("Humanoid_Idle", 1));
	return result;
}