#include "stdafx.h"
#include "Animation.h"
#include "Importer.h"
#include "Object.h"
#include "Mask.h"

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
//AnimationController::AnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& cbvCpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& cbvGpuHandle)
//{
//	UINT ncbElementBytes = ((sizeof(CB_BONE_INFO) + 255) & ~255);
//
//	m_pd3dCBResource = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
//		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
//
//	D3D12_GPU_VIRTUAL_ADDRESS		d3dGpuVirtualAddress;
//	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
//
//	if (nullptr != m_pd3dCBResource) {
//		m_pd3dCBResource->Map(0, NULL, (void**)&m_pCBMappedBones);
//		d3dGpuVirtualAddress = m_pd3dCBResource->GetGPUVirtualAddress();
//		d3dCBVDesc.SizeInBytes = ncbElementBytes;
//		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress;
//		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, cbvCpuHandle);
//
//		cbvCpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;
//	}
//	m_d3dCbvGPUDescriptorHandle = cbvGpuHandle;
//	cbvGpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;
//
//	m_pToWorldTransform = new XMFLOAT4X4[64];
//	m_pAnimationTransform = new XMFLOAT4X4[64];
//}
//
//#define ANIMBLEND
//
//void AnimationController::MakeAnimationTransform(const vector<pair<string, float>> vecPairClipNWeight, const float fTime)
//{
//#ifdef ANIMBLEND
//	vector<AnimClip*> vecClipList;
//	float normalizedTime;
//	XMMATRIX mtxFront, mtxBack;
//	Keyframe result;
//	int i0, i1, i2, i3;
//
//	memset(m_pToWorldTransform, NULL, sizeof(XMFLOAT4X4) * 64);
//	memset(m_pAnimationTransform, NULL, sizeof(XMFLOAT4X4) * 64);
//
//
//	for (int i = 0; i < vecPairClipNWeight.size(); i++) {
//		vecClipList.push_back(gAnimMng.GetAnimClip(vecPairClipNWeight[i].first.c_str()));
//	}
//
//	for (int iClip = 0; iClip < vecClipList.size(); iClip++) {
//		float time = fTime;
//		while (time > vecClipList[iClip]->fClipLength) time -= vecClipList[iClip]->fClipLength;
//
//		for (int timeIdx = 0; timeIdx < vecClipList[iClip]->vecTimes.size(); timeIdx++) {
//			if (time == vecClipList[iClip]->vecTimes[timeIdx]) {
//
//				for (int i = 0; i < vecClipList[iClip]->vecBone.size(); i++) {
//
//					mtxFront = XMLoadFloat4x4(&vecClipList[iClip]->vecBone[i].toDressposeInv);
//					mtxBack = XMMatrixMultiply(
//						XMMatrixRotationQuaternion(XMLoadFloat4(&vecClipList[iClip]->vecBone[i].keys[timeIdx].xmf4QuatRotation)),
//						XMMatrixTranslationFromVector(XMLoadFloat3(&vecClipList[iClip]->vecBone[i].keys[timeIdx].xmf3Translation))
//					);
//					XMStoreFloat4x4(&m_pAnimationTransform[i], XMLoadFloat4x4(&m_pAnimationTransform[i]) + (XMMatrixMultiply(mtxFront, mtxBack) * vecPairClipNWeight[iClip].second));
//					XMStoreFloat4x4(&m_pToWorldTransform[i], XMLoadFloat4x4(&m_pToWorldTransform[i]) + (mtxBack * vecPairClipNWeight[iClip].second));
//				}
//				break;
//			}
//			if (vecClipList[iClip]->vecTimes[timeIdx] < time && time <= vecClipList[iClip]->vecTimes[timeIdx + 1]) {
//				i1 = timeIdx; i2 = timeIdx + 1;
//
//				if (timeIdx != 0)									i0 = i1 - 1;
//				else												i0 = 0;
//				if (i2 != vecClipList[iClip]->vecTimes.size() - 1)	i3 = i2 + 1;
//				else												i3 = i2;
//				// KeySelect End
//
//				normalizedTime = (time - vecClipList[iClip]->vecTimes[i1])
//					/ (vecClipList[iClip]->vecTimes[i2] - vecClipList[iClip]->vecTimes[i1]);
//
//				for (int boneIdx = 0; boneIdx < vecClipList[iClip]->vecBone.size(); boneIdx++) {
//
//					mtxFront = XMLoadFloat4x4(&vecClipList[iClip]->vecBone[boneIdx].toDressposeInv);
//					InterpolateKeyframe(
//						vecClipList[iClip]->vecBone[boneIdx].keys[i0],
//						vecClipList[iClip]->vecBone[boneIdx].keys[i1],
//						vecClipList[iClip]->vecBone[boneIdx].keys[i2],
//						vecClipList[iClip]->vecBone[boneIdx].keys[i3],
//						normalizedTime,
//						result);
//
//					mtxBack = XMMatrixMultiply(
//						XMMatrixRotationQuaternion(XMLoadFloat4(&result.xmf4QuatRotation)),
//						XMMatrixTranslationFromVector(XMLoadFloat3(&result.xmf3Translation))
//					);
//					XMStoreFloat4x4(&m_pAnimationTransform[boneIdx], XMLoadFloat4x4(&m_pAnimationTransform[boneIdx]) + (XMMatrixMultiply(mtxFront, mtxBack) * vecPairClipNWeight[iClip].second));
//					XMStoreFloat4x4(&m_pToWorldTransform[boneIdx], XMLoadFloat4x4(&m_pToWorldTransform[boneIdx]) + (mtxBack * vecPairClipNWeight[iClip].second));
//
//
//				}
//				break;
//			}
//		}
//	}
//
//#else
//	AnimClip* animClip = gAnimMng.GetAnimClip(vecPairClipNWeight.front().first.c_str());
//	XMMATRIX mtxFront, mtxBack;
//	Keyframe result;
//	int i0, i1, i2, i3;
//	double normalizedTime;
//	double time = dTime;
//	while (time > animClip->fClipLength) time -= animClip->fClipLength;
//
//
//
//	for (int timeIdx = 0; timeIdx < animClip->vecTimes.size(); timeIdx++) {
//		if (time == animClip->vecTimes[timeIdx]) {
//
//			for (int i = 0; i < animClip->vecBone.size(); i++) {
//
//				mtxFront = XMLoadFloat4x4(&animClip->vecBone[i].toDressposeInv);
//				mtxBack = XMMatrixMultiply(
//					XMMatrixRotationQuaternion(XMLoadFloat4(&animClip->vecBone[i].keys[timeIdx].xmf4QuatRotation)),
//					XMMatrixTranslationFromVector(XMLoadFloat3(&animClip->vecBone[i].keys[timeIdx].xmf3Translation))
//				);
//				XMStoreFloat4x4(&m_pCBMappedBones->xmf4x4Transform[i], XMMatrixTranspose(XMMatrixMultiply(mtxFront, mtxBack)));
//				XMStoreFloat4x4(&m_vecToWorldTransform[i], mtxBack);
//
//			}
//			break;
//		}
//		if (animClip->vecTimes[timeIdx] < time && time <= animClip->vecTimes[timeIdx + 1]) {
//			i1 = timeIdx; i2 = timeIdx + 1;
//
//			if (timeIdx != 0)							i0 = i1 - 1;
//			else										i0 = 0;
//			if (i2 != animClip->vecTimes.size() - 1)	i3 = i2 + 1;
//			else										i3 = i2;
//			// KeySelect End
//
//			normalizedTime = (time - animClip->vecTimes[i1]) / (animClip->vecTimes[i2] - animClip->vecTimes[i1]);
//
//			for (int boneIdx = 0; boneIdx < animClip->vecBone.size(); boneIdx++) {
//
//				mtxFront = XMLoadFloat4x4(&animClip->vecBone[boneIdx].toDressposeInv);
//				InterpolateKeyframe(
//					animClip->vecBone[boneIdx].keys[i0],
//					animClip->vecBone[boneIdx].keys[i1],
//					animClip->vecBone[boneIdx].keys[i2],
//					animClip->vecBone[boneIdx].keys[i3],
//					normalizedTime,
//					result);
//
//				mtxBack = XMMatrixMultiply(
//					XMMatrixRotationQuaternion(XMLoadFloat4(&result.xmf4QuatRotation)),
//					XMMatrixTranslationFromVector(XMLoadFloat3(&result.xmf3Translation))
//				);
//				XMStoreFloat4x4(&m_pCBMappedBones->xmf4x4Transform[boneIdx], XMMatrixTranspose(XMMatrixMultiply(mtxFront, mtxBack)));
//				XMStoreFloat4x4(&m_vecToWorldTransform[boneIdx], mtxBack);
//			}
//			break;
//		}
//	}
//#endif
//}
//
//XMMATRIX AnimationController::GetLatestToWorldTransformOfSpecificBone(const int boneIdx)
//{
//	return XMLoadFloat4x4( &m_pToWorldTransform[boneIdx] );
//}
//
//void AnimationController::SetAnimationTransform(ID3D12GraphicsCommandList* pd3dCommandList)
//{
//	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURE_ANIMTRANSFORM, m_d3dCbvGPUDescriptorHandle);
//	UINT ncbElementBytes = ((sizeof(CB_BONE_INFO) + 255) & ~255);
//	memset(m_pCBMappedBones, NULL, ncbElementBytes);
//
//	for(int i = 0 ; i < 64; i++)
//		XMStoreFloat4x4(&m_pCBMappedBones->xmf4x4Transform[i], XMMatrixTranspose( XMLoadFloat4x4(&m_pAnimationTransform[i]) ));
//}
//
//void AnimationController::InterpolateKeyframe(Keyframe k0, Keyframe k1, Keyframe k2, Keyframe k3, float t, Keyframe& out)
//{
//	XMStoreFloat4(&out.xmf4QuatRotation, 
//	XMQuaternionSlerp(XMLoadFloat4(&k1.xmf4QuatRotation), XMLoadFloat4(&k2.xmf4QuatRotation), t));
//	out.xmf3Translation = Interpolate(k0.xmf3Translation, k1.xmf3Translation, k2.xmf3Translation, k3.xmf3Translation, t);
//}
//
//XMFLOAT4 AnimationController::Interpolate(const XMFLOAT4 q0, const XMFLOAT4 q1, const XMFLOAT4 q2, const XMFLOAT4 q3, float t)
//{
//	XMVECTOR resultV = XMVectorCatmullRom(XMLoadFloat4(&q0), XMLoadFloat4(&q1), XMLoadFloat4(&q2), XMLoadFloat4(&q3), t);
//	XMFLOAT4 result;
//
//	XMStoreFloat4(&result, resultV);
//
//	return result;
//}
//
//XMFLOAT3 AnimationController::Interpolate(const XMFLOAT3 q0, const XMFLOAT3 q1, const XMFLOAT3 q2, const XMFLOAT3 q3, float t)
//{
//	XMVECTOR resultV = XMVectorCatmullRom(XMLoadFloat3(&q0), XMLoadFloat3(&q1), XMLoadFloat3(&q2), XMLoadFloat3(&q3), t);
//	XMFLOAT3 result;
//
//	XMStoreFloat3(&result, resultV);
//	return result;
//}

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

void AnimationUploader::SetAnimationTransform(ID3D12GraphicsCommandList* pd3dCommandList, AnimatedObject* pTargetObj)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURE_ANIMTRANSFORM, m_d3dCbvGPUDescriptorHandle);
	UINT ncbElementBytes = ((sizeof(CB_BONE_INFO) + 255) & ~255);
	memset(m_pCBMappedBones, NULL, ncbElementBytes);

	for (int i = 0; i < 64; i++)
		XMStoreFloat4x4(&m_pCBMappedBones->xmf4x4Transform[i], XMMatrixTranspose(XMLoadFloat4x4(&pTargetObj->m_pAnimationTransform[i])));
}

void AnimationCalculate::MakeAnimationTransform(const vector<pair<string, float>> vecPairClipNWeight, const float fTime, AnimatedObject* pTargetObj)
{
	vector<AnimClip*> vecClipList;
	float normalizedTime;
	XMMATRIX mtxFront, mtxBack;
	Keyframe result;
	int i0, i1, i2, i3;

	XMFLOAT4X4* pToWorldTransform = new XMFLOAT4X4[64];
	XMFLOAT4X4* pAnimationTransform = new XMFLOAT4X4[64];

	memset(pToWorldTransform, NULL, sizeof(XMFLOAT4X4) * 64); 
	memset(pAnimationTransform, NULL, sizeof(XMFLOAT4X4) * 64);

	for (int i = 0; i < vecPairClipNWeight.size(); i++) {
		vecClipList.push_back(gAnimMng.GetAnimClip(vecPairClipNWeight[i].first.c_str()));
	}
	for (int iClip = 0; iClip < vecClipList.size(); iClip++) {
		float time = fTime;
		while (time > vecClipList[iClip]->fClipLength) time -= vecClipList[iClip]->fClipLength;

		for (int timeIdx = 0; timeIdx < vecClipList[iClip]->vecTimes.size(); timeIdx++) {
			if (time == vecClipList[iClip]->vecTimes[timeIdx]) {

				for (int i = 0; i < vecClipList[iClip]->vecBone.size(); i++) {

					mtxFront = XMLoadFloat4x4(&vecClipList[iClip]->vecBone[i].toDressposeInv);
					mtxBack = XMMatrixMultiply(
						XMMatrixRotationQuaternion(XMLoadFloat4(&vecClipList[iClip]->vecBone[i].keys[timeIdx].xmf4QuatRotation)),
						XMMatrixTranslationFromVector(XMLoadFloat3(&vecClipList[iClip]->vecBone[i].keys[timeIdx].xmf3Translation))
					);
					XMStoreFloat4x4(&pAnimationTransform[i], XMLoadFloat4x4(&pAnimationTransform[i]) + (XMMatrixMultiply(mtxFront, mtxBack) * vecPairClipNWeight[iClip].second));
					XMStoreFloat4x4(&pToWorldTransform[i], XMLoadFloat4x4(&pToWorldTransform[i]) + (mtxBack * vecPairClipNWeight[iClip].second));
				}
				break;
			}
			if (vecClipList[iClip]->vecTimes[timeIdx] < time && time <= vecClipList[iClip]->vecTimes[timeIdx + 1]) {
				i1 = timeIdx; i2 = timeIdx + 1;

				if (timeIdx != 0)									i0 = i1 - 1;
				else												i0 = 0;
				if (i2 != vecClipList[iClip]->vecTimes.size() - 1)	i3 = i2 + 1;
				else												i3 = i2;
				// KeySelect End

				normalizedTime = (time - vecClipList[iClip]->vecTimes[i1])
					/ (vecClipList[iClip]->vecTimes[i2] - vecClipList[iClip]->vecTimes[i1]);

				for (int boneIdx = 0; boneIdx < vecClipList[iClip]->vecBone.size(); boneIdx++) {

					mtxFront = XMLoadFloat4x4(&vecClipList[iClip]->vecBone[boneIdx].toDressposeInv);
					InterpolateKeyframe(
						vecClipList[iClip]->vecBone[boneIdx].keys[i0],
						vecClipList[iClip]->vecBone[boneIdx].keys[i1],
						vecClipList[iClip]->vecBone[boneIdx].keys[i2],
						vecClipList[iClip]->vecBone[boneIdx].keys[i3],
						normalizedTime,
						result);

					mtxBack = XMMatrixMultiply(
						XMMatrixRotationQuaternion(XMLoadFloat4(&result.xmf4QuatRotation)),
						XMMatrixTranslationFromVector(XMLoadFloat3(&result.xmf3Translation))
					);
					XMStoreFloat4x4(&pAnimationTransform[boneIdx], XMLoadFloat4x4(&pAnimationTransform[boneIdx]) + (XMMatrixMultiply(mtxFront, mtxBack) * vecPairClipNWeight[iClip].second));
					XMStoreFloat4x4(&pToWorldTransform[boneIdx], XMLoadFloat4x4(&pToWorldTransform[boneIdx]) + (mtxBack * vecPairClipNWeight[iClip].second));


				}
				break;
			}
		}
	}

	memcpy(pTargetObj->m_pToWorldTransform, pToWorldTransform, sizeof(XMFLOAT4X4) * 64);
	memcpy(pTargetObj->m_pAnimationTransform, pAnimationTransform, sizeof(XMFLOAT4X4) * 64);
	memset(pTargetObj->m_pBoneMask, NULL, sizeof(int) * 64);
}

void AnimationCalculate::BlendToAnimaionTransform(const vector<pair<string, float>> vecPairClipNWeight, const float fTime, AnimatedObject* pTargetObj, BoneMask* boneMask)
{
	/*
	지금 블렌드 하는 애니메이션 클립들은 전부 같은 종류로 볼 것.
	즉, Walk, Run / Crouch / Aiming 등으로 나눠서 할거란 의미.

	구현 전략:
		Clip과 Weight에서 Weight는 전체에 대한 가중치
		1이면 전부 덮어쓰겠다는 의미

		먼저 vecPairCilpWeight를 순회해서 Weight의 합을 구함.
		( 1 - saturate( totalWeight ) ) 가 기존 결과물의 Weight가 될 것.
		단, 여기서 Mask를 봐야 한다. Mask의 BoneIdx에 있는 값들만 Blend 한다. 이 때, stencil 값이 그보다 같거나 큰지도 확인해야 한다.
		해당하는 기존 결과물들은 XMLoadFloat4x4(&pAnimationTransform[i]) *= ( 1 - saturate( totalWeight ) )를 해주자.

		clip 시간도 하나로 맞춰야 함.
	*/
	// For Animation Calc
	XMMATRIX			xmmtxFront, xmmtxBack;
	float				fNormalizedTime;
	XMINT4				xmi4FrameIdx;

	float				totalWeight[64];
	memset(totalWeight, 0, sizeof(float) * 64);



	if (boneMask) {
		// 스텐실 값을 비교해서 유효한 totalWeight[boneIdx]에만 값을 넣어줌.
		for (int i = 0; i < 64; i++) {
			if (pTargetObj->m_pBoneMask[i] <= boneMask->affectedBoneInfo[i].stencil) {
				pTargetObj->m_pBoneMask[i] = boneMask->affectedBoneInfo[i].stencil;
				for (int j = 0; j < vecPairClipNWeight.size(); j++)
					totalWeight[i] += vecPairClipNWeight[j].second * boneMask->affectedBoneInfo[i].weight;
			}
			Clamp(totalWeight[i], 0, 1);
		}

		// 일괄적으로 변경.
		for (int i = 0; i < 64; i++) {
			XMStoreFloat4x4(&pTargetObj->m_pAnimationTransform[i], XMLoadFloat4x4(&pTargetObj->m_pAnimationTransform[i]) * (1 - totalWeight[i]));
			XMStoreFloat4x4(&pTargetObj->m_pToWorldTransform[i], XMLoadFloat4x4(&pTargetObj->m_pToWorldTransform[i]) * (1 - totalWeight[i]));
		}

		// 처리.
		for (int iClip = 0; iClip < vecPairClipNWeight.size(); iClip++) {

			GetFrameIdxAndNormalizedTime(vecPairClipNWeight[iClip].first.c_str(), fTime, fNormalizedTime, xmi4FrameIdx);

			for (int iBone = 0; iBone < gAnimMng.GetAnimClip(vecPairClipNWeight[iClip].first.c_str())->vecBone.size(); iBone++) {
				if (totalWeight[iBone]) {

					GetFrontAndBackMtx(vecPairClipNWeight[iClip].first.c_str(), fNormalizedTime, iBone, xmi4FrameIdx, xmmtxFront, xmmtxBack);
					XMStoreFloat4x4(&pTargetObj->m_pAnimationTransform[iBone], XMLoadFloat4x4(&pTargetObj->m_pAnimationTransform[iBone]) + (XMMatrixMultiply(xmmtxFront, xmmtxBack) * vecPairClipNWeight[iClip].second * boneMask->affectedBoneInfo[iBone].weight));
					XMStoreFloat4x4(&pTargetObj->m_pToWorldTransform[iBone], XMLoadFloat4x4(&pTargetObj->m_pToWorldTransform[iBone]) + (xmmtxBack * vecPairClipNWeight[iClip].second * boneMask->affectedBoneInfo[iBone].weight));
				}
			}
		}
	}
	else {
		// totalWeight[boneIdx] 값을 구함.
		for (int i = 0; i < 64; i++) {
			for (int j = 0; j < vecPairClipNWeight.size(); j++)
				totalWeight[i] += vecPairClipNWeight[j].second;
			Clamp(totalWeight[i], 0, 1);
		}

		// 일괄적으로 변경.
		for (int i = 0; i < 64; i++) {
			XMStoreFloat4x4(&pTargetObj->m_pAnimationTransform[i], XMLoadFloat4x4(&pTargetObj->m_pAnimationTransform[i]) * (1 - totalWeight[i]));
			XMStoreFloat4x4(&pTargetObj->m_pToWorldTransform[i], XMLoadFloat4x4(&pTargetObj->m_pToWorldTransform[i]) * (1 - totalWeight[i]));
		}

		// 처리.
		for (int iClip = 0; iClip < vecPairClipNWeight.size(); iClip++) {

			GetFrameIdxAndNormalizedTime(vecPairClipNWeight[iClip].first.c_str(), fTime, fNormalizedTime, xmi4FrameIdx);

			for (int iBone = 0; iBone < gAnimMng.GetAnimClip(vecPairClipNWeight[iClip].first.c_str())->vecBone.size(); iBone++) {

				GetFrontAndBackMtx(vecPairClipNWeight[iClip].first.c_str(), fNormalizedTime, iBone, xmi4FrameIdx, xmmtxFront, xmmtxBack);
				XMStoreFloat4x4(&pTargetObj->m_pAnimationTransform[iBone], XMLoadFloat4x4(&pTargetObj->m_pAnimationTransform[iBone]) + (XMMatrixMultiply(xmmtxFront, xmmtxBack) * vecPairClipNWeight[iClip].second));
				XMStoreFloat4x4(&pTargetObj->m_pToWorldTransform[iBone], XMLoadFloat4x4(&pTargetObj->m_pToWorldTransform[iBone]) + (xmmtxBack * vecPairClipNWeight[iClip].second));

			}
		}
	}
}

void AnimationCalculate::GetFrameIdxAndNormalizedTime(const char* strClipName, const float fTime, float& OutfNormalizedTime, XMINT4& OutIdx)
{
	AnimClip* clip = gAnimMng.GetAnimClip(strClipName);
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

void AnimationCalculate::GetFrontAndBackMtx(const char* strClipName, const float fNormalizedTime, const int boneIdx, const XMINT4 frameIdx, XMMATRIX& xmmtxFront, XMMATRIX& xmmtxBack)
{
	AnimClip* clip = gAnimMng.GetAnimClip(strClipName);
	Keyframe result;
	if (0 == fNormalizedTime) {
		xmmtxFront = XMLoadFloat4x4(&clip->vecBone[frameIdx.x].toDressposeInv);
		xmmtxBack = XMMatrixMultiply(
			XMMatrixRotationQuaternion(XMLoadFloat4(&clip->vecBone[boneIdx].keys[frameIdx.x].xmf4QuatRotation)),
			XMMatrixTranslationFromVector(XMLoadFloat3(&clip->vecBone[boneIdx].keys[frameIdx.x].xmf3Translation))
		);
		return;
	}
	else {
		xmmtxFront = XMLoadFloat4x4(&clip->vecBone[boneIdx].toDressposeInv);
		AnimationCalculate::InterpolateKeyframe(
			clip->vecBone[boneIdx].keys[frameIdx.x],
			clip->vecBone[boneIdx].keys[frameIdx.y],
			clip->vecBone[boneIdx].keys[frameIdx.z],
			clip->vecBone[boneIdx].keys[frameIdx.w],
			fNormalizedTime,
			result);

		xmmtxBack = XMMatrixMultiply(
			XMMatrixRotationQuaternion(XMLoadFloat4(&result.xmf4QuatRotation)),
			XMMatrixTranslationFromVector(XMLoadFloat3(&result.xmf3Translation))
		);
		return;
	}
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
