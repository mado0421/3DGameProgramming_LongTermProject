#pragma once
#include"stdafx.h"
class Framework;

struct CB_PASS_INFO {
	XMFLOAT4X4	m_xmf4x4CameraView;
	XMFLOAT4X4	m_xmf4x4CameraProjection;
	XMFLOAT4X4	m_xmf4x4CameraViewInv;
	XMFLOAT4X4	m_xmf4x4CameraProjectionInv;
	XMFLOAT4X4	m_xmf4x4TextureTransform;
	XMFLOAT3	m_xmf3CameraPosition;
	float		m_xmfCurrentTime;
};

struct MY_ENV_OBJECT_DATA {
	string strMeshName;
	string strMatName;
	XMFLOAT3 xmf3Position;
	XMFLOAT4 xmf4Rotation;
};

struct MY_COLLIDER_OBJECT_DATA {
	XMFLOAT3 xmf3Position;
	XMFLOAT3 xmf3Extents;
	XMFLOAT4 xmf4Rotation;
};

class Scene
{
public:
	virtual void Init(Framework* pFramework, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) = 0;
	virtual void Render(D3D12_CPU_DESCRIPTOR_HANDLE hBckBufRtv, D3D12_CPU_DESCRIPTOR_HANDLE hBckBufDsv) = 0;
	virtual void Update(float fTimeElapsed) = 0;
	virtual void Input(UCHAR* pKeyBuffer) = 0;
	virtual void ProcessSocket(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
	{
		cout << "Scene OnProcessingWindowMessage\n";
	};
};

