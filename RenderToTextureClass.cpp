#include "stdafx.h"
#include "RenderToTextureClass.h"
#include "Texture.h"

constexpr int RTTWidth = 1000;

void RenderToTextureClass::Init(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle, int nTexture)
{

	for (int i = 0; i < nTexture; i++) {
		Texture* temp = new Texture();
		temp->CreateEmptyTexture(pd3dDevice, RTTWidth, RTTWidth, cpuHandle, gpuHandle);
		m_vecTextures.push_back(temp);
	}

	CreateRtvDescriptorHeap(pd3dDevice, nTexture);
	CreateDsvDescriptorHeap(pd3dDevice);

	m_pDepthStencilTexture = new DepthTexture();
	m_pDepthStencilTexture->CreateEmptyDSB(pd3dDevice, RTTWidth, RTTWidth, m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), cpuHandle, gpuHandle);

}
void RenderToTextureClass::ReadyToPrevPassRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	
	float pfClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	for (int i = 0; i < m_vecTextures.size(); i++) {
		pd3dCommandList->ClearRenderTargetView(rtvHandle, pfClearColor, 0, NULL);
		rtvHandle.ptr += gnCbvSrvDescriptorIncrementSize;
	}

	rtvHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	pd3dCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	pd3dCommandList->OMSetRenderTargets(m_vecTextures.size(), &rtvHandle, TRUE, &dsvHandle);
}

D3D12_GPU_DESCRIPTOR_HANDLE RenderToTextureClass::GetSRVGpuHandle(int i)
{
	return m_vecTextures[i]->GetGpuHandle();
}

D3D12_GPU_DESCRIPTOR_HANDLE RenderToTextureClass::GetDSVGpuHandle()
{
	return m_pDepthStencilTexture->GetGpuHandle();
}

void RenderToTextureClass::CreateRtvDescriptorHeap(ID3D12Device* pd3dDevice, int nTexture)
{
	HRESULT hr = E_FAIL;

	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = nTexture;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	hr = pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dRtvDescriptorHeap);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (int i = 0; i < nTexture; i++) {
		pd3dDevice->CreateRenderTargetView(m_vecTextures[i]->GetTexture(), NULL, d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += gnCbvSrvDescriptorIncrementSize;
	}
}
void RenderToTextureClass::CreateDsvDescriptorHeap(ID3D12Device* pd3dDevice)
{
	HRESULT hr = E_FAIL;

	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	hr = pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);
}
//void RenderToTextureClass::CreateDSV(ID3D12Device* pd3dDevice)
//{
//	D3D12_RESOURCE_DESC d3dResourceDesc;
//	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
//	d3dResourceDesc.Alignment = 0;
//	d3dResourceDesc.Width = RTTWidth;
//	d3dResourceDesc.Height = RTTWidth;
//	d3dResourceDesc.DepthOrArraySize = 1;
//	d3dResourceDesc.MipLevels = 1;
//	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	d3dResourceDesc.SampleDesc.Count = 1;
//	d3dResourceDesc.SampleDesc.Quality = 0;
//	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
//	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
//
//	D3D12_HEAP_PROPERTIES d3dHeapProperties;
//	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
//	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
//	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
//	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
//	d3dHeapProperties.CreationNodeMask = 1;
//	d3dHeapProperties.VisibleNodeMask = 1;
//
//	D3D12_CLEAR_VALUE d3dClearValue;
//	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	d3dClearValue.DepthStencil.Depth = 1.0f;
//	d3dClearValue.DepthStencil.Stencil = 0;
//
//	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
//	pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void**)&m_pd3dDepthStencilBuffer);
//
//	D3D12_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
//	::ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
//	d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	d3dDepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
//	d3dDepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
//
//	pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, d3dDsvCPUDescriptorHandle);
//}
