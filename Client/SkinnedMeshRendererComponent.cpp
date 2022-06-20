#include "stdafx.h"
#include "SkinnedMeshRendererComponent.h"
#include "Components.h"
#include "Object.h"
#include "Model.h"
#include "Material.h"

SkinnedMeshRendererComponent::SkinnedMeshRendererComponent(
	Object* pObject, 
	ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, 
	D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, 
	D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle)
	:Component(pObject)
{
	UINT ncbElementBytes = ((sizeof(XMFLOAT4X4) + 255) & ~255);

	m_pd3dCBResourceForMesh = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	ncbElementBytes = ((sizeof(CB_BONE_INFO) + 255) & ~255);

	m_pd3dCBResourceForAnimation = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	CreateConstantBufferView(pd3dDevice, d3dCbvCPUDescriptorStartHandle);
	SetCBVGpuHandle(d3dCbvGPUDescriptorStartHandle);
}

SkinnedMeshRendererComponent::~SkinnedMeshRendererComponent()
{
}

void SkinnedMeshRendererComponent::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (!m_bEnabled) return;

	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURE_OBJECTS, m_d3dCbvGPUDescriptorHandleForMesh);
	UINT ncbElementBytes = ((sizeof(XMFLOAT4X4) + 255) & ~255);
	memset(m_pCBMappedWorldTransform, NULL, ncbElementBytes);
	XMMATRIX l_xmmtxWorldTransform = m_pObject->FindComponent<TransformComponent>()->GetWorldTransform();
	XMStoreFloat4x4(m_pCBMappedWorldTransform, XMMatrixTranspose(l_xmmtxWorldTransform));

	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURE_ANIMTRANSFORM, m_d3dCbvGPUDescriptorHandleForAnimation);
	ncbElementBytes = ((sizeof(CB_BONE_INFO) + 255) & ~255);
	memset(m_pCBMappedBonesTransform, NULL, ncbElementBytes);

	XMFLOAT4X4* xmf4x4AnimTransform = m_pObject->FindComponent<AnimatorComponent>()->GetFinalResultAnimationTransform();
	for (int i = 0; i < MAX_BONE_NUM; i++) 
		m_pCBMappedBonesTransform->arrxmf4x4Transform[i] = xmf4x4AnimTransform[i];
	

	g_MaterialMng.SetMaterial(m_strMaterialName.c_str(), pd3dCommandList);
	g_ModelMng.Render(m_strModelName.c_str(), pd3dCommandList);
}

void SkinnedMeshRendererComponent::SetModelByName(const char* strModelName)
{
	m_strModelName = strModelName;
}

void SkinnedMeshRendererComponent::SetMaterialByName(const char* strMaterialName)
{
	m_strMaterialName = strMaterialName;
}

void SkinnedMeshRendererComponent::CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle)
{
	if (nullptr != m_pd3dCBResourceForMesh) {
		D3D12_GPU_VIRTUAL_ADDRESS		d3dGpuVirtualAddress;
		D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
		UINT ncbElementBytes = ((sizeof(XMFLOAT4X4) + 255) & ~255);

		m_pd3dCBResourceForMesh->Map(0, NULL, (void**)&m_pCBMappedWorldTransform);
		d3dGpuVirtualAddress = m_pd3dCBResourceForMesh->GetGPUVirtualAddress();
		d3dCBVDesc.SizeInBytes = ncbElementBytes;
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress;
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorStartHandle);

		d3dCbvCPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
	}
	if (nullptr != m_pd3dCBResourceForAnimation) {
		D3D12_GPU_VIRTUAL_ADDRESS		d3dGpuVirtualAddress;
		D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
		UINT ncbElementBytes = ((sizeof(CB_BONE_INFO) + 255) & ~255);

		m_pd3dCBResourceForAnimation->Map(0, NULL, (void**)&m_pCBMappedBonesTransform);
		d3dGpuVirtualAddress = m_pd3dCBResourceForAnimation->GetGPUVirtualAddress();
		d3dCBVDesc.SizeInBytes = ncbElementBytes;
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress;
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorStartHandle);

		d3dCbvCPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
	}
}

void SkinnedMeshRendererComponent::SetCBVGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle)
{
	m_d3dCbvGPUDescriptorHandleForMesh = d3dCbvGPUDescriptorStartHandle;
	d3dCbvGPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;

	m_d3dCbvGPUDescriptorHandleForAnimation = d3dCbvGPUDescriptorStartHandle;
	d3dCbvGPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
}
