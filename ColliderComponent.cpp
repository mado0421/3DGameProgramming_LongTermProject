#include "stdafx.h"
#include "Components.h"

ColliderComponent::ColliderComponent(
	Object* pObject,
	const XMFLOAT3& xmf3Center,
	const XMFLOAT3& xmf3Extents, 
	const XMFLOAT4& xmf4Orientation)
	:Component(pObject, "Collider")
{
	m_pBox = new BoundingOrientedBox(xmf3Center, xmf3Extents, xmf4Orientation);
}

ColliderComponent::ColliderComponent(
	Object* pObject, 
	const XMFLOAT3& xmf3Center, 
	const float fRadius)
	:Component(pObject, "Collider")
{
	m_pSh = new BoundingSphere(xmf3Center, fRadius);
}

ColliderComponent::~ColliderComponent()
{
	if (m_pBox) delete m_pBox;
	if (m_pSh) delete m_pSh;
}

bool ColliderComponent::isIntersect(const ColliderComponent& other)
{
	if (!m_bEnabled) return false;

	if (m_pBox) {
		if (other.m_pBox)	return m_pBox->Intersects(*other.m_pBox);
		else				return m_pBox->Intersects(*other.m_pSh);
	}
	else {	//m_pBS
		if (other.m_pBox)	return m_pSh->Intersects(*other.m_pBox);
		else				return m_pSh->Intersects(*other.m_pSh);
	}
	return false;
}
