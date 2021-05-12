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

public:
	virtual void Move(const XMFLOAT3 xmf3Vector);
	virtual void Rotate(const XMFLOAT3 xmf3Vector);
	virtual void SetPosition(const XMFLOAT3 xmf3Position) { 
		m_xmf4x4Local._41 = xmf3Position.x; 
		m_xmf4x4Local._42 = xmf3Position.y;
		m_xmf4x4Local._43 = xmf3Position.z;
	}
	virtual void SetParent(Object* pObject) { m_pParent = pObject; }
	virtual void SetState(const char* strStateName) {};
	virtual void SetSpeed(float fSpd) { m_fSpeed = fSpd; }
	XMMATRIX const GetWorldTransform() {
		if (m_pParent) return XMMatrixMultiply(XMLoadFloat4x4(&m_xmf4x4Local), m_pParent->GetWorldTransform());
		else return XMLoadFloat4x4(&m_xmf4x4Local);
	}
	virtual XMMATRIX const GetBoneMatrix(int boneIdx) { return XMMatrixIdentity(); }
	virtual XMFLOAT3 const GetLook() { return XMFLOAT3(m_xmf4x4Local._31, m_xmf4x4Local._32, m_xmf4x4Local._33); }
	virtual float const GetSpeed() { return m_fSpeed; }

	virtual void WalkForward() {}



	virtual void PrintTest() {
		cout 
			<< m_xmf4x4Local._41 << ", "
			<< m_xmf4x4Local._42 << ", "
			<< m_xmf4x4Local._43 << "\n";
	}

protected:
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorHandle;
	ID3D12Resource*				m_pd3dCBResource;
	CB_OBJECT_INFO*				m_pCBMappedObjects;
	string model;
	string material;

protected:
	XMFLOAT4X4	m_xmf4x4Local;
	double		m_time;
	float		m_fSpeed;
	Object*		m_pParent;
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
	virtual XMMATRIX const GetBoneMatrix(int boneIdx);

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