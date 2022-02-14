#pragma once
#include "Component.h"

class BoxColliderComponent;
class SphereColliderComponent;
class Object;
class AnimatorComponent;

using BoxColliders		= vector<BoxColliderComponent*>;
using SphereColliders	= vector<SphereColliderComponent*>;

class ColliderComponent : public Component
{
public:
	ColliderComponent() = delete;
	ColliderComponent(Object* pObject, AnimatorComponent* pAnimator, int boneIdx);
	~ColliderComponent();

	virtual void Update(float fTimeElapsed);

public:
	vector<ColliderComponent*>	m_vecpCollided;
	XMFLOAT4X4					m_xmf4x4Local;

protected:
	AnimatorComponent* m_pAnimator;
	int m_boneIdx;
};

class BoxColliderComponent : public ColliderComponent
{
public:
	BoxColliderComponent() = delete;
	BoxColliderComponent(
		Object* pObject,
		const XMFLOAT3& xmf3Extents,
		AnimatorComponent* pAnimator = nullptr, 
		int boneIdx = 0);
	BoxColliderComponent(
		Object* pObject,
		const XMFLOAT3& xmf3Center,
		const XMFLOAT3& xmf3Extents,
		const XMFLOAT4& xmf4Orientation, 
		AnimatorComponent* pAnimator = nullptr,
		int boneIdx = 0);
	~BoxColliderComponent();

	virtual void Update(float fTimeElapsed);

	virtual void CheckCollision(Component* other);

public:
	BoundingOrientedBox m_box;
};

class SphereColliderComponent : public ColliderComponent
{
public:
	SphereColliderComponent() = delete;
	SphereColliderComponent(
		Object* pObject,
		const float& fRadius,
		AnimatorComponent* pAnimator = nullptr,
		int boneIdx = 0);
	SphereColliderComponent(
		Object* pObject,
		const XMFLOAT3& xmf3Center,
		const float& fRadius,
		AnimatorComponent* pAnimator = nullptr,
		int boneIdx = 0);
	~SphereColliderComponent();

	virtual void Update(float fTimeElapsed);

	virtual void CheckCollision(Component* other);

public:
	BoundingSphere m_sphere;
};