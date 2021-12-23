#pragma once
#include "MeshRendererComponent.h"

class SkinnedMeshRendererComponent : public MeshRendererComponent
{
public:
	SkinnedMeshRendererComponent() = delete;
	SkinnedMeshRendererComponent(
		Object* pObject,
		ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);
	~SkinnedMeshRendererComponent();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
};

