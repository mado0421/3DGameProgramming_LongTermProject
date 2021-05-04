#pragma once

constexpr UINT nMaxBone = 64;

struct Keyframe {
	XMFLOAT4 xmf4QuatRotation;
	XMFLOAT3 xmf3Translation;
};
struct CB_BONE_INFO {
	XMFLOAT4X4 xmf4x4Transform[nMaxBone];
};
struct Bone {
	XMFLOAT4X4 toDressposeInv;
	vector<Keyframe> keys;
};

struct AnimClip {
	string strClipName;
	vector<Bone> vecBone;
	vector<double>vecTimes;
	double fClipLength = 0;
};

class AnimationManager {
public:
	/*========================================================================
	* Initialize Part
	========================================================================*/
	void Initialize();

	void AddAnimClip(const char* fileName, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	bool IsAleadyExist(const char* name) { return m_uomAnimClip.count(name); }
	AnimClip* GetAnimClip(const char* name) { return m_uomAnimClip[name]; }

	/*========================================================================
	* Render Part
	========================================================================*/
private:
	unordered_map<string, AnimClip*> m_uomAnimClip;
};

class AnimationController {
public:
	AnimationController() = delete;
	AnimationController(ID3D12Device* pd3dDevice, 
		ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& cbvCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& cbvGpuHandle);
	~AnimationController() { if (m_pd3dCBResource) m_pd3dCBResource->Release(); }

public:
	void Update(float fTimeElapsed);
	void SetMatrix(ID3D12GraphicsCommandList* pd3dCommandList);
	void InterpolateKeyframe(Keyframe k0, Keyframe k1, Keyframe k2, Keyframe k3, float t, Keyframe& out);

	void Test_SetTime(float t) { m_fTime = t; }

protected:
	XMFLOAT4 Interpolate(const XMFLOAT4 q0, const XMFLOAT4 q1, const XMFLOAT4 q2, const XMFLOAT4 q3, float t);
	XMFLOAT3 Interpolate(const XMFLOAT3 v0, const XMFLOAT3 v1, const XMFLOAT3 v2, const XMFLOAT3 v3, float t);

protected:
	float			m_fTime;

	ID3D12Resource* m_pd3dCBResource = NULL;
	CB_BONE_INFO*	m_pCBMappedBones = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorHandle;
};