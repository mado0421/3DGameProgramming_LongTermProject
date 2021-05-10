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

void HumanoidState_Walk::Update(float fTimeElapsed)
{
	if (m_bKeyDown) {
		m_pObject->SetSpeed(m_pObject->GetSpeed() + m_fSpdIncreaseFactor * fTimeElapsed); 
		if (m_pObject->GetSpeed() > m_fSpdMaximum) m_pObject->SetSpeed(m_fSpdMaximum);
		m_bKeyDown = false;
	}
	else {
		m_pObject->SetSpeed(m_pObject->GetSpeed() - m_fSpdDecreaseFactor * fTimeElapsed);
		if (m_pObject->GetSpeed() < 0) m_pObject->SetSpeed(0);
		if (IsSatisfyTransaction("Humanoid_Idle")) ChangeStateTo("Humanoid_Idle");
	}

	m_pObject->Move(Vector3::Multiply(fTimeElapsed * m_pObject->GetSpeed(), m_pObject->GetLook()));
}
