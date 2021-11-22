#include "stdafx.h"
#include "Transform.h"


Transform::Transform(Object* pObject)
	: Component(pObject, "Transform")
	, m_xmf4x4Local(Matrix4x4::Identity())
{
}
Transform::~Transform()
{
}

void Transform::SetPosition(const XMFLOAT3& xmf3Position)
{
	m_xmf4x4Local._41 = xmf3Position.x;
	m_xmf4x4Local._42 = xmf3Position.y;
	m_xmf4x4Local._43 = xmf3Position.z;
}
void Transform::SetPosition(const float& fX, const float& fY, const float& fZ)
{
	m_xmf4x4Local._41 = fX;
	m_xmf4x4Local._42 = fY;
	m_xmf4x4Local._43 = fZ;
}
void Transform::Translate(const XMFLOAT3& xmf3Val)
{
	m_xmf4x4Local._41 += xmf3Val.x;
	m_xmf4x4Local._42 += xmf3Val.y;
	m_xmf4x4Local._43 += xmf3Val.z;
}
void Transform::Translate(const float& fX, const float& fY, const float& fZ)
{
	m_xmf4x4Local._41 += fX;
	m_xmf4x4Local._42 += fY;
	m_xmf4x4Local._43 += fZ;
}
void Transform::RotateXYZDegree(const XMFLOAT3& xmf3Val)
{
	XMMATRIX xmmtxRotate 
		= XMMatrixRotationQuaternion(
			XMQuaternionRotationRollPitchYaw(
				XMConvertToRadians(-xmf3Val.x),
				XMConvertToRadians(-xmf3Val.y),
				XMConvertToRadians(-xmf3Val.z)
			)
		);
	XMFLOAT3 xmf3Right	= XMFLOAT3(m_xmf4x4Local._11, m_xmf4x4Local._12, m_xmf4x4Local._13);
	XMFLOAT3 xmf3Up		= XMFLOAT3(m_xmf4x4Local._21, m_xmf4x4Local._22, m_xmf4x4Local._23);
	XMFLOAT3 xmf3Look	= XMFLOAT3(m_xmf4x4Local._31, m_xmf4x4Local._32, m_xmf4x4Local._33);

	xmf3Look	= Vector3::TransformNormal(xmf3Look, xmmtxRotate);
	xmf3Right	= Vector3::TransformNormal(xmf3Right, xmmtxRotate);

	xmf3Look	= Vector3::Normalize(xmf3Look);
	xmf3Right	= Vector3::CrossProduct(xmf3Up, xmf3Look, true);
	xmf3Up		= Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4Local._11 = xmf3Right.x;	m_xmf4x4Local._12 = xmf3Right.y;	m_xmf4x4Local._13 = xmf3Right.z;
	m_xmf4x4Local._21 = xmf3Up.x;		m_xmf4x4Local._22 = xmf3Up.y;		m_xmf4x4Local._23 = xmf3Up.z;
	m_xmf4x4Local._31 = xmf3Look.x;		m_xmf4x4Local._32 = xmf3Look.y;		m_xmf4x4Local._33 = xmf3Look.z;
}
void Transform::RotateXYZDegree(const float& fX, const float& fY, const float& fZ)
{
	XMMATRIX xmmtxRotate
		= XMMatrixRotationQuaternion(
			XMQuaternionRotationRollPitchYaw(
				XMConvertToRadians(-fX),
				XMConvertToRadians(-fY),
				XMConvertToRadians(-fZ)
			)
		);

	XMFLOAT3 xmf3Right	= XMFLOAT3(m_xmf4x4Local._11, m_xmf4x4Local._12, m_xmf4x4Local._13);
	XMFLOAT3 xmf3Up		= XMFLOAT3(m_xmf4x4Local._21, m_xmf4x4Local._22, m_xmf4x4Local._23);
	XMFLOAT3 xmf3Look	= XMFLOAT3(m_xmf4x4Local._31, m_xmf4x4Local._32, m_xmf4x4Local._33);

	xmf3Look	= Vector3::TransformNormal(xmf3Look, xmmtxRotate);
	xmf3Right	= Vector3::TransformNormal(xmf3Right, xmmtxRotate);

	xmf3Look	= Vector3::Normalize(xmf3Look);
	xmf3Right	= Vector3::CrossProduct(xmf3Up, xmf3Look, true);
	xmf3Up		= Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4Local._11 = xmf3Right.x;	m_xmf4x4Local._12 = xmf3Right.y;	m_xmf4x4Local._13 = xmf3Right.z;
	m_xmf4x4Local._21 = xmf3Up.x;		m_xmf4x4Local._22 = xmf3Up.y;		m_xmf4x4Local._23 = xmf3Up.z;
	m_xmf4x4Local._31 = xmf3Look.x;		m_xmf4x4Local._32 = xmf3Look.y;		m_xmf4x4Local._33 = xmf3Look.z;
}

XMMATRIX Transform::GetLocalTransform()
{
	return XMLoadFloat4x4(&m_xmf4x4Local);
}
XMFLOAT3 const Transform::GetLookVector()
{
	return XMFLOAT3(m_xmf4x4Local._31, m_xmf4x4Local._32, m_xmf4x4Local._33);
}
XMFLOAT3 const Transform::GetPosition()
{
	return XMFLOAT3(m_xmf4x4Local._41, m_xmf4x4Local._42, m_xmf4x4Local._43);
}
