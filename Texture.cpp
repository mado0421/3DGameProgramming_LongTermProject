#include "stdafx.h"
#include "Texture.h"

void Texture::CreateSRV(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle)
{
	D3D12_RESOURCE_DESC d3dResourceDesc = m_pd3dTexture->GetDesc();


	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc
		= GetShaderResourceViewDesc(d3dResourceDesc, RESOURCE_TEXTURE2D);
	pd3dDevice->CreateShaderResourceView(m_pd3dTexture, &d3dShaderResourceViewDesc, cpuHandle);

	m_d3dSrvGPUDescriptorHandle = gpuHandle;
	cpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;
	gpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;
}

void Texture::LoadFromFile(
	ID3D12Device* pd3dDevice, 
	ID3D12GraphicsCommandList* pd3dCommandList, 
	const wchar_t* pszFileName,
	D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle,
	D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle)
{
	m_pd3dTexture = CreateTextureResourceFromFile(pd3dDevice, pd3dCommandList, pszFileName, &m_pd3dUploadBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	CreateSRV(pd3dDevice, cpuHandle, gpuHandle);
}

void Texture::CreateEmptyTexture(
	ID3D12Device* pd3dDevice, UINT width, UINT height, 
	D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle,
	D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle)
{
	HRESULT hr = E_FAIL;

	D3D12_RESOURCE_DESC desc	= {};
	desc.Width					= width;
	desc.Height					= height;
	desc.MipLevels				= 1;
	desc.DepthOrArraySize		= 1;
	desc.Format					= DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.Flags					= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	desc.SampleDesc.Count		= 1;
	desc.SampleDesc.Quality		= 0;
	desc.Dimension				= D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	D3D12_HEAP_PROPERTIES hp;
	hp.CPUPageProperty			= D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hp.MemoryPoolPreference		= D3D12_MEMORY_POOL_UNKNOWN;
	hp.CreationNodeMask			= 1;
	hp.VisibleNodeMask			= 1;
	hp.Type						= D3D12_HEAP_TYPE_DEFAULT;

	hr = pd3dDevice->CreateCommittedResource(
		&hp, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr, IID_PPV_ARGS(&m_pd3dTexture));
	// 여기까지 해서 텍스처 리소스 생성.

	CreateSRV(pd3dDevice, cpuHandle, gpuHandle);
}

ID3D12Resource* Texture::GetTexture()
{
	return m_pd3dTexture;
}

D3D12_GPU_DESCRIPTOR_HANDLE Texture::GetGpuHandle()
{
	return m_d3dSrvGPUDescriptorHandle;
}

void DepthTexture::CreateEmptyDSB(
	ID3D12Device* pd3dDevice, UINT width, UINT height,
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHeapCpuHandle, 
	D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle,
	D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle,
	UINT nArraySize)
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.DepthOrArraySize = 1;
	if (nArraySize) {
		d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		d3dResourceDesc.DepthOrArraySize = nArraySize;
	}
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = width;
	d3dResourceDesc.Height = height;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type					= D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference	= D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask		= 1;
	d3dHeapProperties.VisibleNodeMask		= 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth	= 1.0f;
	d3dClearValue.DepthStencil.Stencil	= 0;

	pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, 
		D3D12_RESOURCE_STATE_COPY_DEST, &d3dClearValue, __uuidof(ID3D12Resource), (void**)&m_pd3dTexture);
	// 여기까지 일단 DepthStencilBuffer는 만들었음
	/*
		책보니까 여기서 DEPTH_STENCIL이 아니라 GENERIC_READ로 만들었음.
	
	*/
	
	D3D12_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	::ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	d3dDepthStencilViewDesc.Format			= DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dDepthStencilViewDesc.ViewDimension	= D3D12_DSV_DIMENSION_TEXTURE2D;
	if (nArraySize) d3dDepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY; 
	d3dDepthStencilViewDesc.Flags			= D3D12_DSV_FLAG_NONE;

	pd3dDevice->CreateDepthStencilView(m_pd3dTexture, NULL, depthStencilHeapCpuHandle);
	//// 이건 DSV를 만든거구요. 이거 할 때 DepthStencilDescriptorHeap 주소 필요해서
	//// 받아와서 씀
	//
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format					= DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension			= D3D12_SRV_DIMENSION_TEXTURE2D;
	if (nArraySize) srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels		= 1;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Texture2D.PlaneSlice	= 0;

	pd3dDevice->CreateShaderResourceView(m_pd3dTexture, &srvDesc, cpuHandle);

	//D3D12_RESOURCE_DESC d3dResourceDesc = m_pd3dTexture->GetDesc();
	//D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc
	//	= GetShaderResourceViewDesc(d3dResourceDesc, RESOURCE_TEXTURE2D);
	//pd3dDevice->CreateShaderResourceView(m_pd3dTexture, &d3dShaderResourceViewDesc, cpuHandle);

	m_d3dSrvGPUDescriptorHandle = gpuHandle;
	cpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;
	gpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;
	// 여기까진 됨
	// SRV 만들면 실행 안 됨
	//CreateSRV(pd3dDevice, cpuHandle, gpuHandle);
}
