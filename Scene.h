#pragma once
#include "stdafx.h"


class Object;
class DebugWindowObject;
class Camera;
class RenderToTextureClass;
class Framework;
class Texture;
class LightManager;

class Scene
{
protected:
	ID3D12RootSignature*		m_pd3dRootSignature;
	ID3D12Device*				m_pd3dDevice;
	ID3D12GraphicsCommandList*	m_pd3dCommandList;
	ID3D12DescriptorHeap*		m_pd3dCbvSrvDescriptorHeap;

	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorStartHandle;

private:
	RenderToTextureClass*						m_pRTTClass;
	vector<DebugWindowObject*>					m_vecDebugWindow;
	vector<Object*>								m_vecObject;
	Camera*										m_pCamera;
	unordered_map<string, ID3D12PipelineState*> m_uomPipelineStates;

protected:
	unordered_map<string, Texture*>				m_uomTextures;
	LightManager*								m_lightMng;

	//Framework*					m_pFramework;
public:

	//ID3D12Resource*				m_pDSBResource;
	//void MakeDSBSRV(ID3D12Resource* resource);
	bool test = false;
	UINT camLightIdx = 0;

public:
	virtual void Init(Framework* pFramework, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void PrevPassRender();
	virtual void Render();
	virtual void Update(float fTimeElapsed);
	virtual void Input(UCHAR* pKeyBuffer, float fTimeElapsed);

protected:
	virtual ID3D12RootSignature* CreateRootSignature();
	virtual void CreateDescriptorHeap();
	virtual void CreateCBV() {}
	virtual void CreateSRV() {}
	virtual void CreatePSO();

};

