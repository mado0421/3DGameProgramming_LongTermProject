#include "stdafx.h"
#include "WeaponControllerComponent.h"
#include "Object.h"
#include "Components.h"

WeaponControllerComponent::WeaponControllerComponent(Object* pObject, Object* pMuzzle, Object* pBullet)
	:Component(pObject)
	, m_pBullet(pBullet)
	, m_pMuzzle(pMuzzle)
	, m_fCooltime(0.5f)
	, m_fCurrCooltime(0.0f)
	, m_maxAmmo(10)
	, m_curAmmo(m_maxAmmo)
	, m_bReloading(false)
	, m_fReloadProgress(0)
{

}

WeaponControllerComponent::~WeaponControllerComponent()
{
}

void WeaponControllerComponent::CheckCollision(Component* other)
{
	if (!m_bEnabled || !other->m_bEnabled) return;

	if (m_fTryRaycast) {
		TransformComponent* muzzle = m_pMuzzle->FindComponent<TransformComponent>();
		XMFLOAT3 xmf3Origin, xmf3Direction;
		XMVECTOR origin, direction;
		float length = 0;

		xmf3Origin		= muzzle->GetPosition(Space::world);
		xmf3Direction	= muzzle->GetLookVector(Space::world);

		origin		= XMLoadFloat3(&xmf3Origin);
		direction	= XMLoadFloat3(&xmf3Direction);

		BoxColliderComponent* otherBoxCollider = dynamic_cast<BoxColliderComponent*>(other);
		if (otherBoxCollider) {
			otherBoxCollider->m_box.Intersects(origin, direction, length);
			if (0 < length) {
				if (m_fMinLength > length) {
					m_fMinLength = length;
					m_pCollided = otherBoxCollider;
					XMStoreFloat3(&m_xmf3CollisionPoint, XMVectorAdd(origin, XMVectorScale(direction, length)));
				}
			}
		}
		SphereColliderComponent* otherSphereCollider = dynamic_cast<SphereColliderComponent*>(other);
		if (otherSphereCollider) {
			otherSphereCollider->m_sphere.Intersects(origin, direction, length);
			if (0 < length) {
				if (m_fMinLength > length) {
					m_fMinLength = length;
					m_pCollided = otherSphereCollider;
					XMStoreFloat3(&m_xmf3CollisionPoint, XMVectorAdd(origin, XMVectorScale(direction, length)));
				}
			}
		}
	}
}

void WeaponControllerComponent::SolveConstraint()
{
	if (m_fTryRaycast) {
		m_fTryRaycast = false;
		m_fMinLength = FLT_MAX;
		if (m_pCollided) {
			Character* enemy = m_pCollided->m_pObject->FindComponent<Character>();

			if (enemy) enemy->Damage(100);
		}
		m_pCollided = nullptr;
	}
}

void WeaponControllerComponent::Update(float fTimeElapsed)
{
	if (!m_bEnabled) return;

	m_fCurrCooltime -= fTimeElapsed;

	// Move to position of Parent's RHand
	if (m_pObject->m_pParent) {
		XMMATRIX l_xmmtxTransform = m_pObject->m_pParent->FindComponent<HumanoidAnimatorComponent>()->GetToWorldTransform(28);
		l_xmmtxTransform = XMMatrixMultiply(XMMatrixRotationRollPitchYaw(0, XMConvertToRadians(-90), XMConvertToRadians(-90)), l_xmmtxTransform);

		TransformComponent* transform = m_pObject->FindComponent<TransformComponent>();
		transform->SetLocalTransform(l_xmmtxTransform);

		XMFLOAT3 xmf3Adjust(0, 0, 0);
		xmf3Adjust = Vector3::Add(xmf3Adjust, Vector3::Multiply(-0.028f, transform->GetRightVector()));
		xmf3Adjust = Vector3::Add(xmf3Adjust, Vector3::Multiply(0.08f, transform->GetLookVector()));
		transform->Translate(xmf3Adjust);
	}
}

void WeaponControllerComponent::Fire()
{
	if (!m_bEnabled) return;


	if (0 >= m_fCurrCooltime) {
		m_pMuzzle->FindComponent<EffectComponent>()->TurnOn();

		m_fCurrCooltime = m_fCooltime;
		m_fTryRaycast = true;
	}


	//if (l_pInput->IsKeyDown(KeyCode::_R) && !m_bReloading) {
	//	m_bReloading = true;
	//	m_fReloadProgress = 0;
	//}
	//if (m_bReloading) {
	//	if (m_fReloadProgress > 3) {
	//		m_fReloadProgress = 0;
	//		m_bReloading = false;
	//		m_curAmmo = m_maxAmmo;
	//	}
	//	else {
	//		m_fReloadProgress += fTimeElapsed;
	//	}
	//}

}
