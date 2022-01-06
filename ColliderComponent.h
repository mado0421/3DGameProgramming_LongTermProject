#pragma once
#include "Component.h"

class BoxColliderComponent;
class SphereColliderComponent;
class Object;

using BoxColliders		= vector<BoxColliderComponent*>;
using SphereColliders	= vector<SphereColliderComponent*>;

class ColliderComponent : public Component
{
public:
	ColliderComponent() = delete;
	ColliderComponent(Object* pObject);
	~ColliderComponent();

	virtual void Update(float fTimeElapsed);

public:
	vector<ColliderComponent*>	m_vecpCollided;
	XMFLOAT4X4					m_xmf4x4Local;
};

class BoxColliderComponent : public ColliderComponent
{
public:
	BoxColliderComponent() = delete;
	BoxColliderComponent(
		Object* pObject,
		const XMFLOAT3& xmf3Extents);
	BoxColliderComponent(
		Object* pObject,
		const XMFLOAT3& xmf3Center,
		const XMFLOAT3& xmf3Extents,
		const XMFLOAT4& xmf4Orientation);
	~BoxColliderComponent();

	virtual void Update(float fTimeElapsed);

	virtual void CheckCollision(Object* other);

public:
	BoundingOrientedBox m_box;
};

class SphereColliderComponent : public ColliderComponent
{
public:
	SphereColliderComponent() = delete;
	SphereColliderComponent(
		Object* pObject,
		const float& fRadius);
	SphereColliderComponent(
		Object* pObject,
		const XMFLOAT3& xmf3Center,
		const float& fRadius);
	~SphereColliderComponent();

	virtual void Update(float fTimeElapsed);

	virtual void CheckCollision(Object* other);

public:
	BoundingSphere m_sphere;
};