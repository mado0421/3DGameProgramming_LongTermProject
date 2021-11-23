#include "stdafx.h"
#include "MeshRenderer.h"
#include "Object.h"
#include "Model.h"
#include "Material.h"

MeshRenderer::MeshRenderer(
	Object* pObject,
	ID3D12Device* pd3dDevice, 
	ID3D12GraphicsCommandList* pd3dCommandList,
	D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, 
	D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle)
	:Component(pObject, "MeshRenderer")
{
	UINT ncbElementBytes = ((sizeof(XMFLOAT4X4) + 255) & ~255);

	m_pd3dCBResource = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	CreateConstantBufferView(pd3dDevice, d3dCbvCPUDescriptorStartHandle);
	SetCBVGpuHandle(d3dCbvGPUDescriptorStartHandle);
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURE_OBJECTS, m_d3dCbvGPUDescriptorHandle);
	UINT ncbElementBytes = ((sizeof(XMFLOAT4X4) + 255) & ~255);
	memset(m_pCBMappedWorldTransform, NULL, ncbElementBytes);

	dynamic_cast<Transform*>(m_pObject->FindComponentByName("Transform"))->GetLocalTransform();
		
	XMStoreFloat4x4(
		m_pCBMappedWorldTransform, 
		XMMatrixTranspose(dynamic_cast<Transform*>(m_pObject->FindComponentByName("Transform"))->GetLocalTransform()));

	g_MaterialMng.SetMaterial(m_strMaterialName.c_str(), pd3dCommandList);
	g_ModelMng.Render(m_strModelName.c_str(), pd3dCommandList);
}

void MeshRenderer::CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle)
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

void MeshRenderer::SetCBVGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle)
{
	m_d3dCbvGPUDescriptorHandle			= d3dCbvGPUDescriptorStartHandle;
	d3dCbvGPUDescriptorStartHandle.ptr	+= gnCbvSrvDescriptorIncrementSize;
}
