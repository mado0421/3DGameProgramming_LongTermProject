#pragma once
#include "Animation.h"
#include "State.h"

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
	virtual void Input(UCHAR* pKeyBuffer);
	virtual void Move(const XMFLOAT3 xmf3Vector);
	virtual void Rotate(const XMFLOAT3 xmf3Vector);
public:
	virtual void SetPosition(const XMFLOAT3 xmf3Position) { 
		m_xmf4x4Local._41 = xmf3Position.x; 
		m_xmf4x4Local._42 = xmf3Position.y;
		m_xmf4x4Local._43 = xmf3Position.z;
	}
	virtual void SetParent(Object* pObject) { m_pParent = pObject; }
	virtual void SetState(const char* strStateName) {};
	virtual void SetVelocity(const XMFLOAT3 xmf3Vector) { m_xmf3Velocity = xmf3Vector; }

	virtual XMMATRIX const GetWorldTransform() {
		if (m_pParent) return XMMatrixMultiply(XMLoadFloat4x4(&m_xmf4x4Local), m_pParent->GetWorldTransform());
		else return XMLoadFloat4x4(&m_xmf4x4Local);
	}
	virtual XMMATRIX const GetBoneMatrix(int boneIdx) { return XMMatrixIdentity(); }
	virtual XMFLOAT3 const GetLook() { return XMFLOAT3(m_xmf4x4Local._31, m_xmf4x4Local._32, m_xmf4x4Local._33); }
	virtual XMFLOAT3 const GetVelocity() { return m_xmf3Velocity; }
	virtual XMFLOAT3 const GetPosition() { return XMFLOAT3(m_xmf4x4Local._41, m_xmf4x4Local._42, m_xmf4x4Local._43); }

protected:
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorHandle;
	ID3D12Resource*				m_pd3dCBResource;
	CB_OBJECT_INFO*				m_pCBMappedObjects;
	string model;
	string material;

protected:
	XMFLOAT4X4	m_xmf4x4Local;
	double		m_time;
	XMFLOAT3	m_xmf3Velocity;
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

class AnimatedObject : public Object {
public:
	AnimatedObject(
		ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);
	~AnimatedObject() {
		if (m_pToWorldTransform)	delete m_pToWorldTransform;
		if (m_pAnimationTransform)	delete m_pAnimationTransform;
		if (m_pBoneMask)			delete m_pBoneMask;
	}

	virtual void SetState(const char* strStateName);
	virtual XMMATRIX const GetBoneMatrix(int boneIdx);

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update(float fTimeElapsed);

public:
	XMFLOAT4X4* m_pToWorldTransform		= NULL;
	XMFLOAT4X4* m_pAnimationTransform	= NULL;
	int*		m_pBoneMask				= NULL;
};

class HumanoidObject : public AnimatedObject {
public:
	HumanoidObject(
		ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Input(UCHAR* pKeyBuffer);
	virtual void Update(float fTimeElapsed);
	void AddSubState(const char* strStateName);
	void AddAction(const char* strStateName);
	void QuitSubState(const char* strStateName);
	void QuitAction(const char* strStateName);

private:
	HumanoidState* m_pCurState = NULL;
	unordered_map<string, HumanoidState*> m_uomStates;
	unordered_map<string, HumanoidState*> m_vecSubStates;
	unordered_map<string, HumanoidState*> m_vecActions;
};