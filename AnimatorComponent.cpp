#include "stdafx.h"
#include "AnimatorComponent.h"

AnimatorComponent::AnimatorComponent(Object* pObject, const char* strClipNameForBoneHierarchy)
	:Component(pObject, "Animator")
{
	memset(m_arrToDressInv,		NULL, sizeof(XMFLOAT4X4) * MAX_BONE_NUM);
	memset(m_arrToParent,		NULL, sizeof(XMFLOAT4X4) * MAX_BONE_NUM);
	memset(m_arrToWorld,		NULL, sizeof(XMFLOAT4X4) * MAX_BONE_NUM);
	memset(m_arrLocalRotation,	NULL, sizeof(XMFLOAT4) * MAX_BONE_NUM);
	memset(m_arrParentIdx,		NULL, sizeof(unsigned char) * MAX_BONE_NUM);
	m_numBone = 0;


}
