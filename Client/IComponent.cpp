#include "stdafx.h"
#include "IComponent.h"

ITransform::ITransform()
	:m_xmf4x4world(Matrix4x4::Identity())
{
}

void ITransform::Move(XMFLOAT3 value)
{
	m_xmf4x4world._41 += value.x;
	m_xmf4x4world._42 += value.y;
	m_xmf4x4world._43 += value.z;
}

void ITransform::Rotate(XMFLOAT3 angle)
{
	XMMATRIX xmmtxRotate = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(-angle.x), XMConvertToRadians(-angle.y), XMConvertToRadians(-angle.z)));
	XMFLOAT3 xmf3Right = XMFLOAT3(m_xmf4x4world._11, m_xmf4x4world._12, m_xmf4x4world._13);
	XMFLOAT3 xmf3Up = XMFLOAT3(m_xmf4x4world._21, m_xmf4x4world._22, m_xmf4x4world._23);
	XMFLOAT3 xmf3Look = XMFLOAT3(m_xmf4x4world._31, m_xmf4x4world._32, m_xmf4x4world._33);

	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);
	xmf3Right = Vector3::TransformNormal(xmf3Right, xmmtxRotate);

	xmf3Look = Vector3::Normalize(xmf3Look);
	xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);
	xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4world._11 = xmf3Right.x;	m_xmf4x4world._12 = xmf3Right.y;	m_xmf4x4world._13 = xmf3Right.z;
	m_xmf4x4world._21 = xmf3Up.x;		m_xmf4x4world._22 = xmf3Up.y;		m_xmf4x4world._23 = xmf3Up.z;
	m_xmf4x4world._31 = xmf3Look.x;		m_xmf4x4world._32 = xmf3Look.y;		m_xmf4x4world._33 = xmf3Look.z;
}
