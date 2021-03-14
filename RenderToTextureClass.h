#pragma once
class Texture;
class DepthTexture;

class RenderToTextureClass
{
public:
	void Init(
		ID3D12Device* pd3dDevice,
		D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle,
		int nTexture);

	void ReadyToPrevPassRender(ID3D12GraphicsCommandList* pd3dCommandList);

	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGpuHandle(int i = 0);
	D3D12_GPU_DESCRIPTOR_HANDLE GetDSVGpuHandle();

private:
	void CreateRtvDescriptorHeap(ID3D12Device* pd3dDevice, int nTexture);
	void CreateDsvDescriptorHeap(ID3D12Device* pd3dDevice);

public:
	vector<Texture*>		m_vecTextures;
	DepthTexture*			m_pDepthStencilTexture;

private:
	ID3D12DescriptorHeap*	m_pd3dRtvDescriptorHeap;
	ID3D12DescriptorHeap*	m_pd3dDsvDescriptorHeap;

};

