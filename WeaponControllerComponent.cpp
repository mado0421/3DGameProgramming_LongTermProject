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
			cout << "Shoot at " << m_xmf3CollisionPoint.x << ", " << m_xmf3CollisionPoint.y << ", " << m_xmf3CollisionPoint.z << "\n";
			m_pCollided->m_pObject->SetActive(false);
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
		//xmf3Adjust = Vector3::Add(xmf3Adjust, Vector3::Multiply(0.0f, transform->GetUpVector()));
		xmf3Adjust = Vector3::Add(xmf3Adjust, Vector3::Multiply(0.08f, transform->GetLookVector()));
		transform->Translate(xmf3Adjust);


		//-0.028f, 0.0f, 0.08f
	}
}

void WeaponControllerComponent::Fire()
{
	if (!m_bEnabled) return;

	if (0 >= m_fCurrCooltime) {
		//Create(m_pBullet);
		cout << "WeaponControllerComponent::Fire()\n";

		m_fCurrCooltime = m_fCooltime;
		m_fTryRaycast = true;
	}
}
