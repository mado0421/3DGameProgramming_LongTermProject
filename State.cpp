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
	float fSpdLimit = 0;


	if (m_bKeyDown) {
		// 만약 Shift가 눌렸다면
		// 속력 제한을 RunningSPD까지
		if (m_bShiftDown)	fSpdLimit = runSpd;
		else 				fSpdLimit = walkSpd;

		m_pObject->SetVelocity(Vector3::Multiply(1 - min(fTimeElapsed * 0.3f, 1), m_pObject->GetVelocity()));

		// 증가
		m_pObject->SetVelocity(Vector3::Add(m_pObject->GetVelocity(), Vector3::Multiply(fTimeElapsed * 1.5f, Vector3::Normalize(m_xmf3MovingDir))));

		// 절대값이 한도를 넘으면 제한
		XMFLOAT3 currVelocity = m_pObject->GetVelocity();
		Clamp(currVelocity.x, -fSpdLimit, fSpdLimit);
		Clamp(currVelocity.z, -fSpdLimit, fSpdLimit);
		if (IsZero(currVelocity.x)) currVelocity.x = 0;
		if (IsZero(currVelocity.z)) currVelocity.z = 0;
		m_pObject->SetVelocity(currVelocity);

		m_bKeyDown		= false;
		m_bShiftDown	= false;
	}
	else {
		m_pObject->SetVelocity(Vector3::Multiply(1 - min(fTimeElapsed * 2.5f, 1), m_pObject->GetVelocity()));
		XMFLOAT3 currVelocity = m_pObject->GetVelocity();
		if (IsZero(currVelocity.x)) currVelocity.x = 0;
		if (IsZero(currVelocity.z)) currVelocity.z = 0;
		m_pObject->SetVelocity(currVelocity);

		if (IsSatisfyTransaction("Humanoid_Idle")) { m_pObject->SetSpeed(0); ChangeStateTo("Humanoid_Idle"); }
	}
	//m_pObject->Move(Vector3::Multiply(fTimeElapsed, m_pObject->GetVelocity()));

}

void HumanoidState_Moving::Input(UCHAR* pKeyBuffer)
{
	// 눌린 방향을 알려준다.
	m_xmf3MovingDir = XMFLOAT3(0, 0, 0);
	if (IsKeyDown(pKeyBuffer, KeyCode::_W)) { m_xmf3MovingDir.z += 1; m_bKeyDown = true; }
	if (IsKeyDown(pKeyBuffer, KeyCode::_A)) { m_xmf3MovingDir.x -= 1; m_bKeyDown = true; }
	if (IsKeyDown(pKeyBuffer, KeyCode::_S)) { m_xmf3MovingDir.z -= 1; m_bKeyDown = true; }
	if (IsKeyDown(pKeyBuffer, KeyCode::_D)) { m_xmf3MovingDir.x += 1; m_bKeyDown = true; }
	if (IsKeyDown(pKeyBuffer, KeyCode::_Shift)) {m_bShiftDown = true; }
}

vector<pair<string, float>> HumanoidState_Moving::GetAnimClipNameList()
{
	vector<pair<string, float>> result;

	float fLength = Vector3::Length( m_pObject->GetVelocity() );
	if (walkSpd >= fLength) {
		//float walkFactor = fLength / walkSpd;
		//float idleFactor = (1 - fLength) / walkSpd;
		float walkFactor = 1;

		XMFLOAT3 normalizedDir = Vector3::Normalize(m_pObject->GetVelocity());
		if (0 < normalizedDir.x) result.push_back(pair<string, float>("Humanoid_WalkingRightStrafe",walkFactor * normalizedDir.x));
		if (0 < normalizedDir.z) result.push_back(pair<string, float>("Humanoid_WalkingForward",	walkFactor * normalizedDir.z));
		if (0 > normalizedDir.x) result.push_back(pair<string, float>("Humanoid_WalkingLeftStrafe", walkFactor * -normalizedDir.x));
		if (0 > normalizedDir.z) result.push_back(pair<string, float>("Humanoid_WalkingBackward",	walkFactor * -normalizedDir.z));
		//result.push_back(pair<string, float>("Humanoid_Idle", idleFactor));


	}
	else {

	}

	system("cls");
	cout << "fLength :" << fLength << "\n";
	for (int i = 0; i < result.size(); i++) cout << result[i].first.c_str() << "\t - :" << result[i].second << "\n";
	Sleep(1);

	if (result.empty()) result.push_back(pair<string, float>("Humanoid_Idle", 1));
	return result;
}