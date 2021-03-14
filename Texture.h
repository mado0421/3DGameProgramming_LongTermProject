#pragma once
class Texture
{
public:
	void LoadFromFile(
		ID3D12Device* pd3dDevice, 
		ID3D12GraphicsCommandList* pd3dCommandList, 
		const wchar_t* pszFileName,
		D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle);
	void CreateEmptyTexture(
		ID3D12Device* pd3dDevice, UINT width, UINT height, 
		D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle);

	ID3D12Resource* GetTexture();
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle();

protected:
	ID3D12Resource* m_pd3dTexture		= nullptr;
	ID3D12Resource* m_pd3dUploadBuffer	= nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE m_d3dSrvGPUDescriptorHandle;

protected:
	void CreateSRV(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle);
};

class DepthTexture : public Texture
{
public:
	void CreateEmptyDSB(
		ID3D12Device* pd3dDevice, UINT width, UINT height,
		D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHeapCpuHandle,
		D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle,
		UINT nArraySize = 0);

};