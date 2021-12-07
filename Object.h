#pragma once
#include "Animation.h"
#include "State.h"

#include "TransformComponent.h"



struct CB_OBJECT_INFO {
	XMFLOAT4X4	xmf4x4World;
};
class Mesh;
class DebugWindowMesh;
class MESH_DATA;

class Object
{
	// Initialize
public:
	Object();
	virtual ~Object();

public:
	virtual void CheckCollsion(Object& other);
	virtual void SolveConstraint();
	virtual void Input(UCHAR* pKeyBuffer);
	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

public:
	virtual void Move(const XMFLOAT3 xmf3Vector);
	virtual void Rotate(const XMFLOAT3 xmf3Vector);
	virtual void SetParent(Object* pObject) { m_pParent = pObject; }
	virtual void SetState(const char* strStateName) {};
	virtual void SetVelocity(const XMFLOAT3 xmf3Vector) { m_xmf3Velocity = xmf3Vector; }

	//virtual XMMATRIX const GetWorldTransform() {
	//	if (m_pParent) return XMMatrixMultiply(XMLoadFloat4x4(&m_xmf4x4Local), m_pParent->GetWorldTransform());
	//	else return XMLoadFloat4x4(&m_xmf4x4Local);
	//}
	virtual XMMATRIX const GetBoneMatrix(int boneIdx) { return XMMatrixIdentity(); }
	virtual XMFLOAT3 const GetVelocity() { return m_xmf3Velocity; }

public:
	template<typename t>
	t* FindComponent() {
		for (Component* c : m_vecComponents) {
			t* as = dynamic_cast<t*>(c);
			if (nullptr != as) return as;
		}
		return nullptr;
	}

	template<typename t>
	vector<t*> FindComponents() {
		vector<t*> result;

		FindComponentsReq(result);

		return result;
	}

	void AddComponent(Component* component) {
		m_vecComponents.push_back(component);
	}

protected:
	template<typename t>
	void FindComponentsReq(vector<t*>& result) {
		for (Component* c : m_vecComponents) {
			t* as = dynamic_cast<t*>(c);
			if (nullptr != as)
				result.push_back(as);
		}

		for (Object* c : m_vecpChild) c->FindComponentsReq<t>(result);
	}

protected:
	float			m_fTime			= 0;
	XMFLOAT3		m_xmf3Velocity	= XMFLOAT3(0, 0, 0);
	Object*			m_pParent		= nullptr;
	vector<Object*> m_vecpChild;

	vector<Component*> m_vecComponents;
};

class Screen {
public:
	Screen() = delete;
	Screen(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle,
		float width, float height);
	~Screen();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

private:
	Mesh*						m_pScreenMesh				= nullptr;
	ID3D12Resource*				m_pd3dCBResource			= nullptr;
	XMFLOAT4X4*					m_pCBMappedTransform		= nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorHandle;

};

//class AnimatedObject : public Object {
//public:
//	AnimatedObject(
//		ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
//		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle,
//		D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);
//
//public:
//	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
//	virtual void Update(float fTimeElapsed);
//	virtual void Input(UCHAR* pKeyBuffer);
//
//public:
//	virtual XMMATRIX const GetBoneMatrix(int boneIdx);
//	
//	BoneHierarchy		m_boneHierarchyInfo;
//protected:
//	vector<StateLayer*>	m_vecpStateLayer;
//};
//
//class HumanoidObject : public AnimatedObject {
//public:
//	HumanoidObject(
//		ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
//		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle,
//		D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);
//};
//
