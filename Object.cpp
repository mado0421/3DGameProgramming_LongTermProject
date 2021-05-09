#include "stdafx.h"
#include "Object.h"
#include "Mesh.h"
#include "Model.h"
#include "Material.h"
#include "State.h"
#include "Transaction.h"

Object::Object(
	ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle,
	D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle)
{
	m_xmf4x4World	= Matrix4x4::Identity();
	Mesh * pMesh			= new Mesh(pd3dDevice, pd3dCommandList);

	UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);

	m_pd3dCBResource = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	CreateCBV(pd3dDevice, d3dCbvCPUDescriptorStartHandle);
	SetCbvGpuHandle(d3dCbvGPUDescriptorStartHandle);
}

void Object::Update(float fTimeElapsed)
{
	m_time += fTimeElapsed;
}

void Object::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURE_OBJECTS, m_d3dCbvGPUDescriptorHandle);
	UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	memset(m_pCBMappedObjects, NULL, ncbElementBytes);
	XMStoreFloat4x4(&m_pCBMappedObjects->xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

	gMaterialMng.SetMaterial(material.c_str(), pd3dCommandList);
	gModelMng.Render(model.c_str(), pd3dCommandList);
}

void Object::SetCbvGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle)
{
	m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorStartHandle;
	d3dCbvGPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
}

void Object::CreateCBV(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle)
{
	D3D12_GPU_VIRTUAL_ADDRESS		d3dGpuVirtualAddress;
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);

	if (nullptr != m_pd3dCBResource) {
		m_pd3dCBResource->Map(0, NULL, (void**)&m_pCBMappedObjects);
		d3dGpuVirtualAddress = m_pd3dCBResource->GetGPUVirtualAddress();
		d3dCBVDesc.SizeInBytes = ncbElementBytes;
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress;
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorStartHandle);

		d3dCbvCPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
	}
}

void Object::Move(const XMFLOAT3 xmf3Vector)
{
	m_xmf4x4World._41 += xmf3Vector.x;
	m_xmf4x4World._42 += xmf3Vector.y;
	m_xmf4x4World._43 += xmf3Vector.z;
}

void Object::Rotate(const XMFLOAT3 angle)
{
	XMMATRIX xmmtxRotate	= XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(-angle.x), XMConvertToRadians(-angle.y), XMConvertToRadians(-angle.z)));
	XMFLOAT3 xmf3Right		= XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	XMFLOAT3 xmf3Up			= XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	XMFLOAT3 xmf3Look		= XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);

	xmf3Look	= Vector3::TransformNormal(xmf3Look, xmmtxRotate);
	xmf3Right	= Vector3::TransformNormal(xmf3Right, xmmtxRotate);

	xmf3Look	= Vector3::Normalize(xmf3Look);
	xmf3Right	= Vector3::CrossProduct(xmf3Up, xmf3Look, true);
	xmf3Up		= Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4World._11 = xmf3Right.x;	m_xmf4x4World._12 = xmf3Right.y;	m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x;		m_xmf4x4World._22 = xmf3Up.y;		m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x;		m_xmf4x4World._32 = xmf3Look.y;		m_xmf4x4World._33 = xmf3Look.z;
}

DebugWindowObject::DebugWindowObject(
	ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle,
	D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle, 
	bool bIsPass2Screen)
	: Object(pd3dDevice, pd3dCommandList, d3dCbvCPUDescriptorStartHandle, d3dCbvGPUDescriptorStartHandle)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_pDWMesh = new DebugWindowMesh(pd3dDevice, pd3dCommandList, bIsPass2Screen);

	UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);

	m_pd3dCBResource = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	CreateCBV(pd3dDevice, d3dCbvCPUDescriptorStartHandle);
	SetCbvGpuHandle(d3dCbvGPUDescriptorStartHandle);
}

void DebugWindowObject::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURE_OBJECTS, m_d3dCbvGPUDescriptorHandle);
	UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	memset(m_pCBMappedObjects, NULL, ncbElementBytes);
	XMStoreFloat4x4(&m_pCBMappedObjects->xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

	m_pDWMesh->Render(pd3dCommandList);
}

HumanoidObject::HumanoidObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle)
	:AnimatedObject(pd3dDevice, pd3dCommandList, d3dCbvCPUDescriptorStartHandle, d3dCbvGPUDescriptorStartHandle)
{

	HumanoidState_Idle* idle = new HumanoidState_Idle("Humanoid_Idle", this);
	HumanoidState_Walk* walk = new HumanoidState_Walk("Humanoid_Walk", this);

	idle->AddTransation(new Transaction(walk, TV::JUST, NULL, 0));
	walk->AddTransation(new Transaction(idle, TV::SPD, LessThanEqualsTo, 0));

	m_uomStates[idle->m_strStateName] = idle;
	m_uomStates[walk->m_strStateName] = walk;

	m_currState = idle;
}

void HumanoidObject::WalkForward()
{
	m_currState->MoveForward();
}

AnimatedObject::AnimatedObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle)
	:Object(pd3dDevice, pd3dCommandList, d3dCbvCPUDescriptorStartHandle, d3dCbvGPUDescriptorStartHandle)
{
}

void AnimatedObject::SetState(const char* strStateName)
{
	m_currState = m_uomStates[strStateName]; m_currState->EnterState();
}

void AnimatedObject::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	g_AnimCtrl->SetMatrix(pd3dCommandList, m_currState->m_strStateName.c_str(), m_time);
	Object::Render(pd3dCommandList);
}

void AnimatedObject::Update(float fTimeElapsed)
{
	m_time += fTimeElapsed;
	m_currState->Update(fTimeElapsed);
}
