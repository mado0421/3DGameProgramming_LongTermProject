#pragma once
#include "Component.h"

#define MAX_BONE_NUM 64

class AnimatorComponent : public Component
{
public:
	AnimatorComponent() = delete;
	AnimatorComponent(Object* pObject, const char* strClipNameForBoneHierarchy);
	~AnimatorComponent();

public:
	virtual void Update(float fTimeElapsed) {}

protected:
	XMFLOAT4X4		m_arrToDressInv[MAX_BONE_NUM];
	XMFLOAT4X4		m_arrToParent[MAX_BONE_NUM];
	XMFLOAT4X4		m_arrToWorld[MAX_BONE_NUM];
	XMFLOAT4		m_arrLocalRotation[MAX_BONE_NUM];
	unsigned char	m_arrParentIdx[MAX_BONE_NUM];
	unsigned char	m_numBone;
};

