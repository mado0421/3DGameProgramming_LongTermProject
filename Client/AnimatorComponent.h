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
	XMFLOAT4X4* GetFinalResultAnimationTransform();
	XMMATRIX GetToWorldTransform(int boneIdx);
	XMMATRIX GetFinalResultTransform(int boneIdx);

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

class TargetBoardAnimatorComponent : public AnimatorComponent
{
public:
	TargetBoardAnimatorComponent() = delete;
	TargetBoardAnimatorComponent(Object*, const char*);
	~TargetBoardAnimatorComponent();

public:
	virtual void Update(float fTimeElapsed);

private:
	float m_fStandInterpolationValue;	// 1: Stand, 0: Down, [0~1]
};