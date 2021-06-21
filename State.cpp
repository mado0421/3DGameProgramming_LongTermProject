#include "stdafx.h"
#include "State.h"
#include "Object.h"
#include "Mask.h"

//==================================================================
// State Layer
//==================================================================
void StateLayer::Animate(AnimatedObject* pObj)
{
	//AnimationCalculate::AnimateLocalTransform(pObj, m_pState->m_fTime, m_pState->GetAnimateClipPair(), m_pMask);
	XMVECTOR* quat = m_pState->GetRotation();

	XMVECTOR local[64];

	for (int i = 0; i < 64; i++) {
		if (m_pMask) {
		local[i] = XMLoadFloat4(&pObj->m_boneHierarchyInfo.local[i]) * ( 1 - m_pMask->weight[i] );
		local[i] += quat[i] * m_pMask->weight[i];
		}
		else {
			local[i] = quat[i];
		}

		XMStoreFloat4(&pObj->m_boneHierarchyInfo.local[i], local[i]);
	}

}
void StateLayer::Update(float fTimeElapsed)
{
	m_pState->Update(fTimeElapsed);
}
void StateLayer::Input(UCHAR* pKeyBuffer)
{
	m_pState->Input(pKeyBuffer);
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
StateHumanoidIdle::StateHumanoidIdle(AnimatedObject* pObj)
	:State("Humanoid_Idle", pObj)
{
	//Transition* toWalk = new Transition();
}
void StateHumanoidIdle::Update(float fTimeElapsed)
{
	m_fTime += fTimeElapsed;

	XMFLOAT3 xmf3Velocity = m_pObj->GetVelocity();
	float fPrevSpd = Vector3::Length(xmf3Velocity);
	float fSpdLimit = 1.5f;
	if (!Vector3::IsZero(m_xmf3InputDir)) {
		xmf3Velocity = Vector3::Add(xmf3Velocity, Vector3::Multiply(fTimeElapsed, Vector3::Normalize(m_xmf3InputDir)));
		Clamp(xmf3Velocity.x, -fSpdLimit, fSpdLimit);
		Clamp(xmf3Velocity.z, -fSpdLimit, fSpdLimit);

		m_pObj->SetVelocity(xmf3Velocity);
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

	m_pObj->SetVelocity(xmf3Velocity);

	m_pObj->Move(Vector3::Multiply(fTimeElapsed, m_pObj->GetVelocity()));

	//if (0 == fPrevSpd && 0 < Vector3::Length(xmf3Velocity)) AddSubState("MOVING");
	//if (!m_bPrevAiming && m_bCurrAiming) AddSubState("AIMING");
	//if (m_bPrevAiming && !m_bCurrAiming) DisableSubState("AIMING");

	//MakeAnimTransform();
}
void StateHumanoidIdle::Input(UCHAR* pKeyBuffer)
{
	m_xmf3InputDir = XMFLOAT3(0, 0, 0);
	if (pKeyBuffer[KeyCode::_W] & 0xF0) { m_xmf3InputDir.z += 1; }
	if (pKeyBuffer[KeyCode::_A] & 0xF0) { m_xmf3InputDir.x -= 1; }
	if (pKeyBuffer[KeyCode::_S] & 0xF0) { m_xmf3InputDir.z -= 1; }
	if (pKeyBuffer[KeyCode::_D] & 0xF0) { m_xmf3InputDir.x += 1; }

	//if (pKeyBuffer[VK_RBUTTON] & 0xF0) { m_bCurrAiming = true; }
	//else m_bCurrAiming = false;
}
ClipPair StateHumanoidIdle::GetAnimateClipPair()
{
	return ClipPair();

	////ClipPair result;
	////if (result.empty()) result.push_back(pair<string, float>("Humanoid_Idle", 1));
	////return result;
	//ClipPair result;
	////XMFLOAT3 xmf3Velocity = m_pObj->GetVelocity();
	////float fVelocityLength = Vector3::Length(xmf3Velocity);
	////float idleFactor = (1.5 - fVelocityLength) / 1.5;
	////Clamp(idleFactor, 0, 1);
	////if (fVelocityLength) {
	////	XMFLOAT3 normalizedDir = Vector3::Normalize(xmf3Velocity);
	////	result.push_back(pair<string, float>("Humanoid_Idle", idleFactor));
	////	if (!normalizedDir.x) {
	////		if (0 < normalizedDir.z) result.push_back(pair<string, float>("Humanoid_WalkingForward", (1 - idleFactor) * 1));
	////		if (0 > normalizedDir.z) result.push_back(pair<string, float>("Humanoid_WalkingBackward", (1 - idleFactor) * 1));
	////	}
	////	else if (!normalizedDir.z) {
	////		if (0 < normalizedDir.x) result.push_back(pair<string, float>("Humanoid_WalkingRightStrafe", (1 - idleFactor) * 1));
	////		if (0 > normalizedDir.x) result.push_back(pair<string, float>("Humanoid_WalkingLeftStrafe", (1 - idleFactor) * 1));
	////	}
	////	else {
	////		float x = normalizedDir.x / (abs(normalizedDir.x) + abs(normalizedDir.z));
	////		float z = normalizedDir.z / (abs(normalizedDir.x) + abs(normalizedDir.z));
	////		ClipPair result;
	////		if (0 < z) result.push_back(pair<string, float>("Humanoid_WalkingForward", (1 - idleFactor) * z));
	////		if (0 > z) result.push_back(pair<string, float>("Humanoid_WalkingBackward", (1 - idleFactor) * -z));
	////		float forward = normalizedDir.z;
	////		float strafe = 1 - abs(normalizedDir.z);
	////		float backward = -normalizedDir.z;
	////		Clamp(forward, 0, 1);
	////		Clamp(backward, 0, 1);
	////		if (0 < x) {
	////			if (forward) result.push_back(pair<string, float>("Humanoid_WalkingRightStrafeForward", (1 - idleFactor) * x * forward));
	////			if (strafe)	result.push_back(pair<string, float>("Humanoid_WalkingRightStrafe", (1 - idleFactor) * x * strafe));
	////			if (backward)result.push_back(pair<string, float>("Humanoid_WalkingLeftStrafeBack", (1 - idleFactor) * x * backward));
	////		}
	////		else if (0 > x) {
	////			if (forward) result.push_back(pair<string, float>("Humanoid_WalkingLeftStrafeForward", (1 - idleFactor) * -x * forward));
	////			if (strafe)	result.push_back(pair<string, float>("Humanoid_WalkingLeftStrafe", (1 - idleFactor) * -x * strafe));
	////			if (backward)result.push_back(pair<string, float>("Humanoid_WalkingRightStrafeBack", (1 - idleFactor) * -x * backward));
	////		}
	////	}
	////}
	//XMFLOAT3 xmf3Velocity = m_pObj->GetVelocity();
	//float fVelocityLength = Vector3::Length(xmf3Velocity);
	//float idleFactor = (1.5 - fVelocityLength) / 1.5;
	//Clamp(idleFactor, 0, 1);
	////result.push_back(pair<string, float>("Humanoid_WalkingForward", (1 - idleFactor)));
	////result.push_back(pair<string, float>("Humanoid_Idle", idleFactor));
	//result.push_back(pair<string, float>("Humanoid_WalkingForward", 0.5f));
	//result.push_back(pair<string, float>("Humanoid_WalkingRightStrafeForward", 0.5f));
	//if (result.empty()) result.push_back(pair<string, float>("Humanoid_Idle", 1));
	//return result;
}
XMVECTOR* StateHumanoidIdle::GetRotation()
{
	XMVECTOR* result = new XMVECTOR[64];
	memset(result, NULL, sizeof(XMVECTOR) * 64);

	ClipPair lPair;

	if (lPair.empty()) {
		lPair.push_back(pair<string, float>("Humanoid_WalkingForward", 0.5f));
		lPair.push_back(pair<string, float>("Humanoid_WalkingRightStrafeForward", 0.5f));
	}
	for (int i = 0; i < lPair.size(); i++) {
		AnimClip* clip = g_AnimMng.GetAnimClip(lPair[i].first.c_str());
		XMINT4 xmi4FrameIdx;
		float fNormalizedTime;
		
		AnimationCalculate::GetFrameIdxAndNormalizedTime(clip, m_fTime, fNormalizedTime, xmi4FrameIdx);
		for (int j = 0; j < clip->vecBone.size(); j++) {
			result[j] += AnimationCalculate::GetLocalTransform(clip, j, fNormalizedTime, xmi4FrameIdx) * (lPair[i].second);
		}
	}
	 
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
StateHumanoidAim::StateHumanoidAim(AnimatedObject* pObj)
	:State("Humanoid_Idle", pObj)
{
	//Transition* toStand = new Transition(new StateHumanoidStand());Humanoid_Aiming
	//m_pvTransition.push_back(toStand);
}
void StateHumanoidAim::Update(float fTimeElapsed)
{
}
void StateHumanoidAim::Input(UCHAR* pKeyBuffer)
{
}
XMVECTOR* StateHumanoidAim::GetRotation()
{
	XMVECTOR* result = new XMVECTOR[64];
	memset(result, NULL, sizeof(XMVECTOR) * 64);

	ClipPair lPair;

	if (lPair.empty()) lPair.push_back(pair<string, float>("Humanoid_Aiming", 1));

	for (int i = 0; i < lPair.size(); i++) {
		AnimClip* clip = g_AnimMng.GetAnimClip(lPair[i].first.c_str());
		XMINT4 xmi4FrameIdx;
		float fNormalizedTime;

		AnimationCalculate::GetFrameIdxAndNormalizedTime(clip, m_fTime, fNormalizedTime, xmi4FrameIdx);
		for (int j = 0; j < clip->vecBone.size(); j++) {
			result[j] += AnimationCalculate::GetLocalTransform(clip, j, fNormalizedTime, xmi4FrameIdx) * (lPair[i].second);
		}
	}

	return result;
}
ClipPair StateHumanoidAim::GetAnimateClipPair()
{
	return ClipPair();
	//ClipPair result;

	//if (result.empty()) {
	//	result.push_back(pair<string, float>("Humanoid_Idle", 1.0f));
	//}
	//return result;
}