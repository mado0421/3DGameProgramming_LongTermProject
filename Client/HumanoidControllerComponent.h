#pragma once
#include "Component.h"
#include "Character.h"

class TransformComponent;
class InputManagerComponent;

class HumanoidControllerComponent : public Component, public Character
{
public:
	HumanoidControllerComponent() = delete;
	HumanoidControllerComponent(Object* pObject, Object* pWeapon);
	~HumanoidControllerComponent();

	void SetLookAt(Object* pObejct);

public:

	virtual void Update(float fTimeElapsed);

	float		m_fTime;

	// For Movement Part
	XMFLOAT3	m_xmf3Velocity;
	float		m_fSpeed;
	float		m_fDragFactor;

	float		m_fAimProgress;
	float		m_fTimeForAim;
	Object*		m_pWeaponObject;

	TransformComponent* m_pLookAt = nullptr;
	float lookAtYAngle = 0;
	InputManagerComponent* l_pInput;
	TransformComponent* l_transform;
};

