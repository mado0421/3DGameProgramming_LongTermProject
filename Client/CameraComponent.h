#pragma once
#include "Component.h"

class CameraComponent : public Component
{
public:
	CameraComponent() = delete;
	CameraComponent(Object* pObject);
	~CameraComponent();

public:
	void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList);
	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);
	void SetLookAtWorldPos(const XMFLOAT3& xmf3LookAtWorld);
	void SetLookAtWorldPos(const float& x, const float& y, const float& z);
	void SetFocusObject(Object* pObject);
	void SetFocusDisable();

	const XMFLOAT4X4 GetViewMatrix();
	const XMFLOAT4X4 GetProjectionMatrix();

	virtual void Update(float fTimeElapsed);

protected:
	void CalculateViewMatrix();
	void CalculateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);

protected:
	XMFLOAT4X4			m_xmf4x4View;
	XMFLOAT4X4			m_xmf4x4Projection;

	XMFLOAT3			m_xmf3Right;
	XMFLOAT3			m_xmf3Up;
	XMFLOAT3			m_xmf3Look;

	D3D12_VIEWPORT		m_d3dViewport;
	D3D12_RECT			m_d3dScissorRect;

	Object*		m_pFocusObject;
};

