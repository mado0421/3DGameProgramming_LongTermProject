#pragma once
class Texture;
class DepthTexture;

class RenderToTextureClass
{
public:
	virtual void Init(
		ID3D12Device* pd3dDevice,
		D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle,
		UINT nTexture);

	virtual void ReadyToPrevPassRender(ID3D12GraphicsCommandList* pd3dCommandList);

	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGpuHandle(UINT i = 0);
	D3D12_GPU_DESCRIPTOR_HANDLE GetDSVGpuHandle();

	Texture* GetTexture(UINT i)		{ return m_vecTextures[i]; }
	DepthTexture* GetDepthTexture() { return m_pDepthStencilTexture; }

protected:
	virtual void CreateRtvDescriptorHeap(ID3D12Device* pd3dDevice, UINT nTexture);
	virtual void CreateDsvDescriptorHeap(ID3D12Device* pd3dDevice);

private:
	vector<Texture*>		m_vecTextures;
protected:
	DepthTexture*			m_pDepthStencilTexture;

protected:
	ID3D12DescriptorHeap*	m_pd3dRtvDescriptorHeap;
	ID3D12DescriptorHeap*	m_pd3dDsvDescriptorHeap;

};

class ShadowMapRenderer : public RenderToTextureClass {
public:
	virtual void Init(
		ID3D12Device* pd3dDevice,
		D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle,
		UINT nArraySize = 1);
	virtual void ReadyToPrevPassRender(ID3D12GraphicsCommandList* pd3dCommandList, UINT idx = 0);

protected:
	virtual void CreateDsvDescriptorHeap(ID3D12Device* pd3dDevice, UINT nArraySize = 1);

};