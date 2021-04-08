#pragma once

constexpr UINT nMaxBone = 64;

struct FrameTransform {
	XMFLOAT4 xmf4QuatRotation;
	XMFLOAT3 xmf3Translation;
};
struct CB_BONE_INFO {
	XMFLOAT4X4 xmf4x4Transform[nMaxBone];
};

using FrameEachBone = vector<FrameTransform>;

struct AnimClip {
	string strName;
	UINT nFrame;
	//vecBone.size() == nBone

	vector<FrameEachBone> vecBone;
	ID3D12Resource*		m_pd3dCBResource = NULL;
	CB_BONE_INFO*		m_pCBMappedBones = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorHandle;


	void PrepareCB(
		ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);
	~AnimClip() {
		if (m_pd3dCBResource) m_pd3dCBResource->Release();
	}
};

class AnimationManager {
public:
	void Initialize();

	void SettingCB(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);
	void AddAnimClip(const char* fileName, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void Render(const char* name, double objectTime, ID3D12GraphicsCommandList* pd3dCommandList);
	bool IsAleadyExist(const char* name) { return m_uomAnimClip.count(name); }
private:
	unordered_map<string, AnimClip> m_uomAnimClip;
	string prevAnimClipName = "";
};

/*========================================================================
* 
* 할 것:
* 
* 애니메이션 클립 읽었으니까
* 잘 읽어지는지 확인할 것
* 
* 
========================================================================*/