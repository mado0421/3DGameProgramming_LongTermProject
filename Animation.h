#pragma once

constexpr UINT nMaxBone = 64;

class AnimatedObject;
class BoneMask;
class State;
class StateLayer;

using ClipPair = vector<pair<string, float>>;

struct Keyframe {
	XMFLOAT4 xmf4QuatRotation;
	XMFLOAT3 xmf3Translation;
};
struct CB_BONE_INFO {
	XMFLOAT4X4 xmf4x4Transform[nMaxBone];
};
struct Bone {
	XMFLOAT4X4	toDressposeInv;
	XMFLOAT4X4	toParent;
	int			parentIdx;
	vector<Keyframe> keys;
};
struct AnimClip {
	string strClipName;
	vector<Bone> vecBone;
	vector<double>vecTimes;
	double fClipLength = 0;
};
struct BoneHierarchy {
	XMFLOAT4X4	toDressposeInv[64];
	XMFLOAT4X4	toParent[64];
	XMFLOAT4X4	toWorld[64];
	XMFLOAT4X4	local[64];
	int			parentIdx[64];
	int			nBone;

	BoneHierarchy() {
		memset(toDressposeInv, NULL, sizeof(XMFLOAT4X4) * 64);
		memset(toParent, NULL, sizeof(XMFLOAT4X4) * 64);
		memset(toWorld, NULL, sizeof(XMFLOAT4X4) * 64);
		memset(local, NULL, sizeof(XMFLOAT4X4) * 64);
		memset(parentIdx, NULL, sizeof(int) * 64);
		nBone = 0;
	}
};

class AnimationManager {
public:
	void Initialize();

	void AddAnimClip(const char* fileName, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	bool IsAleadyExist(const char* name) { return m_uomAnimClip.count(name); }
	AnimClip* GetAnimClip(const char* name) { return m_uomAnimClip[name]; }
	BoneHierarchy GetBoneHierarchyFromAnimClip(const char* clipName);
private:
	unordered_map<string, AnimClip*> m_uomAnimClip;
};

// Object한테 vecAnimation을 받아서 toWorld와 animTransform을 채워주는 역할
// Object한테 vecAnimation을 받아서 toWorld와 animTransform에 Blend 해주는 역할
namespace AnimationCalculate {
	void GetFrameIdxAndNormalizedTime(AnimClip* clip, const float fTime, float& OutfNormalizedTime, XMINT4& OutIdx);
	XMMATRIX GetLocalTransform(AnimClip* clip, const int boneIdx, const float fNormalizedTime, const XMINT4 frameIdx);
	void AnimateLocalTransform(AnimatedObject* pObj, const float fTime, ClipPair vecClipPair, BoneMask* pMask = NULL);

	void InterpolateKeyframe(Keyframe k0, Keyframe k1, Keyframe k2, Keyframe k3, float t, Keyframe& out);
	XMFLOAT3 Interpolate(const XMFLOAT3 v0, const XMFLOAT3 v1, const XMFLOAT3 v2, const XMFLOAT3 v3, float t);

	void MakeToWorldTransform(BoneHierarchy& bh);
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
	void SetAnimationTransform(ID3D12GraphicsCommandList* pd3dCommandList, BoneHierarchy bh);

protected:
	ID3D12Resource*				m_pd3dCBResource = NULL;
	CB_BONE_INFO*				m_pCBMappedBones = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorHandle;
};
