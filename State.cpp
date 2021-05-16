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
	float lSpdLimit = 0;
	// 만약 Key가 눌렸다면
	// 속력을 증가시킨다
	if (m_bKeyDown) {
		// 만약 Shift가 눌렸다면
		// 속력 제한을 RunningSPD까지
		if (m_bShiftDown)	lSpdLimit = runSpd;
		else 				lSpdLimit = walkSpd;
		
		// 만약 현재 속력이 limit보다 작다면 증가
		// 아니라면 감소
		if (lSpdLimit > m_pObject->GetSpeed()) 
				m_pObject->SetSpeed(m_pObject->GetSpeed() + fTimeElapsed);
		else	m_pObject->SetSpeed(m_pObject->GetSpeed() - fTimeElapsed);

		m_bKeyDown = false;
		m_bShiftDown = false;
	}
	else {
		m_pObject->SetSpeed(m_pObject->GetSpeed() - fTimeElapsed);
		if (IsSatisfyTransaction("Humanoid_Idle")) { m_pObject->SetSpeed(0); ChangeStateTo("Humanoid_Idle"); }
	}

	m_pObject->Move(Vector3::Multiply(m_pObject->GetSpeed() * fTimeElapsed, Vector3::Normalize(m_xmf3MovingDir)));
}

void HumanoidState_Moving::Input(UCHAR* pKeyBuffer)
{
	//m_xmf3MovingDir
	m_xmf3MovingDir = XMFLOAT3(0, 0, 0);
	if (IsKeyDown(pKeyBuffer, KeyCode::_W)) { m_xmf3MovingDir.z += 1; m_bKeyDown = true; }
	if (IsKeyDown(pKeyBuffer, KeyCode::_A)) { m_xmf3MovingDir.x -= 1; m_bKeyDown = true; }
	if (IsKeyDown(pKeyBuffer, KeyCode::_S)) { m_xmf3MovingDir.z -= 1; m_bKeyDown = true; }
	if (IsKeyDown(pKeyBuffer, KeyCode::_D)) { m_xmf3MovingDir.x += 1; m_bKeyDown = true; }
	if (IsKeyDown(pKeyBuffer, KeyCode::_Shift)) {m_bShiftDown = true; }
}

vector<string> HumanoidState_Moving::GetAnimClipNameList()
{
	vector<string> result;
	if (m_xmf3MovingDir.z == 1) result.push_back("Humanoid_WalkingForward");
	if (m_xmf3MovingDir.x == 1) result.push_back("Humanoid_WalkingRightStrafe");
	if (m_xmf3MovingDir.z == -1) result.push_back("Humanoid_WalkingBackward");
	if (m_xmf3MovingDir.x == -1) result.push_back("Humanoid_WalkingLeftStrafe");
	if (result.empty()) result.push_back("Humanoid_Idle");
	return result;
}