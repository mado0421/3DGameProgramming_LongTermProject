#include "stdafx.h"
#include "Camera.h"
#include "Object.h"


Camera::Camera()
	: m_xmf4x4View(Matrix4x4::Identity())
	, m_xmf4x4Projection(Matrix4x4::Identity())
	, m_xmf4x4ViewInv(Matrix4x4::Identity())
	, m_xmf4x4ProjectionInv(Matrix4x4::Identity())
	, m_d3dViewport({ 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f })
	, m_d3dScissorRect({ 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT })
	, m_xmf3Position(XMFLOAT3(0.0f, 0.0f, 0.0f))
	, m_xmf3Right(XMFLOAT3(1.0f, 0.0f, 0.0f))
	, m_xmf3Look(XMFLOAT3(0.0f, 0.0f, 1.0f))
	, m_xmf3Direction(XMFLOAT3(0.0f,0.0f,0.0f))
	, m_xmf3Up(XMFLOAT3(0.0f, 1.0f, 0.0f))
	, m_xmf3Offset(XMFLOAT3(0.0f, 0.0f, 0.0f))
	, m_fTimeLag(0.0f)
	, m_xmf3LookAtWorld(XMFLOAT3(0.0f, 0.0f, 0.0f))
{
}

Camera::~Camera() {}

void Camera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX	= float(xTopLeft);
	m_d3dViewport.TopLeftY	= float(yTopLeft);
	m_d3dViewport.Width		= float(nWidth);
	m_d3dViewport.Height	= float(nHeight);
	m_d3dViewport.MinDepth	= fMinZ;
	m_d3dViewport.MaxDepth	= fMaxZ;
}

void Camera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_d3dScissorRect.left = xLeft;
	m_d3dScissorRect.top = yTop;
	m_d3dScissorRect.right = xRight;
	m_d3dScissorRect.bottom = yBottom;
}

void Camera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
	m_xmf4x4ProjectionInv = Matrix4x4::Inverse(m_xmf4x4Projection);
}

void Camera::GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up)
{
	m_xmf3Position = xmf3Position;
	m_xmf3LookAtWorld = xmf3LookAt;
	m_xmf3Up = xmf3Up;

	GenerateViewMatrix();
}

void Camera::GenerateViewMatrix()
{
	m_xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, m_xmf3LookAtWorld, m_xmf3Up);
	m_xmf4x4ViewInv = Matrix4x4::Inverse(m_xmf4x4View);

}

void Camera::RegenerateViewMatrix()
{
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);

	m_xmf4x4View._11 = m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x; m_xmf4x4View._13 = m_xmf3Look.x;
	m_xmf4x4View._21 = m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y; m_xmf4x4View._23 = m_xmf3Look.y;
	m_xmf4x4View._31 = m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z; m_xmf4x4View._33 = m_xmf3Look.z;
	m_xmf4x4View._41 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Right);
	m_xmf4x4View._42 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Up);
	m_xmf4x4View._43 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Look);
	m_xmf4x4ViewInv = Matrix4x4::Inverse(m_xmf4x4View);

}

void Camera::Update(float fTimeElapsed)
{
	RegenerateViewMatrix();
}

void Camera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}

FollowCamera::FollowCamera() : Camera()
{
	SetTimeLag(0.1f);
	SetOffset(XMFLOAT3(0.0f, 30.0f, -80.0f));	// �÷��̾� ���� ���̿� ���� ���� �ʿ� ����
												// �� �κ��� ���߿� ���ߵǴϱ� �������
	GenerateProjectionMatrix(0.1f, 1000.0f, ASPECT_RATIO, 60.0f);

	SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
}

FollowCamera::~FollowCamera()
{
}
void FollowCamera::SetTarget(void * target)
{
	//m_pTarget = static_cast<CObject*>(target);
	//XMFLOAT3 pos = m_pTarget->GetCameraTargetPos();
	///*�������� ���ϰ� �������� �־� �������� ���� ��Ų �ڿ� LookAt�� ���� ������
	//���� ��ġ�� �ٶ󺸷��� ���� ��ġ�鼭 �ٶ󺸴� ���� ���Ͱ� (0, 0, 0)�� �Ǳ� ������
	//������ ����*/
	//SetLookAt(pos);
}

Object * FollowCamera::GetTarget()
{
	return m_pTarget;
}

void FollowCamera::Update(float fTimeElapsed)
{
	RegenerateViewMatrix();

	Move(Vector3::Multiply(fTimeElapsed, m_xmf3Direction));
	m_xmf3Direction.x = 0;
	m_xmf3Direction.y = 0;
	m_xmf3Direction.z = 0;
}

void FollowCamera::SetLookAt(const XMFLOAT3 & xmf3LookAt)
{
	//XMFLOAT3 up = m_pTarget->GetUp();
	////���⼭ �������̶� LookAt�̶� ������ EyeDir�� 0, 0, 0�̶�� ���� ��

	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, XMFLOAT3(0,1,0));
	m_xmf3Right = XMFLOAT3(mtxLookAt._11, mtxLookAt._21, mtxLookAt._31);
	m_xmf3Up	= XMFLOAT3(mtxLookAt._12, mtxLookAt._22, mtxLookAt._32);
	m_xmf3Look	= XMFLOAT3(mtxLookAt._13, mtxLookAt._23, mtxLookAt._33);
}

BoardCamera::BoardCamera() : Camera()
{
	SetTimeLag(0.0f);
	SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
	GenerateProjectionMatrix(1.01f, 5000.0f);
	//���� �� �ϸ� ���� �������� �� �� ���� �ʾ�?
	//�ƴ�..
	//���� �ϰ� �⺻����̶� �־���� �� �� ������?
	//����..
	SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
}

BoardCamera::~BoardCamera()
{
}

void BoardCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance)
{
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(60.0f), ASPECT_RATIO, fNearPlaneDistance, fFarPlaneDistance);

	//	m_xmf4x4Projection = Matrix4x4::OrthographicLH(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, fNearPlaneDistance, fFarPlaneDistance);
}