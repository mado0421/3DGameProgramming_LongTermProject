#include "stdafx.h"
#include "State.h"
#include "Object.h"
#include "Mask.h"

//==================================================================
// State Layer
//==================================================================
void StateLayer::Animate(AnimatedObject* pObj)
{
	AnimationCalculate::AnimateLocalTransform(pObj, m_pState->m_fTime, m_pState->GetAnimateClipPair(), m_pMask);
}
void StateLayer::Update(AnimatedObject* pObj, float fTimeElapsed)
{
	m_pState->Update(pObj, fTimeElapsed);
}
void StateLayer::Input(AnimatedObject* pObj, UCHAR* pKeyBuffer)
{
	m_pState->Input(pObj, pKeyBuffer);
}
void StateLayer::Interrupted()
{
}
void StateLayer::ChangeStateTo(State* pState)
{
	m_pState = pState;
}

//==================================================================
// BasicLayer::StateHumanoidIdle
//==================================================================
StateHumanoidIdle::StateHumanoidIdle()
	:State("Humanoid_Idle")
{
	//Transition* toWalk = new Transition();
}
void StateHumanoidIdle::Update(AnimatedObject* pObj, float fTimeElapsed)
{
	m_fTime += fTimeElapsed;
}
void StateHumanoidIdle::Input(AnimatedObject* pObj, UCHAR* pKeyBuffer)
{
}
ClipPair StateHumanoidIdle::GetAnimateClipPair()
{
	ClipPair result;

	if (result.empty()) result.push_back(pair<string, float>("Humanoid_Idle", 1));

	return result;
}

////==================================================================
//// ActionLayer::StateHumanoidStand
////==================================================================
//StateHumanoidStand::StateHumanoidStand()
//	:State("Humanoid_Shooting")
//{
//	Transition* toAim = new Transition(new StateHumanoidAim());
//	m_pvTransition.push_back(toAim);
//}
//void StateHumanoidStand::Update(AnimatedObject* pObj, float fTimeElapsed)
//{
//}
//void StateHumanoidStand::Input(AnimatedObject* pObj, UCHAR* pKeyBuffer)
//{
//}
//
//==================================================================
// ActionLayer::StateHumanoidAim
//==================================================================
StateHumanoidAim::StateHumanoidAim()
	:State("Humanoid_Idle")
{
	//Transition* toStand = new Transition(new StateHumanoidStand());Humanoid_Aiming
	//m_pvTransition.push_back(toStand);
}
void StateHumanoidAim::Update(AnimatedObject* pObj, float fTimeElapsed)
{
}
void StateHumanoidAim::Input(AnimatedObject* pObj, UCHAR* pKeyBuffer)
{
}

ClipPair StateHumanoidAim::GetAnimateClipPair()
{
	ClipPair result;

	if (result.empty()) result.push_back(pair<string, float>("Humanoid_Aiming", 1));

	return result;
}
