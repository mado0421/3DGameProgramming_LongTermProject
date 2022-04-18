#pragma once
#include"stdafx.h"
class Framework;

class Scene
{
public:
	virtual void Init(Framework* pFramework, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) = 0;
	virtual void Render(D3D12_CPU_DESCRIPTOR_HANDLE hBckBufRtv, D3D12_CPU_DESCRIPTOR_HANDLE hBckBufDsv) = 0;
	virtual void Update(float fTimeElapsed) = 0;
	virtual void Input(UCHAR* pKeyBuffer) = 0;
};

