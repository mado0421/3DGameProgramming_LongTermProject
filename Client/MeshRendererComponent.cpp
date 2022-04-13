#include "stdafx.h"
#include "Components.h"
#include "Object.h"
#include "Model.h"
#include "Material.h"

MeshRendererComponent::MeshRendererComponent(
	Object* pObject,
	ID3D12Device* pd3dDevice, 
	ID3D12GraphicsCommandList* pd3dCommandList,
	D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, 
	D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle)
	:Component(pObject)
{
	UINT ncbElementBytes = ((sizeof(XMFLOAT4X4) + 255) & ~255);

	m_pd3dCBResource = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	CreateConstantBufferView(pd3dDevice, d3dCbvCPUDescriptorStartHandle);
	SetCBVGpuHandle(d3dCbvGPUDescriptorStartHandle);
}

MeshRendererComponent::~MeshRendererComponent()
{
}

void MeshRendererComponent::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (!m_bEnabled) return;

	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURE_OBJECTS, m_d3dCbvGPUDescriptorHandle);
	UINT ncbElementBytes = ((sizeof(XMFLOAT4X4) + 255) & ~255);
	memset(m_pCBMappedWorldTransform, NULL, ncbElementBytes);

	XMStoreFloat4x4(
		m_pCBMappedWorldTransform,
		XMMatrixTranspose(m_pObject->FindComponent<TransformComponent>()->GetWorldTransform()));

	g_MaterialMng.SetMaterial(m_strMaterialName.c_str(), pd3dCommandList);
	g_ModelMng.Render(m_strModelName.c_str(), pd3dCommandList);
}

void MeshRendererComponent::SetModelByName(const char* strModelName)
{
	m_strModelName = strModelName;
}

void MeshRendererComponent::SetMaterialByName(const char* strMaterialName)
{
	m_strMaterialName = strMaterialName;
}

void MeshRendererComponent::CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle)
{
	D3D12_GPU_VIRTUAL_ADDRESS		d3dGpuVirtualAddress;
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	UINT ncbElementBytes = ((sizeof(XMFLOAT4X4) + 255) & ~255);

	if (nullptr != m_pd3dCBResource) {
		m_pd3dCBResource->Map(0, NULL, (void**)&m_pCBMappedWorldTransform);
		d3dGpuVirtualAddress = m_pd3dCBResource->GetGPUVirtualAddress();
		d3dCBVDesc.SizeInBytes = ncbElementBytes;
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress;
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorStartHandle);

		d3dCbvCPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
	}
}

void MeshRendererComponent::SetCBVGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle)
{
	m_d3dCbvGPUDescriptorHandle			= d3dCbvGPUDescriptorStartHandle;
	d3dCbvGPUDescriptorStartHandle.ptr	+= gnCbvSrvDescriptorIncrementSize;
}
