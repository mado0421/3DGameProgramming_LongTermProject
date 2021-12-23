#pragma once
#include "Component.h"

/*
	TransformComponent Component is Component for Object Coordnation.
*/
class TransformComponent : public Component
{
public:
	TransformComponent() = delete;
	TransformComponent(Object* pObject);
	~TransformComponent();

	void SetPosition(const XMFLOAT3& xmf3Position);
	void SetPosition(const float& fX, const float& fY, const float& fZ);
	void Translate(const XMFLOAT3& xmf3Val);
	void Translate(const float& fX, const float& fY, const float& fZ);
	void RotateXYZDegree(const XMFLOAT3& xmf3Val);
	void RotateXYZDegree(const float& fX, const float& fY, const float& fZ);

	XMMATRIX GetLocalTransform();
	XMMATRIX GetWorldTransform();
	XMFLOAT3 const GetLookVector();
	XMFLOAT3 const GetPosition();

protected:
	XMFLOAT4X4	m_xmf4x4Local;
};

