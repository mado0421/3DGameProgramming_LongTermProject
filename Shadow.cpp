#include "stdafx.h"
#include "Shadow.h"
#include "Texture.h"

//void Shadow::Init(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle)
//{
//	HRESULT hr = E_FAIL;
//
//	//===========================================================================
//	// Create DepthStencil Descriptor Heap
//	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
//	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
//	d3dDescriptorHeapDesc.NumDescriptors = 1;
//	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
//	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//	d3dDescriptorHeapDesc.NodeMask = 0;
//
//	hr = pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);
//	//===========================================================================
//	// Create DepthTexture Array
//	//m_pDepthStencilTexture = new DepthTexture();
//	//m_pDepthStencilTexture->CreateEmptyDSB(
//	//	pd3dDevice, shadowMapWidth, shadowMapWidth,
//	//	m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), cpuHandle, gpuHandle,
//	//	m_nMaxShadowMap);
//}
