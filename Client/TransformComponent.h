#pragma once
#include "Component.h"

/*
	TransformComponent Component is Component for Object Coordnation.
*/

enum class Space {
	local = false,
	world = true
};

class TransformComponent : public Component
{
public:
	TransformComponent() = delete;
	TransformComponent(Object* pObject);
	~TransformComponent();

	void SetLocalTransform(const XMMATRIX& xmmtxTransform);
	void SetLocalTransform(const XMFLOAT4X4& xmf4x4Transform);
	void SetPosition(const XMFLOAT3& xmf3Position);
	void SetPosition(float fX, float fY, float fZ);
	void Translate(const XMFLOAT3& xmf3Val);
	void Translate(float fX, float fY, float fZ);
	void RotateXYZDegree(const XMFLOAT3& xmf3Val);
	void RotateXYZDegree(float fX, float fY, float fZ);

	XMMATRIX GetLocalTransform();
	XMMATRIX GetWorldTransform();
	XMFLOAT3 const GetLookVector(Space space = Space::local);
	XMFLOAT3 const GetUpVector(Space space = Space::local);
	XMFLOAT3 const GetRightVector(Space space = Space::local);
	XMFLOAT3 const GetPosition(Space space = Space::local);
	XMFLOAT4 const GetRotationQuaternion(Space space = Space::local);

protected:
	XMFLOAT4X4	m_xmf4x4Local;
};

