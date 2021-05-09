#pragma once
#include "Animation.h"

struct CB_OBJECT_INFO {
	XMFLOAT4X4	xmf4x4World;
};
class Mesh;
class DebugWindowMesh;
class MESH_DATA;

using meshVec = vector<Mesh*>;

class Object
{
	// Initialize
public:
	Object(
		ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, 
		D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);
	virtual ~Object() {
		if (m_pd3dCBResource) m_pd3dCBResource->Release();
		if (m_pCBMappedObjects) delete m_pCBMappedObjects;
	}

protected:
	virtual void SetCbvGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);
	virtual void CreateCBV(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle);

	// External Accsess
public:
	void SetModel(const char* modelName) { model = modelName; }
	void SetMaterial(const char* matName) { material = matName; }

	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void SetState(const char* strStateName) {};
	XMFLOAT3 const GetLook() { return XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33); }
	float const GetSpeed() { return m_fSpeed; }
	void SetSpeed(float fSpd) { m_fSpeed = fSpd; }
	void Move(const XMFLOAT3 xmf3Vector);
	void Rotate(const XMFLOAT3 xmf3Vector);

	virtual void WalkForward() {}

protected:
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorHandle;
	ID3D12Resource*				m_pd3dCBResource;
	CB_OBJECT_INFO*				m_pCBMappedObjects;
	string model;
	string material;

protected:
	XMFLOAT4X4	m_xmf4x4World;
	double		m_time;
	float		m_fSpeed;
};

class DebugWindowObject : public Object {
public:
	DebugWindowObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle,
		bool bIsPass2Screen = false);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

private:
	DebugWindowMesh*			m_pDWMesh;
};

class State;

class AnimatedObject : public Object {
public:
	AnimatedObject(
		ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);

	virtual void SetState(const char* strStateName);

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update(float fTimeElapsed);

protected:
	State* m_currState;
	unordered_map<string, State*> m_uomStates;
};

class HumanoidObject : public AnimatedObject {
public:
	HumanoidObject(
		ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);
	
	virtual void WalkForward();

};