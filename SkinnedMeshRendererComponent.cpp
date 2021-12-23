#include "stdafx.h"
#include "SkinnedMeshRendererComponent.h"

SkinnedMeshRendererComponent::SkinnedMeshRendererComponent(
	Object* pObject, 
	ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, 
	D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, 
	D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle)
	:MeshRendererComponent(pObject, pd3dDevice, pd3dCommandList,
		d3dCbvCPUDescriptorStartHandle,
		d3dCbvGPUDescriptorStartHandle)
{
}

SkinnedMeshRendererComponent::~SkinnedMeshRendererComponent()
{
}
