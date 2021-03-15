#include "stdafx.h"
#include "RenderToTextureClass.h"
#include "Texture.h"

constexpr int RTTWidth = 1000;
constexpr int shadowMapWidth = 1000;

void RenderToTextureClass::Init(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle, UINT nTexture)
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
D3D12_GPU_DESCRIPTOR_HANDLE RenderToTextureClass::GetSRVGpuHandle(UINT i)
{
	return m_vecTextures[i]->GetGpuHandle();
}
D3D12_GPU_DESCRIPTOR_HANDLE RenderToTextureClass::GetDSVGpuHandle()
{
	return m_pDepthStencilTexture->GetGpuHandle();
}

void RenderToTextureClass::CreateRtvDescriptorHeap(ID3D12Device* pd3dDevice, UINT nTexture)
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

void ShadowMapRenderer::Init(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle, UINT nArraySize)
{
	CreateDsvDescriptorHeap(pd3dDevice, nArraySize);

	m_pDepthStencilTexture = new DepthTexture();
	m_pDepthStencilTexture->CreateEmptyDSB(pd3dDevice, shadowMapWidth, shadowMapWidth, m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), cpuHandle, gpuHandle, nArraySize);


}
void ShadowMapRenderer::ReadyToPrevPassRender(ID3D12GraphicsCommandList* pd3dCommandList, UINT idx)
{
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	dsvHandle.ptr += gnCbvSrvDescriptorIncrementSize * idx;
	pd3dCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	pd3dCommandList->OMSetRenderTargets(0, NULL, TRUE, &dsvHandle);
}
void ShadowMapRenderer::CreateDsvDescriptorHeap(ID3D12Device* pd3dDevice, UINT nArraySize)
{
	HRESULT hr = E_FAIL;

	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = nArraySize;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	hr = pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);
}