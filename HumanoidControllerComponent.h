#pragma once
#include "Component.h"

class HumanoidControllerComponent : public Component
{
public:
	HumanoidControllerComponent() = delete;
	HumanoidControllerComponent(Object* pObject, Object* pWeapon);
	~HumanoidControllerComponent();

public:
	virtual void Update(float fTimeElapsed);

	float		m_fTime = 0;

	// For Movement Part
	XMFLOAT3	m_xmf3Velocity = XMFLOAT3(0, 0, 0);
	float		m_fSpeed = 1.0f;
	float		m_fDragFactor = 1.0f;

	float		m_fAimProgress = 0;
	float		m_fTimeForAim = 1.0f;
	Object*		m_pWeaponObject = nullptr;
};

