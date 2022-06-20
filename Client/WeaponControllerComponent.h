#pragma once
#include "Component.h"

class TransformComponent;
class CameraComponent;

class WeaponControllerComponent : public Component
{
public:
	WeaponControllerComponent() = delete;
	WeaponControllerComponent(Object* pObject, Object* pMuzzle, Object* pBullet);
	~WeaponControllerComponent();

public:
	virtual void CheckCollision(Component* other);
	virtual void SolveConstraint();
	virtual void Update(float fTimeElapsed);

public:
	void Fire();
	void Reload();

	void SetCam(Object* pCam);

public:

	// For Ammo
	int m_maxAmmo;
	int m_curAmmo;
	bool m_bReloading = false;
	float m_fReloadProgress = 0;

	float m_fReloadTime = 3.0f;

protected:
	Object* m_pBullet;
	Object* m_pMuzzle;
	float	m_fCooltime;
	float	m_fCurrCooltime;

private:
	bool				m_fTryRaycast	= false;
	float				m_fMinLength	= FLT_MAX;
	Component*			m_pCollided		= nullptr;
	XMFLOAT3			m_xmf3CollisionPoint = XMFLOAT3(0, 0, 0);


	TransformComponent* muzzleTransform;
	TransformComponent* myTransform;
	TransformComponent* camTransform;
	CameraComponent*	cam;
};

