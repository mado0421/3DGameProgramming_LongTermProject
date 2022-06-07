#include "stdafx.h"
#include "CameraComponent.h"
#include "Components.h"
#include "Object.h"

CameraComponent::CameraComponent(Object* pObject)
	:Component(pObject)
	, m_xmf4x4View(Matrix4x4::Identity())
	, m_xmf4x4Projection(Matrix4x4::Identity())
	, m_d3dViewport({ 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f })
	, m_d3dScissorRect({ 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT })
	, m_xmf3Right(1, 0, 0)
	, m_xmf3Up(0, 1, 0)
	, m_xmf3Look(0, 0, 1)
	, m_pFocusObject(nullptr)
{
	CalculateProjectionMatrix(0.1f, 1000.0f, ASPECT_RATIO, 60.0f);

	SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

	t = m_pObject->FindComponent<TransformComponent>();

}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}

void CameraComponent::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
}

void CameraComponent::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_d3dScissorRect.left = xLeft;
	m_d3dScissorRect.top = yTop;
	m_d3dScissorRect.right = xRight;
	m_d3dScissorRect.bottom = yBottom;
}

void CameraComponent::SetLookAtWorldPos(const XMFLOAT3& xmf3LookAt)
{
	XMFLOAT3 xmf3Pos = t->GetPosition(Space::world);

	XMFLOAT4X4 xmf4x4New = Matrix4x4::LookAtLH(xmf3Pos, xmf3LookAt, XMFLOAT3(0, 1, 0));
	//XMFLOAT4X4 xmf4x4New = Matrix4x4::LookAtLH(xmf3Pos, xmf3LookAt, m_xmf3Up);

	m_xmf3Right = XMFLOAT3(xmf4x4New._11, xmf4x4New._21, xmf4x4New._31);
	m_xmf3Up	= XMFLOAT3(xmf4x4New._12, xmf4x4New._22, xmf4x4New._32);
	m_xmf3Look	= XMFLOAT3(xmf4x4New._13, xmf4x4New._23, xmf4x4New._33);
}

void CameraComponent::SetLookAtWorldPos(const float& x, const float& y, const float& z)
{
	SetLookAtWorldPos(XMFLOAT3(x, y, z));
}

void CameraComponent::SetFocusObject(Object* pObject)
{
	m_pFocusObject = pObject;
}

void CameraComponent::SetFocusDisable()
{
	m_pFocusObject = nullptr;
}

const XMFLOAT4X4 CameraComponent::GetViewMatrix()
{
	return m_xmf4x4View;
}

const XMFLOAT4X4 CameraComponent::GetProjectionMatrix()
{
	return m_xmf4x4Projection;
}

void CameraComponent::Update(float fTimeElapsed)
{
	if (!m_bEnabled) return;
	if (m_pFocusObject) {
		SetLookAtWorldPos(m_pFocusObject->FindComponent<TransformComponent>()->GetPosition(Space::world));
	}
	CalculateViewMatrix();
}

void CameraComponent::CalculateViewMatrix()
{
	TransformComponent* transform = t;
	XMFLOAT3 xmf3Position;
	xmf3Position	= transform->GetPosition(Space::world);

	m_xmf3Look	= Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up	= Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);

	m_xmf4x4View._11 = m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x; m_xmf4x4View._13 = m_xmf3Look.x;
	m_xmf4x4View._21 = m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y; m_xmf4x4View._23 = m_xmf3Look.y;
	m_xmf4x4View._31 = m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z; m_xmf4x4View._33 = m_xmf3Look.z;
	m_xmf4x4View._41 = -Vector3::DotProduct(xmf3Position, m_xmf3Right);
	m_xmf4x4View._42 = -Vector3::DotProduct(xmf3Position, m_xmf3Up);
	m_xmf4x4View._43 = -Vector3::DotProduct(xmf3Position, m_xmf3Look);
}

void CameraComponent::CalculateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
}
