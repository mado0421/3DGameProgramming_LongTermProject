#pragma once
#include "Component.h"

class ColliderComponent : public Component
{
public:
	ColliderComponent() = delete;
	ColliderComponent(
		Object* pObject, 
		const XMFLOAT3& xmf3Center, 
		const XMFLOAT3& xmf3Extents,
		const XMFLOAT4& xmf4Orientation);
	ColliderComponent(
		Object* pObject,
		const XMFLOAT3& xmf3Center,
		const float fRadius);
	~ColliderComponent();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void InputEvent(UCHAR* pKeyBuffer) {}

	bool isIntersect(const ColliderComponent& other);

public:
	BoundingOrientedBox*	m_pBox	= nullptr;
	BoundingSphere*			m_pSh	= nullptr;

protected:
	vector<Object*>			m_vecCollidedObject;
};

