#include "stdafx.h"
#include "Components.h"
#include "Object.h"

ColliderComponent::ColliderComponent(Object* pObject)
	:Component(pObject)
	, m_xmf4x4Local(Matrix4x4::Identity())
{
}

ColliderComponent::~ColliderComponent()
{
}

void ColliderComponent::Update(float fTimeElapsed)
{
	m_vecpCollided.clear();
}

BoxColliderComponent::BoxColliderComponent(
	Object* pObject, 
	const XMFLOAT3& xmf3Extents)
	:ColliderComponent(pObject)
{
	m_box = BoundingOrientedBox(XMFLOAT3(0,0,0), xmf3Extents, XMFLOAT4(0,0,0,1));
}

BoxColliderComponent::BoxColliderComponent(
	Object* pObject,
	const XMFLOAT3& xmf3Center, 
	const XMFLOAT3& xmf3Extents,
	const XMFLOAT4& xmf4Orientation)
	: ColliderComponent(pObject)
{
	m_box = BoundingOrientedBox(xmf3Center, xmf3Extents, xmf4Orientation);
	XMStoreFloat4x4(&m_xmf4x4Local,	XMMatrixRotationQuaternion(XMLoadFloat4(&xmf4Orientation)));
	m_xmf4x4Local._41 = xmf3Center.x;
	m_xmf4x4Local._42 = xmf3Center.y;
	m_xmf4x4Local._43 = xmf3Center.z;
}

BoxColliderComponent::~BoxColliderComponent()
{
}

void BoxColliderComponent::Update(float fTimeElapsed)
{
	if (!m_bEnabled) return;

	m_vecpCollided.clear();

	XMMATRIX world, local;
	XMFLOAT4X4 xmf4x4Result;
	XMFLOAT4 xmf4Orientation;

	local = XMLoadFloat4x4(&m_xmf4x4Local);
	world = m_pObject->FindComponent<TransformComponent>()->GetWorldTransform();

	local = XMMatrixMultiply(world, local);

	XMStoreFloat4x4(&xmf4x4Result, local);
	XMStoreFloat4(&xmf4Orientation, XMQuaternionRotationMatrix(local));

	m_box.Center.x = xmf4x4Result._41;
	m_box.Center.y = xmf4x4Result._42;
	m_box.Center.z = xmf4x4Result._43;

	m_box.Orientation.x = xmf4Orientation.x;
	m_box.Orientation.y = xmf4Orientation.y;
	m_box.Orientation.z = xmf4Orientation.z;
	m_box.Orientation.w	= xmf4Orientation.w;
}

void BoxColliderComponent::CheckCollision(Component* other)
{
	if (!m_bEnabled) return;

	BoxColliderComponent* otherBox = dynamic_cast<BoxColliderComponent*>(other);
	if (otherBox) {
		if (otherBox->m_bEnabled && m_box.Intersects(otherBox->m_box)) m_vecpCollided.push_back(otherBox);
	}
	SphereColliderComponent* otherSphere = dynamic_cast<SphereColliderComponent*>(other);
	if (otherSphere) {
		if (otherSphere->m_bEnabled && m_box.Intersects(otherSphere->m_sphere)) m_vecpCollided.push_back(otherSphere);
	}
}

SphereColliderComponent::SphereColliderComponent(
	Object* pObject, 
	const float& fRadius)
	:ColliderComponent(pObject)
{
	m_sphere = BoundingSphere(XMFLOAT3(0,0,0), fRadius);
}

SphereColliderComponent::SphereColliderComponent(
	Object* pObject,
	const XMFLOAT3& xmf3Center, 
	const float& fRadius)
	: ColliderComponent(pObject)
{
	m_sphere = BoundingSphere(xmf3Center, fRadius);
	m_xmf4x4Local._41 = xmf3Center.x;
	m_xmf4x4Local._42 = xmf3Center.y;
	m_xmf4x4Local._43 = xmf3Center.z;
}

SphereColliderComponent::~SphereColliderComponent()
{
}

void SphereColliderComponent::Update(float fTimeElapsed)
{
	if (!m_bEnabled) return;

	m_vecpCollided.clear();

	XMMATRIX world, local;
	XMFLOAT4X4 xmf4x4Result;

	local = XMLoadFloat4x4(&m_xmf4x4Local);
	world = m_pObject->FindComponent<TransformComponent>()->GetWorldTransform();

	local = XMMatrixMultiply(world, local);

	XMStoreFloat4x4(&xmf4x4Result, local);

	m_sphere.Center.x = xmf4x4Result._41;
	m_sphere.Center.y = xmf4x4Result._42;
	m_sphere.Center.z = xmf4x4Result._43;
}

void SphereColliderComponent::CheckCollision(Component* other)
{
	if (!m_bEnabled) return;

	BoxColliderComponent* otherBox = dynamic_cast<BoxColliderComponent*>(other);
	if (otherBox) {
		if (otherBox->m_bEnabled && m_sphere.Intersects(otherBox->m_box)) m_vecpCollided.push_back(otherBox);
	}
	SphereColliderComponent* otherSphere = dynamic_cast<SphereColliderComponent*>(other);
	if (otherSphere) {
		if (otherSphere->m_bEnabled && m_sphere.Intersects(otherSphere->m_sphere)) m_vecpCollided.push_back(otherSphere);
	}
}
