#pragma once
#include "Component.h"

class ColliderComponent : public Component
{
public:
	ColliderComponent() = delete;
	ColliderComponent(
		Object* pObject,
		const char* strTag, 
		const XMFLOAT3& xmf3Center, 
		const XMFLOAT3& xmf3Extents,
		const XMFLOAT4& xmf4Orientation);
	ColliderComponent(
		Object* pObject, 
		const char* strTag,
		const XMFLOAT3& xmf3Center,
		const float fRadius);
	~ColliderComponent();

	virtual void Update(float fTimeElapsed);

	void CheckCollision(ColliderComponent* other);

protected:
	bool isIntersect(const ColliderComponent& other);

public:
	BoundingOrientedBox*	m_pBox		= nullptr;
	BoundingSphere*			m_pSh		= nullptr;
	string					m_strTag	= "";

protected:
	vector<ColliderComponent*> m_vecpCollidedCollider;
};

