#pragma once
#include"stdafx.h"
class Framework;

namespace LoadMy {
	vector<string> Split(istringstream& ss, const char delim);
	vector<MY_ENV_OBJECT_DATA> LoadEnvMeshList(const char* path);
	vector<MY_COLLIDER_OBJECT_DATA> LoadColliderList(const char* path);
}
class Object;
class Screen;
class Camera;
class RenderToTextureClass;
class Framework;
class Texture;
class LightManager;
class TextureManager;
class ClientWsaModule;
class MainScene : public Scene
{
protected:
	ID3D12RootSignature * m_pd3dRootSignature;
	ID3D12Device* m_pd3dDevice;
	ID3D12GraphicsCommandList* m_pd3dCommandList;
	ID3D12DescriptorHeap* m_pd3dCbvSrvDescriptorHeap;

	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorStartHandle;


	vector<Object*>								m_vecObject;

	// for Render
	vector<Object*>								m_vecNonAnimObjectRenderGroup;
	vector<Object*>								m_vecAnimObjectRenderGroup;
	vector<Object*>								m_vecEffectRenderGroup;
	vector<Object*>								m_vecUIRenderGroup;

	// for Particle
	vector<Object*>								m_vecParticleEmitter;
	vector<Object*>								m_vecParticlePool;

	// for Camera
	Object* m_pCameraObject = nullptr;

	vector<Screen*>								m_vecScreenObject;
	unordered_map<string, ID3D12PipelineState*> m_uomPipelineStates;

	LightManager* m_LightMng;

	ID3D12Resource* m_pd3duabHDRAvgLum;
	D3D12_GPU_DESCRIPTOR_HANDLE					m_d3dCbvGPUuabHDRAvgLumHandle;
	ID3D12Resource* m_pd3dcbPassInfo;
	CB_PASS_INFO* m_pcbMappedPassInfo;
	D3D12_GPU_DESCRIPTOR_HANDLE					m_d3dCbvGPUPassInfoHandle;

	Framework* m_pFramework;
	Camera* m_pCamera;
	float										m_fCurrentTime = 0;



public:
	bool test = false;
	int startEndState = 1;	// 0: main, 1: start, 2: end
public:
	virtual void Init(Framework* pFramework, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void CheckCollsion();
	virtual void SolveConstraint();
	virtual void Input(UCHAR* pKeyBuffer);
	virtual void Update(float fTimeElapsed);
	virtual void Render(D3D12_CPU_DESCRIPTOR_HANDLE hBckBufRtv, D3D12_CPU_DESCRIPTOR_HANDLE hBckBufDsv);

	virtual void Release();

protected:
	virtual ID3D12RootSignature* CreateRootSignature();
	virtual void CreateDescriptorHeap();
	virtual void CreateCBV() {}
	virtual void CreateSRV() {}
	virtual void CreatePSO();

	void CreatePassInfoShaderResource();
	void UpdatePassInfoAboutCamera();

	void BuildObject();

protected:
	/*==============================================================================
	* For Test!!
	*
	==============================================================================*/
	void ReloadLight();

protected:
	Object* FindObjectByName(const char*);


	void LoadLevelEnvironment();

	void CreateEnvObject(const char* strModelName, const char* strMaterialName, XMFLOAT3 pos, XMFLOAT4 rot);
	void CreateEnvObject(MY_ENV_OBJECT_DATA objData);

	void CreateCollider(MY_COLLIDER_OBJECT_DATA colData);

	void CreateTargetBoard(const char* strName, XMFLOAT3 position, XMFLOAT3 rotationAngle, bool initialStateDied,
		ID3D12Device* device, ID3D12GraphicsCommandList* commandlist,
		D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle);
};
