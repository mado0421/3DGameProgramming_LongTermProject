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

class AnimatedObject;
class BoneMask;

// Object한테 vecAnimation을 받아서 toWorld와 animTransform을 채워주는 역할
// Object한테 vecAnimation을 받아서 toWorld와 animTransform에 Blend 해주는 역할
namespace AnimationCalculate {
	void MakeAnimationTransform(const vector<pair<string, float>> vecPairClipNWeight, const float fTime, AnimatedObject* pTargetObj);
	void BlendToAnimaionTransform(const vector<pair<string, float>> vecPairClipNWeight, const float fTime, AnimatedObject* pTargetObj, BoneMask* boneMask = NULL);

	void GetFrameIdxAndNormalizedTime(const char* strClipName, const float fTime, float& OutfNormalizedTime, XMINT4& OutIdx);
	void GetFrontAndBackMtx(const char* strClipName, const float fNormalizedTime, const int boneIdx, const XMINT4 frameIdx, XMMATRIX& xmmtxFront, XMMATRIX& xmmtxBack);

	void InterpolateKeyframe(Keyframe k0, Keyframe k1, Keyframe k2, Keyframe k3, float t, Keyframe& out);
	XMFLOAT3 Interpolate(const XMFLOAT3 v0, const XMFLOAT3 v1, const XMFLOAT3 v2, const XMFLOAT3 v3, float t);
};

// Object한테 vecAnimation을 받아서 animTransform을 GPU에 보내주는 역할
class AnimationUploader {
public:
	AnimationUploader() = delete;
	AnimationUploader(ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& cbvCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& cbvGpuHandle);
	~AnimationUploader() { 
		if (m_pd3dCBResource) m_pd3dCBResource->Release();
		if (m_pCBMappedBones) delete m_pCBMappedBones;
	}

public:
	void SetAnimationTransform(ID3D12GraphicsCommandList* pd3dCommandList, AnimatedObject* pTargetObj);

protected:
	ID3D12Resource*				m_pd3dCBResource = NULL;
	CB_BONE_INFO*				m_pCBMappedBones = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorHandle;
};