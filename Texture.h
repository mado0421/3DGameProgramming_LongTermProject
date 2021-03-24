#pragma once

#define MAXNUMSHADOW 64
#define MAXNUMRENDERTARGETTEXTURE 8

typedef enum TEXTURETYPE {
	NONE = (UINT)0,
	ONLY_SRV,
	RTV_SRV,
	DSV_SRV
}TextureType;

/*========================================================================
* Texture
*=======================================================================*/
class TempTexture {
public:
	~TempTexture() {
		if (m_pd3dTexture) m_pd3dTexture->Release();
		if (m_pd3dUploadBuffer) m_pd3dUploadBuffer->Release();
	}
	void SetByDepthBuffer(ID3D12Device* pd3dDevice, UINT width, UINT height,
		D3D12_CPU_DESCRIPTOR_HANDLE& dsvCpuHandle,
		D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);
	void SetByDepthBuffer(ID3D12Device* pd3dDevice, UINT width, UINT height,
		D3D12_CPU_DESCRIPTOR_HANDLE& dsvCpuHandle,
		D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle,
		UINT nArraySize);
	void SetByCubeDepthBuffer(ID3D12Device* pd3dDevice, UINT width, UINT height,
		D3D12_CPU_DESCRIPTOR_HANDLE& dsvCpuHandle,
		D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);
	void SetByRenderTarget(ID3D12Device* pd3dDevice, UINT width, UINT height,
		D3D12_CPU_DESCRIPTOR_HANDLE& rtvCpuHandle,
		D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);
	void LoadFromFile(
		const wchar_t* pszFileName,
		ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);

	ID3D12Resource* GetTextureResource();
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGPUHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE GetDsvCPUHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE GetRtvCPUHandle();

private:
	bool IsTexTypeIsNone() { return m_TextureType == TextureType::NONE; }

private:
	TextureType m_TextureType = TextureType::NONE;
	ID3D12Resource* m_pd3dTexture = nullptr;
	ID3D12Resource* m_pd3dUploadBuffer = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE m_d3dSrvGPUDescriptorHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_d3dDsvCPUDescriptorHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_d3dRtvCPUDescriptorHandle;
};

/*========================================================================
* Texture Manager
*=======================================================================*/
using unorderedTextureMap = unordered_map<string, TempTexture*>;

class TextureManager {
public:
	TextureManager() = delete;
	TextureManager(ID3D12Device* pd3dDevice);

	void AddDepthBufferTexture(
		const char* name,
		ID3D12Device* pd3dDevice, UINT width, UINT height,
		D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);
	void AddDepthBufferTextureArray(
		const char* name,
		UINT nArraySize,
		ID3D12Device* pd3dDevice, UINT width, UINT height,
		D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);
	void AddDepthBufferTextureCube(
		const char* name,
		ID3D12Device* pd3dDevice, UINT width, UINT height,
		D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);
	void AddRenderTargetTexture(
		const char* name,
		ID3D12Device* pd3dDevice, UINT width, UINT height,
		D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);
	void LoadFromFile(
		const char* name,
		const wchar_t* pszFileName,
		ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);

	void DeleteTexture(const char* name) {
		m_uomTextures.erase(name);
	}

	ID3D12Resource* GetTextureResource(const char* name);
	void UseForShaderResource(const char* name, ID3D12GraphicsCommandList* pd3dCommandList, UINT rootParameterIdx);

	D3D12_CPU_DESCRIPTOR_HANDLE GetDsvCPUHandle(const char* name);
	D3D12_CPU_DESCRIPTOR_HANDLE GetRtvCPUHandle(const char* name);

private:
	void CreateDsvDescriptorHeap(ID3D12Device* pd3dDevice);
	void CreateRtvDescriptorHeap(ID3D12Device* pd3dDevice);

private:
	ID3D12DescriptorHeap*		m_pd3dDsvDescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_d3dDsvCPUDescriptorHandle;
	ID3D12DescriptorHeap*		m_pd3dRtvDescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_d3dRtvCPUDescriptorHandle;
	unorderedTextureMap			m_uomTextures;
};

