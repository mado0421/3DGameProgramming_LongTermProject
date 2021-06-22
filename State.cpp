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
		local[i] = XMLoadFloat4(&pObj->m_boneHierarchyInfo.local[i]) * ( 1 - (m_pMask->weight[i] * m_fWeight));
		local[i] += quat[i] * m_pMask->weight[i] * m_fWeight;
		}
		else {
			local[i] = XMLoadFloat4(&pObj->m_boneHierarchyInfo.local[i]) * (1 - m_fWeight);
			local[i] += quat[i] * m_fWeight;
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
void StateLayer::ChangeStateTo(State* pState)
{
	//delete m_pState;
	//if(m_pState) m_pState->Leave();
	m_pState = pState;
	m_pState->Enter();
}

//==================================================================
// BasicLayer::StateHumanoidIdle
//==================================================================
StateHumanoidIdle::StateHumanoidIdle(AnimatedObject* pObj, StateLayer* pLayer)
	:State(pObj, pLayer)
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
		xmf3Velocity = Vector3::Add(xmf3Velocity, Vector3::Multiply(fTimeElapsed * 2.8, Vector3::Normalize(m_xmf3InputDir)));
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
XMVECTOR* StateHumanoidIdle::GetRotation()
{





	XMVECTOR* result = new XMVECTOR[64];
	memset(result, NULL, sizeof(XMVECTOR) * 64);

	ClipPair lPair;

	XMFLOAT3 xmf3Velocity = m_pObj->GetVelocity();
	float fVelocityLength = Vector3::Length(xmf3Velocity);
	float idleFactor = (1.5 - fVelocityLength) / 1.5;

	Clamp(idleFactor, 0, 1);

	if (fVelocityLength) {
		XMFLOAT3 normalizedDir = Vector3::Normalize(xmf3Velocity);
		lPair.push_back(pair<string, float>("Humanoid_Idle", idleFactor));
		if (!normalizedDir.x) {
			if (0 < normalizedDir.z) lPair.push_back(pair<string, float>("Humanoid_WalkingForward", (1 - idleFactor) * 1));
			if (0 > normalizedDir.z) lPair.push_back(pair<string, float>("Humanoid_WalkingBackward", (1 - idleFactor) * 1));
		}
		else if (!normalizedDir.z) {
			if (0 < normalizedDir.x) lPair.push_back(pair<string, float>("Humanoid_WalkingRightStrafe", (1 - idleFactor) * 1));
			if (0 > normalizedDir.x) lPair.push_back(pair<string, float>("Humanoid_WalkingLeftStrafe", (1 - idleFactor) * 1));
		}
		else {
			float x = normalizedDir.x / (abs(normalizedDir.x) + abs(normalizedDir.z));
			float z = normalizedDir.z / (abs(normalizedDir.x) + abs(normalizedDir.z));
			if (0 < z) lPair.push_back(pair<string, float>("Humanoid_WalkingForward", (1 - idleFactor) * z));
			if (0 > z) lPair.push_back(pair<string, float>("Humanoid_WalkingBackward", (1 - idleFactor) * -z));
			float forward = normalizedDir.z;
			float strafe = 1 - abs(normalizedDir.z);
			float backward = -normalizedDir.z;
			Clamp(forward, 0, 1);
			Clamp(backward, 0, 1);
			if (0 < x) {
				if (forward) lPair.push_back(pair<string, float>("Humanoid_WalkingRightStrafeForward", (1 - idleFactor) * x * forward));
				if (strafe)	 lPair.push_back(pair<string, float>("Humanoid_WalkingRightStrafe", (1 - idleFactor) * x * strafe));
				if (backward)lPair.push_back(pair<string, float>("Humanoid_WalkingLeftStrafeBack", (1 - idleFactor) * x * backward));
			}
			else if (0 > x) {
				if (forward) lPair.push_back(pair<string, float>("Humanoid_WalkingLeftStrafeForward", (1 - idleFactor) * -x * forward));
				if (strafe)	 lPair.push_back(pair<string, float>("Humanoid_WalkingLeftStrafe", (1 - idleFactor) * -x * strafe));
				if (backward)lPair.push_back(pair<string, float>("Humanoid_WalkingRightStrafeBack", (1 - idleFactor) * -x * backward));
			}
		}
	}

	if (lPair.empty()) {
		lPair.push_back(pair<string, float>("Humanoid_Idle", 1.0f));
		//lPair.push_back(pair<string, float>("Humanoid_WalkingForward", 0.5f));
		//lPair.push_back(pair<string, float>("Humanoid_WalkingRightStrafeForward", 0.5f));
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
StateHumanoidStand::StateHumanoidStand(AnimatedObject* pObj, StateLayer* pLayer)
	:State(pObj, pLayer)
{
}
void StateHumanoidStand::Update(float fTimeElapsed)
{
	m_pLayer->m_fWeight = 0.0f;
	if (m_bTransition) {
		m_fTimeToTransition += fTimeElapsed;
		m_pLayer->m_fWeight = m_fTimeToTransition / m_fDelay;
		if (m_fDelay <= m_fTimeToTransition) {
			m_pLayer->m_fWeight = 1.0f;
			m_pLayer->ChangeStateTo(new StateHumanoidAim(m_pObj, m_pLayer));
		}
	}
}
void StateHumanoidStand::Input(UCHAR* pKeyBuffer)
{
	if(!m_bTransition)
		if (pKeyBuffer[VK_RBUTTON] & 0xF0) {
			m_bTransition = true; 
			m_fTimeToTransition = 0;
			m_fDelay = 0.12f;
		}
}
XMVECTOR* StateHumanoidStand::GetRotation()
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

//==================================================================
// ActionLayer::StateHumanoidAim
//==================================================================
StateHumanoidAim::StateHumanoidAim(AnimatedObject* pObj, StateLayer* pLayer)
	:State(pObj, pLayer)
{
}
void StateHumanoidAim::Update(float fTimeElapsed)
{
	m_pLayer->m_fWeight = 1.0f;
	if (m_bTransition) {
		m_fTimeToTransition += fTimeElapsed;
		m_pLayer->m_fWeight = 1 - (m_fTimeToTransition / m_fDelay);
		if (m_fDelay <= m_fTimeToTransition) {
			m_pLayer->m_fWeight = 0.0f;
			m_pLayer->ChangeStateTo(new StateHumanoidStand(m_pObj, m_pLayer));
		}
	}
}
void StateHumanoidAim::Input(UCHAR* pKeyBuffer)
{
	if (!m_bTransition)
		if (!(pKeyBuffer[VK_RBUTTON] & 0xF0)) {
			m_bTransition = true;
			m_fTimeToTransition = 0;
			m_fDelay = 0.23f;
		}
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