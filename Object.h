#pragma once

struct CB_OBJECT_INFO {
	XMFLOAT4X4	xmf4x4World;
};
class Mesh;
class DebugWindowMesh;
class MESH_DATA;

using meshVec = vector<Mesh*>;

struct IComponent;

class Object
{
public:
	Object(
		ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, 
		D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);
	virtual ~Object() {
		if (m_pd3dCBResource) m_pd3dCBResource->Release();
		if (m_pCBMappedObjects) delete m_pCBMappedObjects;
	}

	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void SetCbvGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);
	virtual void CreateCBV(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle);

	void Move(const XMFLOAT3 xmf3Vector);
	void Rotate(const XMFLOAT3 xmf3Vector);
	void SetModel(const char* modelName) { model = modelName; }
	void SetMaterial(const char* matName) { material = matName; }

protected:
	XMFLOAT4X4					m_xmf4x4World;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorHandle;
	ID3D12Resource*				m_pd3dCBResource;
	CB_OBJECT_INFO*				m_pCBMappedObjects;

	string material;
	string model;
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

class AnimationController;
class AnimatedObject : public Object {
public:
	AnimatedObject(
		ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);
	virtual ~AnimatedObject() {
		if (m_AnimCtrl) delete m_AnimCtrl;
	}
	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);


protected:
	AnimationController* m_AnimCtrl;
};