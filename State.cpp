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

//void InitializeHumanoidStates()
//{
//	HumanoidState_Idle* idle = new HumanoidState_Idle("Humanoid_Idle");
//	HumanoidState_Walk* walk = new HumanoidState_Walk("Humanoid_Walk");
//
//	idle->AddTransation(new Transaction(walk, TV::JUST, NULL, 0));
//	walk->AddTransation(new Transaction(idle, TV::SPD, LessThanEqualsTo, 0));
//
//	g_uomState[idle->m_strStateName] = idle;
//	g_uomState[walk->m_strStateName] = walk;
//	//State* Idle = new State("Humanoid_Idle");
//	//State* Walk = new State("Humanoid_Walk");
//	//State* Run = new State("Humanoid_Run");
//	//State* Jump = new State("Humanoid_Jump");
//	//State* Attack = new State("Humanoid_Attack");
//	//State* Die = new State("Humanoid_Die");
//
//	//Idle->AddTransation(new Transaction(Walk,	TV::SPD, GreaterThan, 0));
//	//Idle->AddTransation(new Transaction(Jump,	TV::JUST, NULL, 0));
//	//Idle->AddTransation(new Transaction(Attack, TV::JUST, NULL, 0));
//	//Idle->AddTransation(new Transaction(Die,	TV::HP, LessThanEqualsTo, 0));
//
//	//Walk->AddTransation(new Transaction(Run,	TV::SPD, GreaterThan, walkSpd));
//	//Walk->AddTransation(new Transaction(Idle,	TV::SPD, LessThanEqualsTo, 0));
//	//Walk->AddTransation(new Transaction(Attack, TV::JUST, NULL, 0));
//	//Walk->AddTransation(new Transaction(Jump,	TV::JUST, NULL, 0));
//	//Walk->AddTransation(new Transaction(Die,	TV::HP, LessThanEqualsTo, 0));
//
//	//Run->AddTransation(new Transaction(Walk,	TV::SPD, LessThanEqualsTo, walkSpd));
//	//Run->AddTransation(new Transaction(Attack,	TV::JUST, NULL, 0));
//	//Run->AddTransation(new Transaction(Jump,	TV::JUST, NULL, 0));
//	//Run->AddTransation(new Transaction(Die,		TV::HP, LessThanEqualsTo, 0));
//
//	//Jump->AddTransation(new Transaction(Idle,	TV::TIME, GreaterThanEqualsTo, 1)); //gAnimMng.GetAnimClip("Humanoid_Jump")->fClipLength;
//	//Jump->AddTransation(new Transaction(Die,	TV::HP, LessThanEqualsTo, 0));
//
//	//Attack->AddTransation(new Transaction(Idle,	TV::TIME, GreaterThanEqualsTo, 1)); //gAnimMng.GetAnimClip("Humanoid_Attack")->fClipLength;
//	//Attack->AddTransation(new Transaction(Die,	TV::HP, LessThanEqualsTo, 0));
//
//	//g_uomState[Idle->m_strStateName] = Idle;
//	//g_uomState[Walk->m_strStateName] = Walk;
//	//g_uomState[Run->m_strStateName] = Run;
//	//g_uomState[Jump->m_strStateName] = Jump;
//	//g_uomState[Attack->m_strStateName] = Attack;
//	//g_uomState[Die->m_strStateName] = Die;
//}

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
