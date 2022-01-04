#pragma once
#include "Component.h"

class BoneMask;

class AnimatorComponent : public Component
{
public:
	AnimatorComponent() = delete;
	AnimatorComponent(Object* pObject, const char* strClipNameForBoneHierarchy);
	~AnimatorComponent();

public:
	virtual void Update(float fTimeElapsed) {}

	XMFLOAT4X4* GetFinalResultAnimationTransform();
	XMMATRIX GetToWorldTransform(int boneIdx);

protected:
	void CalcToWorld();

protected:
	XMFLOAT4X4		m_arrToDressInv[MAX_BONE_NUM];
	XMFLOAT4X4		m_arrToParent[MAX_BONE_NUM];
	int				m_arrParentIdx[MAX_BONE_NUM];
	int				m_numBone;

	XMFLOAT4X4		m_arrToWorld[MAX_BONE_NUM];
	XMFLOAT4		m_arrLocalRotation[MAX_BONE_NUM];
};

class HumanoidAnimatorComponent : public AnimatorComponent 
{
public:
	HumanoidAnimatorComponent() = delete;
	HumanoidAnimatorComponent(Object* pObject, const char* strClipNameForBoneHierarchy);
	~HumanoidAnimatorComponent();

public:
	virtual void Update(float fTimeElapsed);

protected:
	BoneMask* m_pAimingMask = nullptr;
};