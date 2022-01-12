#include "stdafx.h"
#include "AnimatorComponent.h"
#include "Animation.h"
#include "Object.h"
#include "Components.h"
#include "Mask.h"

AnimatorComponent::AnimatorComponent(Object* pObject, const char* strClipNameForBoneHierarchy)
	:Component(pObject)
{
	memset(m_arrToDressInv,		NULL, sizeof(XMFLOAT4X4) * MAX_BONE_NUM);
	memset(m_arrToParent,		NULL, sizeof(XMFLOAT4X4) * MAX_BONE_NUM);
	memset(m_arrToWorld,		NULL, sizeof(XMFLOAT4X4) * MAX_BONE_NUM);
	memset(m_arrLocalRotation,	NULL, sizeof(XMFLOAT4) * MAX_BONE_NUM);
	memset(m_arrParentIdx,		NULL, sizeof(unsigned char) * MAX_BONE_NUM);
	m_numBone = 0;


	// toDressInv, toParent, parentIdx, nBone을 받아야 함.
	AnimClip* clip = g_AnimMng.GetAnimClip(strClipNameForBoneHierarchy);

	assert(("Clip is nullptr", clip != nullptr));

	m_numBone = clip->vecBone.size();

	// clip의 구조 때문에 memcpy로 한 번에 긁어올 수가 없음.
	for (int i = 0; i < clip->vecBone.size(); i++) {
		m_arrToDressInv[i]	= clip->vecBone[i].toDressposeInv;
		m_arrToParent[i]	= clip->vecBone[i].toParent;
		m_arrParentIdx[i]	= clip->vecBone[i].parentIdx;
	}
}

AnimatorComponent::~AnimatorComponent()
{
}

XMFLOAT4X4* AnimatorComponent::GetFinalResultAnimationTransform()
{
	XMFLOAT4X4 result[MAX_BONE_NUM];

	for (int i = 0; i < MAX_BONE_NUM; i++)
		XMStoreFloat4x4(&result[i],	XMMatrixTranspose(XMMatrixMultiply(XMLoadFloat4x4(&m_arrToDressInv[i]), XMLoadFloat4x4(&m_arrToWorld[i]))));

	return result;
}

XMMATRIX AnimatorComponent::GetToWorldTransform(int boneIdx)
{
	return XMLoadFloat4x4(&m_arrToWorld[boneIdx]);
}

void AnimatorComponent::CalcToWorld()
{
	memset(m_arrToWorld, NULL, sizeof(XMFLOAT4X4) * MAX_BONE_NUM);

	for (int i = 0; i < m_numBone; i++) {
		if (-1 != m_arrParentIdx[i]) {
			XMStoreFloat4x4(
				&m_arrToWorld[i],
				XMMatrixMultiply(XMMatrixMultiply(XMMatrixRotationQuaternion(XMLoadFloat4(&m_arrLocalRotation[i])), XMLoadFloat4x4(&m_arrToParent[i])), XMLoadFloat4x4(&m_arrToWorld[m_arrParentIdx[i]])));
		}
		else {
			XMStoreFloat4x4(
				&m_arrToWorld[i],
				XMMatrixMultiply(XMMatrixRotationQuaternion(XMLoadFloat4(&m_arrLocalRotation[i])), XMLoadFloat4x4(&m_arrToParent[i]))
			);
		}
	}
}

HumanoidAnimatorComponent::HumanoidAnimatorComponent(Object* pObject, const char* strClipNameForBoneHierarchy)
	:AnimatorComponent(pObject, strClipNameForBoneHierarchy)
	, m_pAimingMask(new BoneMask(BoneMask::PreDefined::eUpperBody))
{
}

HumanoidAnimatorComponent::~HumanoidAnimatorComponent()
{
	delete m_pAimingMask;
}

void HumanoidAnimatorComponent::Update(float fTimeElapsed)
{
	if (!m_bEnabled) return;

	memset(m_arrLocalRotation, NULL, sizeof(XMFLOAT4) * MAX_BONE_NUM);

	HumanoidControllerComponent* l_HCC = m_pObject->FindComponent<HumanoidControllerComponent>();
	float l_fTime = l_HCC->m_fTime;

	// Movement Layer
	XMVECTOR l_arrMovementLocalRotation[MAX_BONE_NUM];
	memset(l_arrMovementLocalRotation, NULL, sizeof(XMVECTOR) * MAX_BONE_NUM);
	{
		ClipPair lPair;
		
		XMFLOAT3 l_xmf3Velocity = l_HCC->m_xmf3Velocity;
		float l_fVelocityLength = Vector3::Length(l_xmf3Velocity);
		float l_fidleFactor = (1.5 - l_fVelocityLength) / 1.5;

		Clamp(l_fidleFactor, 0, 1);

		if (l_fVelocityLength) {
			XMFLOAT3 normalizedDir = Vector3::Normalize(l_xmf3Velocity);
			lPair.push_back(pair<string, float>("Humanoid_Idle", l_fidleFactor));
			if (!normalizedDir.x) {
				if (0 < normalizedDir.z) lPair.push_back(pair<string, float>("Humanoid_WalkingForward", (1 - l_fidleFactor) * 1));
				if (0 > normalizedDir.z) lPair.push_back(pair<string, float>("Humanoid_WalkingBackward", (1 - l_fidleFactor) * 1));
			}
			else if (!normalizedDir.z) {
				if (0 < normalizedDir.x) lPair.push_back(pair<string, float>("Humanoid_WalkingRightStrafe", (1 - l_fidleFactor) * 1));
				if (0 > normalizedDir.x) lPair.push_back(pair<string, float>("Humanoid_WalkingLeftStrafe", (1 - l_fidleFactor) * 1));
			}
			else {
				float x = normalizedDir.x / (abs(normalizedDir.x) + abs(normalizedDir.z));
				float z = normalizedDir.z / (abs(normalizedDir.x) + abs(normalizedDir.z));
				if (0 < z) lPair.push_back(pair<string, float>("Humanoid_WalkingForward", (1 - l_fidleFactor) * z));
				if (0 > z) lPair.push_back(pair<string, float>("Humanoid_WalkingBackward", (1 - l_fidleFactor) * -z));
				float forward = normalizedDir.z;
				float strafe = 1 - abs(normalizedDir.z);
				float backward = -normalizedDir.z;
				Clamp(forward, 0, 1);
				Clamp(backward, 0, 1);
				if (0 < x) {
					if (forward) lPair.push_back(pair<string, float>("Humanoid_WalkingRightStrafeForward", (1 - l_fidleFactor) * x * forward));
					if (strafe)	 lPair.push_back(pair<string, float>("Humanoid_WalkingRightStrafe", (1 - l_fidleFactor) * x * strafe));
					if (backward)lPair.push_back(pair<string, float>("Humanoid_WalkingLeftStrafeBack", (1 - l_fidleFactor) * x * backward));
				}
				else if (0 > x) {
					if (forward) lPair.push_back(pair<string, float>("Humanoid_WalkingLeftStrafeForward", (1 - l_fidleFactor) * -x * forward));
					if (strafe)	 lPair.push_back(pair<string, float>("Humanoid_WalkingLeftStrafe", (1 - l_fidleFactor) * -x * strafe));
					if (backward)lPair.push_back(pair<string, float>("Humanoid_WalkingRightStrafeBack", (1 - l_fidleFactor) * -x * backward));
				}
			}
		}

		if (lPair.empty()) {
			lPair.push_back(pair<string, float>("Humanoid_Idle", 1.0f));
		}
		for (int i = 0; i < lPair.size(); i++) {
			AnimClip* clip = g_AnimMng.GetAnimClip(lPair[i].first.c_str());
			XMINT4 xmi4FrameIdx;
			float fNormalizedTime;

			AnimationCalculate::GetFrameIdxAndNormalizedTime(clip, l_fTime, fNormalizedTime, xmi4FrameIdx);
			for (int j = 0; j < clip->vecBone.size(); j++) {
				l_arrMovementLocalRotation[j] += AnimationCalculate::GetLocalTransform(clip, j, fNormalizedTime, xmi4FrameIdx) * (lPair[i].second);
			}
		}
	}

	// Aiming State
	XMVECTOR l_arrAimingLocalRotation[MAX_BONE_NUM];
	memset(l_arrAimingLocalRotation, NULL, sizeof(XMVECTOR) * MAX_BONE_NUM);
	{
		ClipPair lPair;

		if (lPair.empty()) lPair.push_back(pair<string, float>("Humanoid_Aiming", 1));

		for (int i = 0; i < lPair.size(); i++) {
			AnimClip* clip = g_AnimMng.GetAnimClip(lPair[i].first.c_str());
			XMINT4 xmi4FrameIdx;
			float fNormalizedTime;

			AnimationCalculate::GetFrameIdxAndNormalizedTime(clip, l_fTime, fNormalizedTime, xmi4FrameIdx);
			for (int j = 0; j < clip->vecBone.size(); j++) {
				l_arrAimingLocalRotation[j] += AnimationCalculate::GetLocalTransform(clip, j, fNormalizedTime, xmi4FrameIdx) * (lPair[i].second);
			}
		}
	}

	float l_fAimingWeight =  l_HCC->m_fAimProgress / l_HCC->m_fTimeForAim;
	for (int i = 0; i < MAX_BONE_NUM; i++) {
		l_arrMovementLocalRotation[i] *= (1 - (m_pAimingMask->weight[i] * l_fAimingWeight));
		l_arrMovementLocalRotation[i] += l_arrAimingLocalRotation[i] * m_pAimingMask->weight[i] * l_fAimingWeight;
		XMStoreFloat4(&m_arrLocalRotation[i], l_arrMovementLocalRotation[i]);
	}

	CalcToWorld();
}
