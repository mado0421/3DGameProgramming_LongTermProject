#include "stdafx.h"
#include "Components.h"
#include "Object.h"

ColliderComponent::ColliderComponent(Object* pObject)
	:Component(pObject)
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
	const XMFLOAT3& xmf3Center,
	const XMFLOAT3& xmf3Extents, 
	const XMFLOAT4& xmf4Orientation)
	:ColliderComponent(pObject)
{
	m_box = BoundingOrientedBox(xmf3Center, xmf3Extents, xmf4Orientation);
}

BoxColliderComponent::~BoxColliderComponent()
{
}

void BoxColliderComponent::Update(float fTimeElapsed)
{
	if (!m_bEnabled) return;

	m_vecpCollided.clear();

	TransformComponent* transform = m_pObject->FindComponent<TransformComponent>();
	m_box.Center		= transform->GetPosition();
	m_box.Orientation	= transform->GetRotationQuaternion();
}

void BoxColliderComponent::CheckCollision(Object* other)
{
	if (!m_bEnabled) return;

	BoxColliders	boxes	= other->FindComponents<BoxColliderComponent>();
	SphereColliders spheres	= other->FindComponents<SphereColliderComponent>();

	for_each(boxes.begin(), boxes.end(), [&](BoxColliderComponent* b) { 
		if (b->m_bEnabled && m_box.Intersects(b->GetBoundingBox())) m_vecpCollided.push_back(b); });

	for_each(spheres.begin(), spheres.end(), [&](SphereColliderComponent* s) {
		if (s->m_bEnabled && m_box.Intersects(s->GetBoundingSphere())) m_vecpCollided.push_back(s); });
}

BoundingOrientedBox& BoxColliderComponent::GetBoundingBox()
{
	return m_box;
}

SphereColliderComponent::SphereColliderComponent(
	Object* pObject,
	const XMFLOAT3& xmf3Center, 
	const float fRadius)
	:ColliderComponent(pObject)
{
	m_sphere = BoundingSphere(xmf3Center, fRadius);
}

SphereColliderComponent::~SphereColliderComponent()
{
}

void SphereColliderComponent::Update(float fTimeElapsed)
{
	if (!m_bEnabled) return;

	m_vecpCollided.clear();

	TransformComponent* transform = m_pObject->FindComponent<TransformComponent>();
	m_sphere.Center = transform->GetPosition();
}

void SphereColliderComponent::CheckCollision(Object* other)
{
	if (!m_bEnabled) return;

	BoxColliders	boxes = other->FindComponents<BoxColliderComponent>();
	SphereColliders spheres = other->FindComponents<SphereColliderComponent>();

	for_each(boxes.begin(), boxes.end(), [&](BoxColliderComponent* b) {
		if (b->m_bEnabled && m_sphere.Intersects(b->GetBoundingBox())) m_vecpCollided.push_back(b); });

	for_each(spheres.begin(), spheres.end(), [&](SphereColliderComponent* s) {
		if (s->m_bEnabled && m_sphere.Intersects(s->GetBoundingSphere())) m_vecpCollided.push_back(s); });
}

BoundingSphere& SphereColliderComponent::GetBoundingSphere()
{
	return m_sphere;
}
