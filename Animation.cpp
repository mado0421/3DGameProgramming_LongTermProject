#include "stdafx.h"
#include "Animation.h"
#include "Importer.h"
#include "Object.h"
#include "Mask.h"
#include "State.h"

//==================================================================
// Animation Manager
//==================================================================
void AnimationManager::Initialize()
{
	m_uomAnimClip.clear();
}
void AnimationManager::AddAnimClip(const char* fileName, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	AnimClipDataImporter importer;
	AnimClip* animClip = new AnimClip();
	*animClip = importer.Load(fileName);

	m_uomAnimClip[fileName] = animClip;
}
BoneHierarchy AnimationManager::GetBoneHierarchyFromAnimClip(const char* clipName)
{
	BoneHierarchy bh;
	AnimClip* clip = GetAnimClip(clipName);
	bh.nBone = clip->vecBone.size();

	for (int i = 0; i < clip->vecBone.size(); i++) {
		bh.toDressposeInv[i]	= clip->vecBone[i].toDressposeInv;
		bh.toParent[i]			= clip->vecBone[i].toParent;
		bh.parentIdx[i]			= clip->vecBone[i].parentIdx;
	}

	return bh;
}

//==================================================================
// Animation Uploader
//==================================================================
AnimationUploader::AnimationUploader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& cbvCpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& cbvGpuHandle)
{
	UINT ncbElementBytes = ((sizeof(CB_BONE_INFO) + 255) & ~255);

	m_pd3dCBResource = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	D3D12_GPU_VIRTUAL_ADDRESS		d3dGpuVirtualAddress;
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;

	if (nullptr != m_pd3dCBResource) {
		m_pd3dCBResource->Map(0, NULL, (void**)&m_pCBMappedBones);
		d3dGpuVirtualAddress = m_pd3dCBResource->GetGPUVirtualAddress();
		d3dCBVDesc.SizeInBytes = ncbElementBytes;
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress;
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, cbvCpuHandle);

		cbvCpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;
	}
	m_d3dCbvGPUDescriptorHandle = cbvGpuHandle;
	cbvGpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;
}
void AnimationUploader::SetAnimationTransform(ID3D12GraphicsCommandList* pd3dCommandList, BoneHierarchy bh)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURE_ANIMTRANSFORM, m_d3dCbvGPUDescriptorHandle);
	UINT ncbElementBytes = ((sizeof(CB_BONE_INFO) + 255) & ~255);
	memset(m_pCBMappedBones, NULL, ncbElementBytes);

	for (int i = 0; i < 64; i++)
		XMStoreFloat4x4(&m_pCBMappedBones->xmf4x4Transform[i],
			XMMatrixTranspose(XMMatrixMultiply(XMLoadFloat4x4(&bh.toDressposeInv[i]), XMLoadFloat4x4(&bh.toWorld[i]))));
}

//==================================================================
// Animation Calculator
//==================================================================
void AnimationCalculate::GetFrameIdxAndNormalizedTime(AnimClip* clip, const float fTime, float& OutfNormalizedTime, XMINT4& OutIdx)
{
	float time = fTime;
	while (time > clip->fClipLength) time -= clip->fClipLength;

	for (int timeIdx = 0; timeIdx < clip->vecTimes.size(); timeIdx++) {
		if (time == clip->vecTimes[timeIdx]) {

			OutfNormalizedTime = 0;
			OutIdx.x = OutIdx.y = OutIdx.z = OutIdx.w = timeIdx;
			return;
		}
		if (clip->vecTimes[timeIdx] < time && time <= clip->vecTimes[timeIdx + 1]) {
			OutIdx.y = timeIdx; OutIdx.z = timeIdx + 1;

			if (timeIdx != 0)							OutIdx.x = OutIdx.y - 1;
			else										OutIdx.x = 0;
			if (OutIdx.z != clip->vecTimes.size() - 1)	OutIdx.w = OutIdx.z + 1;
			else										OutIdx.w = OutIdx.z;
			// KeySelect End

			OutfNormalizedTime = (time - clip->vecTimes[OutIdx.y]) / (clip->vecTimes[OutIdx.z] - clip->vecTimes[OutIdx.y]);
			return;
		}
	}
}
XMMATRIX AnimationCalculate::GetLocalTransform(AnimClip* clip, const int boneIdx, const float fNormalizedTime, const XMINT4 frameIdx)
{
	if (0 == fNormalizedTime) 
		return XMMatrixMultiply(
			XMMatrixRotationQuaternion(XMLoadFloat4(&clip->vecBone[boneIdx].keys[frameIdx.x].xmf4QuatRotation)),
			XMMatrixTranslationFromVector(XMLoadFloat3(&clip->vecBone[boneIdx].keys[frameIdx.x].xmf3Translation)));
	
	Keyframe temp;

	AnimationCalculate::InterpolateKeyframe(
		clip->vecBone[boneIdx].keys[frameIdx.x],
		clip->vecBone[boneIdx].keys[frameIdx.y],
		clip->vecBone[boneIdx].keys[frameIdx.z],
		clip->vecBone[boneIdx].keys[frameIdx.w],
		fNormalizedTime,
		temp);

	return XMMatrixMultiply(
		XMMatrixRotationQuaternion(XMLoadFloat4(&temp.xmf4QuatRotation)),
		XMMatrixTranslationFromVector(XMLoadFloat3(&temp.xmf3Translation)));
}
void AnimationCalculate::AnimateLocalTransform(AnimatedObject* pObj, const float fTime, ClipPair vecClipPair, BoneMask* pMask)
{
	float weight[64];
	memset(weight, 0, sizeof(float) * 64);

	/*
	* ClipPair의 Weight는 무조건 총합 1을 가정
	* 
	* local[i] *= (1 - pMask[i]);
	* 
	* local[i] += clip.Bone[i].local * clip.weight * pMask[i];
	* 
	* 
	* 
	*/
	pObj->m_boneHierarchyInfo.local;

	XMMATRIX local[64];

	// XMMATRIX init
	for (int i = 0; i < 64; i++) 
		local[i] = XMLoadFloat4x4(&pObj->m_boneHierarchyInfo.local[i]);

	// 1. Mask
	if (pMask) {
		for (int i = 0; i < 64; i++)
			local[i] *= (1 - pMask->weight[i]);

		for (int clipIdx = 0; clipIdx < vecClipPair.size(); clipIdx++) {
			AnimClip* clip = g_AnimMng.GetAnimClip(vecClipPair[clipIdx].first.c_str());
			XMINT4 xmi4FrameIdx;
			float fNormalizedTime;

			GetFrameIdxAndNormalizedTime(clip, fTime, fNormalizedTime, xmi4FrameIdx);
			for (int i = 0; i < clip->vecBone.size(); i++) {
				local[i] += GetLocalTransform(clip, i, fNormalizedTime, xmi4FrameIdx) * (vecClipPair[clipIdx].second * pMask->weight[i]);
			}
		}
	}
	else {
		for (int clipIdx = 0; clipIdx < vecClipPair.size(); clipIdx++) {
			AnimClip* clip = g_AnimMng.GetAnimClip(vecClipPair[clipIdx].first.c_str());
			XMINT4 xmi4FrameIdx;
			float fNormalizedTime;

			GetFrameIdxAndNormalizedTime(clip, fTime, fNormalizedTime, xmi4FrameIdx);
			for (int i = 0; i < clip->vecBone.size(); i++) {
				local[i] += GetLocalTransform(clip, i, fNormalizedTime, xmi4FrameIdx) * vecClipPair[clipIdx].second;
			}
		}
	}

	for (int i = 0; i < 64; i++)
		XMStoreFloat4x4(&pObj->m_boneHierarchyInfo.local[i], local[i]);
}
void AnimationCalculate::InterpolateKeyframe(Keyframe k0, Keyframe k1, Keyframe k2, Keyframe k3, float t, Keyframe& out)
{
	XMStoreFloat4(&out.xmf4QuatRotation,
		XMQuaternionSlerp(XMLoadFloat4(&k1.xmf4QuatRotation), XMLoadFloat4(&k2.xmf4QuatRotation), t));
	out.xmf3Translation = Interpolate(k0.xmf3Translation, k1.xmf3Translation, k2.xmf3Translation, k3.xmf3Translation, t);
}
XMFLOAT3 AnimationCalculate::Interpolate(const XMFLOAT3 v0, const XMFLOAT3 v1, const XMFLOAT3 v2, const XMFLOAT3 v3, float t)
{
	XMVECTOR resultV = XMVectorCatmullRom(XMLoadFloat3(&v0), XMLoadFloat3(&v1), XMLoadFloat3(&v2), XMLoadFloat3(&v3), t);
	XMFLOAT3 result;

	XMStoreFloat3(&result, resultV);
	return result;
}
void AnimationCalculate::MakeToWorldTransform(BoneHierarchy& bh)
{
	memset(bh.toWorld, NULL, sizeof(XMFLOAT4X4) * 64);

	for (int i = 0; i < bh.nBone; i++) {
		if (-1 != bh.parentIdx[i]) {
			XMStoreFloat4x4(&bh.toWorld[i],
				XMMatrixMultiply( 
					XMMatrixMultiply(XMLoadFloat4x4(&bh.local[i]), XMLoadFloat4x4(&bh.toParent[i])),
					XMLoadFloat4x4(&bh.toWorld[bh.parentIdx[i]])
				)
			);
		}
		else {
			XMStoreFloat4x4(&bh.toWorld[i],
				XMMatrixMultiply(XMLoadFloat4x4(&bh.local[i]), XMLoadFloat4x4(&bh.toParent[i]))
			);
		}
	}
}
