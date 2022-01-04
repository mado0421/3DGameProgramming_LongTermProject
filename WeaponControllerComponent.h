#pragma once
#include "Component.h"

class WeaponControllerComponent : public Component
{
public:
	WeaponControllerComponent() = delete;
	WeaponControllerComponent(Object* pObject, Object* pMuzzle, Object* pBullet);
	~WeaponControllerComponent();

public:
	virtual void Update(float fTimeElapsed);

public:
	void Fire();

protected:
	Object* m_pBullet;
	Object* m_pMuzzle;
	float	m_fCooltime;
	float	m_fCurrCooltime;
};

