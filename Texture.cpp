#include "stdafx.h"
#include "Texture.h"

/*========================================================================
* Texture
*=======================================================================*/
void TempTexture::SetByDepthBuffer(
	ID3D12Device* pd3dDevice, UINT width, UINT height,
	D3D12_CPU_DESCRIPTOR_HANDLE& dsvCpuHandle,
	D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
	D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle)
{
	if (!IsTexTypeIsNone()) { assert(!"이미 다른 용도로 만든 텍스처를 깊이버퍼 용도로 재설정 하려 하였습니다.\n"); return; }
	m_TextureType = TextureType::DSV_SRV;

	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension			= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.DepthOrArraySize	= 1;
	d3dResourceDesc.Alignment			= 0;
	d3dResourceDesc.Width				= width;
	d3dResourceDesc.Height				= height;
	d3dResourceDesc.MipLevels			= 1;
	d3dResourceDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count	= 1;
	d3dResourceDesc.SampleDesc.Quality	= 0;
	d3dResourceDesc.Layout				= D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags				= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

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
		D3D12_RESOURCE_STATE_GENERIC_READ, &d3dClearValue, __uuidof(ID3D12Resource), (void**)&m_pd3dTexture);

	D3D12_TEX2D_DSV tex;
	tex.MipSlice = 0;

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D = tex;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	pd3dDevice->CreateDepthStencilView(m_pd3dTexture, &dsvDesc, dsvCpuHandle);
	m_d3dDsvCPUDescriptorHandle = dsvCpuHandle;
	dsvCpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping			= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format							= DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension					= D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip		= 0;
	srvDesc.Texture2D.MipLevels				= 1;
	srvDesc.Texture2D.ResourceMinLODClamp	= 0.0f;
	srvDesc.Texture2D.PlaneSlice			= 0;

	pd3dDevice->CreateShaderResourceView(m_pd3dTexture, &srvDesc, srvCpuHandle);
	srvCpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;

	m_d3dSrvGPUDescriptorHandle = srvGpuHandle;
	srvGpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;
}
void TempTexture::SetByDepthBuffer(ID3D12Device* pd3dDevice, UINT width, UINT height, D3D12_CPU_DESCRIPTOR_HANDLE& dsvCpuHandle, D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle, UINT nArraySize)
{
	if (!IsTexTypeIsNone()) { assert(!"이미 다른 용도로 만든 텍스처를 깊이버퍼 용도로 재설정 하려 하였습니다.\n"); return; }
	m_TextureType = TextureType::DSV_SRV;

	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension			= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.DepthOrArraySize	= nArraySize;
	d3dResourceDesc.Alignment			= 0;
	d3dResourceDesc.Width				= width;
	d3dResourceDesc.Height				= height;
	d3dResourceDesc.MipLevels			= 1;
	d3dResourceDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count	= 1;
	d3dResourceDesc.SampleDesc.Quality	= 0;
	d3dResourceDesc.Layout				= D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags				= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

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
		D3D12_RESOURCE_STATE_GENERIC_READ, &d3dClearValue, __uuidof(ID3D12Resource), (void**)&m_pd3dTexture);

	D3D12_TEX2D_ARRAY_DSV tex;
	tex.MipSlice		= 0;
	tex.ArraySize		= nArraySize;
	tex.FirstArraySlice = 0;

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format			= DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension	= D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvDesc.Texture2DArray	= tex;
	dsvDesc.Flags			= D3D12_DSV_FLAG_NONE;

	pd3dDevice->CreateDepthStencilView(m_pd3dTexture, &dsvDesc, dsvCpuHandle);
	m_d3dDsvCPUDescriptorHandle = dsvCpuHandle;
	dsvCpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping				= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format								= DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension						= D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.ArraySize			= nArraySize;
	srvDesc.Texture2DArray.FirstArraySlice		= 0;
	srvDesc.Texture2DArray.MipLevels			= 1;
	srvDesc.Texture2DArray.MostDetailedMip		= 0;
	srvDesc.Texture2DArray.PlaneSlice			= 0;
	srvDesc.Texture2DArray.ResourceMinLODClamp	= 0.0f;

	pd3dDevice->CreateShaderResourceView(m_pd3dTexture, &srvDesc, srvCpuHandle);
	srvCpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;

	m_d3dSrvGPUDescriptorHandle = srvGpuHandle;
	srvGpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;
}
void TempTexture::SetByRenderTarget(
	ID3D12Device* pd3dDevice, UINT width, UINT height,
	D3D12_CPU_DESCRIPTOR_HANDLE& rtvCpuHandle,
	D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
	D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle)
{
	if (!IsTexTypeIsNone()) { assert(!"이미 다른 용도로 만든 텍스처를 렌더타겟 용도로 재설정 하려 하였습니다.\n"); return; }
	m_TextureType = TextureType::RTV_SRV;

	HRESULT hr = E_FAIL;

	D3D12_RESOURCE_DESC desc = {};
	desc.Width				= width;
	desc.Height				= height;
	desc.MipLevels			= 1;
	desc.DepthOrArraySize	= 1;
	desc.Format				= DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.Flags				= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	desc.SampleDesc.Count	= 1;
	desc.SampleDesc.Quality = 0;
	desc.Dimension			= D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	D3D12_HEAP_PROPERTIES hp;
	hp.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hp.MemoryPoolPreference	= D3D12_MEMORY_POOL_UNKNOWN;
	hp.CreationNodeMask		= 1;
	hp.VisibleNodeMask		= 1;
	hp.Type					= D3D12_HEAP_TYPE_DEFAULT;

	hr = pd3dDevice->CreateCommittedResource(
		&hp, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr, IID_PPV_ARGS(&m_pd3dTexture));

	pd3dDevice->CreateRenderTargetView(m_pd3dTexture, NULL, rtvCpuHandle);
	m_d3dRtvCPUDescriptorHandle = rtvCpuHandle;
	rtvCpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;

	D3D12_RESOURCE_DESC d3dResourceDesc = m_pd3dTexture->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc
		= GetShaderResourceViewDesc(d3dResourceDesc, RESOURCE_TEXTURE2D);
	pd3dDevice->CreateShaderResourceView(m_pd3dTexture, &d3dShaderResourceViewDesc, srvCpuHandle);
	srvCpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;

	m_d3dSrvGPUDescriptorHandle = srvGpuHandle;
	srvGpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;
}
void TempTexture::LoadFromFile(
	const wchar_t* pszFileName,
	ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
	D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle) 
{
	if (!IsTexTypeIsNone()) { assert(!"이미 다른 용도로 만든 텍스처를 쉐이더 리소스 전용 용도로 재설정 하려 하였습니다.\n"); return; }
	m_TextureType = TextureType::ONLY_SRV;

	m_pd3dTexture = CreateTextureResourceFromFile(pd3dDevice, pd3dCommandList, pszFileName, &m_pd3dUploadBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	
	D3D12_RESOURCE_DESC d3dResourceDesc = m_pd3dTexture->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc
		= GetShaderResourceViewDesc(d3dResourceDesc, RESOURCE_TEXTURE2D);
	pd3dDevice->CreateShaderResourceView(m_pd3dTexture, &d3dShaderResourceViewDesc, srvCpuHandle);

	m_d3dSrvGPUDescriptorHandle = srvGpuHandle;
	srvCpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;
	srvGpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;
}

ID3D12Resource* TempTexture::GetTextureResource() {
	if (IsTexTypeIsNone()) { assert(!"아직 설정되지 않은 텍스처입니다.\n"); return nullptr; }
	return m_pd3dTexture;
}
D3D12_GPU_DESCRIPTOR_HANDLE TempTexture::GetSrvGPUHandle() {
	if (IsTexTypeIsNone()) assert(!"아직 설정되지 않은 텍스처입니다.\n");
	return m_d3dSrvGPUDescriptorHandle;
}
D3D12_CPU_DESCRIPTOR_HANDLE TempTexture::GetDsvCPUHandle() {
	if(m_TextureType != TextureType::DSV_SRV) assert(!"깊이버퍼 용도로 설정되지 않은 텍스처입니다.\n");
	return m_d3dDsvCPUDescriptorHandle;
}
D3D12_CPU_DESCRIPTOR_HANDLE TempTexture::GetRtvCPUHandle() {
	if (m_TextureType != TextureType::RTV_SRV) assert(!"렌더타겟 용도로 설정되지 않은 텍스처입니다.\n");
	return m_d3dRtvCPUDescriptorHandle;
}

/*========================================================================
* Texture Manager
*=======================================================================*/
TextureManager::TextureManager(ID3D12Device* pd3dDevice)
	: m_pd3dDsvDescriptorHeap(nullptr)
{
	CreateDsvDescriptorHeap(pd3dDevice);
	CreateRtvDescriptorHeap(pd3dDevice);
	m_d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
}
void TextureManager::AddDepthBufferTexture(
	const char* name, ID3D12Device* pd3dDevice, UINT width, UINT height, 
	D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
	D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle)
{
	TempTexture* temp = new TempTexture();
	temp->SetByDepthBuffer(pd3dDevice, width, height, m_d3dDsvCPUDescriptorHandle, srvCpuHandle, srvGpuHandle);
	
	m_uomTextures[name] = temp;
}
void TextureManager::AddDepthBufferTextureArray(const char* name, UINT nArraySize, ID3D12Device* pd3dDevice, UINT width, UINT height, D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle)
{
	TempTexture* temp = new TempTexture();
	temp->SetByDepthBuffer(pd3dDevice, width, height, m_d3dDsvCPUDescriptorHandle, srvCpuHandle, srvGpuHandle, nArraySize);

	m_uomTextures[name] = temp;
}
void TextureManager::AddRenderTargetTexture(
	const char* name, ID3D12Device* pd3dDevice, UINT width, UINT height,
	D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
	D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle) 
{
	TempTexture* temp = new TempTexture();
	temp->SetByRenderTarget(pd3dDevice, width, height, m_d3dRtvCPUDescriptorHandle, srvCpuHandle, srvGpuHandle);

	m_uomTextures[name] = temp;
}
void TextureManager::LoadFromFile(
	const char* name,
	const wchar_t* pszFileName,
	ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
	D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle)
{
	TempTexture* temp = new TempTexture();
	temp->LoadFromFile(pszFileName, pd3dDevice, pd3dCommandList, srvCpuHandle, srvGpuHandle);

	m_uomTextures[name] = temp;
}
ID3D12Resource* TextureManager::GetTextureResource(const char* name)
{
	return m_uomTextures[name]->GetTextureResource();
}
void TextureManager::UseForShaderResource(const char* name, ID3D12GraphicsCommandList* pd3dCommandList, UINT rootParameterIdx)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(rootParameterIdx, m_uomTextures[name]->GetSrvGPUHandle());
}
void TextureManager::CreateDsvDescriptorHeap(ID3D12Device* pd3dDevice)
{
	HRESULT hr = E_FAIL;

	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors	= MAXNUMSHADOW;
	d3dDescriptorHeapDesc.Type				= D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	d3dDescriptorHeapDesc.Flags				= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask			= 0;
	hr = pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);
}
void TextureManager::CreateRtvDescriptorHeap(ID3D12Device* pd3dDevice)
{
	HRESULT hr = E_FAIL;

	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors	= MAXNUMRENDERTARGETTEXTURE;
	d3dDescriptorHeapDesc.Type				= D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags				= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask			= 0;
	hr = pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dRtvDescriptorHeap);
}
D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::GetDsvCPUHandle(const char* name) {
	return m_uomTextures[name]->GetDsvCPUHandle();
}
D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::GetRtvCPUHandle(const char* name) {
	return m_uomTextures[name]->GetRtvCPUHandle();
}
