#include "stdafx.h"
#include "Animation.h"
#include "Importer.h"

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

AnimationController::AnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& cbvCpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& cbvGpuHandle)
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

	m_pToWorldTransform = new XMFLOAT4X4[64];
	m_pAnimationTransform = new XMFLOAT4X4[64];
}

float maxEpsilon(float f) {
	if (abs(f) <= 0.001f) return 0;
	return f;
}
void PrintMtx(const XMMATRIX& m) {
	XMFLOAT4 rotation, translation;
	XMFLOAT4X4 xmf4x4result;
	float yaw, pitch, roll;

	XMStoreFloat4(&rotation, XMQuaternionRotationMatrix(m));
	XMStoreFloat4x4(&xmf4x4result, m);
	quat_2_euler_d3d(rotation, yaw, pitch, roll);
	translation.x = xmf4x4result._41;
	translation.y = xmf4x4result._42;
	translation.z = xmf4x4result._43;

	cout
		<< "- Rq: "
		<< maxEpsilon(rotation.x) << ", "
		<< maxEpsilon(rotation.y) << ", "
		<< maxEpsilon(rotation.z) << ", "
		<< maxEpsilon(rotation.w) << "\n"
		<< "- Rv: "
		<< maxEpsilon(XMConvertToDegrees(yaw)) << ", "
		<< maxEpsilon(XMConvertToDegrees(pitch)) << ", "
		<< maxEpsilon(XMConvertToDegrees(roll)) << "\n"
		<< "- Tv: "
		<< maxEpsilon(translation.x) << ", "
		<< maxEpsilon(translation.y) << ", "
		<< maxEpsilon(translation.z) << "\n";

}

#define ANIMBLEND

void AnimationController::SetMatrix(
	ID3D12GraphicsCommandList* pd3dCommandList, 
	const vector<pair<string, float>> vecPair,
	const float dTime)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURE_ANIMTRANSFORM, m_d3dCbvGPUDescriptorHandle);
	UINT ncbElementBytes = ((sizeof(CB_BONE_INFO) + 255) & ~255);
	memset(m_pCBMappedBones, NULL, ncbElementBytes);


#ifdef ANIMBLEND
	vector<AnimClip*> vecClipList;	
	double normalizedTime;
	double time = dTime;
	float fAverageClipLength = 0;
	XMMATRIX mtxFront, mtxBack;
	vector<XMMATRIX> mtxBones;
	Keyframe result;
	int i0, i1, i2, i3;

	for (int i = 0; i < vecPair.size(); i++) {
		vecClipList.push_back(gAnimMng.GetAnimClip(vecPair[i].first.c_str()));
		fAverageClipLength += vecClipList[i]->fClipLength * vecPair[i].second;
	}

	mtxBones.resize(64);

	while (time > fAverageClipLength) time -= fAverageClipLength;

	for (int iClip = 0; iClip < vecClipList.size(); iClip++) {

		float fCurrClipNormalizedFactor = fAverageClipLength / vecClipList[iClip]->fClipLength;

		for (int timeIdx = 0; timeIdx < vecClipList[iClip]->vecTimes.size(); timeIdx++) {
			// 특정 Key와 시간이 같다면(맨 앞이거나, 맨 뒤거나, 중간에 어떤 Key에 걸렸거나)

			if (time == vecClipList[iClip]->vecTimes[timeIdx] * fCurrClipNormalizedFactor) {

				for (int i = 0; i < vecClipList[iClip]->vecBone.size(); i++) {

					mtxFront = XMLoadFloat4x4(&vecClipList[iClip]->vecBone[i].toDressposeInv);
					mtxBack = XMMatrixMultiply(
						XMMatrixRotationQuaternion(XMLoadFloat4(&vecClipList[iClip]->vecBone[i].keys[timeIdx].xmf4QuatRotation)),
						XMMatrixTranslationFromVector(XMLoadFloat3(&vecClipList[iClip]->vecBone[i].keys[timeIdx].xmf3Translation))
					);
					mtxBones[i] += XMMatrixMultiply(mtxFront, mtxBack) * vecPair[iClip].second;
				}
				break;
			}
			if (vecClipList[iClip]->vecTimes[timeIdx] * fCurrClipNormalizedFactor < time && time <= vecClipList[iClip]->vecTimes[timeIdx + 1] * fCurrClipNormalizedFactor) {
				i1 = timeIdx; i2 = timeIdx + 1;

				if (timeIdx != 0)									i0 = i1 - 1;
				else												i0 = 0;
				if (i2 != vecClipList[iClip]->vecTimes.size() - 1)	i3 = i2 + 1;
				else												i3 = i2;
				// KeySelect End

				normalizedTime = (time - vecClipList[iClip]->vecTimes[i1] * fCurrClipNormalizedFactor) 
					/ (vecClipList[iClip]->vecTimes[i2] * fCurrClipNormalizedFactor - vecClipList[iClip]->vecTimes[i1] * fCurrClipNormalizedFactor);

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
					mtxBones[boneIdx] += XMMatrixMultiply(mtxFront, mtxBack) * vecPair[iClip].second;
				}
				break;
			}
		}
	}
	for (int boneIdx = 0; boneIdx < vecClipList[0]->vecBone.size(); boneIdx++) {
		XMStoreFloat4x4(&m_pCBMappedBones->xmf4x4Transform[boneIdx], XMMatrixTranspose(mtxBones[boneIdx]));
	}
#else
	AnimClip* animClip = gAnimMng.GetAnimClip(vecPair.front().first.c_str());
	XMMATRIX mtxFront, mtxBack;
	Keyframe result;
	int i0, i1, i2, i3;
	double normalizedTime;
	double time = dTime;
	while (time > animClip->fClipLength) time -= animClip->fClipLength;



	for (int timeIdx = 0; timeIdx < animClip->vecTimes.size(); timeIdx++) {
		if (time == animClip->vecTimes[timeIdx]) {

			for (int i = 0; i < animClip->vecBone.size(); i++) {

				mtxFront = XMLoadFloat4x4(&animClip->vecBone[i].toDressposeInv);
				mtxBack = XMMatrixMultiply(
					XMMatrixRotationQuaternion(XMLoadFloat4(&animClip->vecBone[i].keys[timeIdx].xmf4QuatRotation)),
					XMMatrixTranslationFromVector(XMLoadFloat3(&animClip->vecBone[i].keys[timeIdx].xmf3Translation))
				);
				XMStoreFloat4x4(&m_pCBMappedBones->xmf4x4Transform[i], XMMatrixTranspose(XMMatrixMultiply(mtxFront, mtxBack)));
			}
			break;
		}
		if (animClip->vecTimes[timeIdx] < time && time <= animClip->vecTimes[timeIdx + 1]) {
			i1 = timeIdx; i2 = timeIdx + 1;

			if (timeIdx != 0)							i0 = i1 - 1;
			else										i0 = 0;
			if (i2 != animClip->vecTimes.size() - 1)	i3 = i2 + 1;
			else										i3 = i2;
			// KeySelect End

			normalizedTime = (time - animClip->vecTimes[i1]) / (animClip->vecTimes[i2] - animClip->vecTimes[i1]);

			for (int boneIdx = 0; boneIdx < animClip->vecBone.size(); boneIdx++) {

				mtxFront = XMLoadFloat4x4(&animClip->vecBone[boneIdx].toDressposeInv);
				InterpolateKeyframe(
					animClip->vecBone[boneIdx].keys[i0], 
					animClip->vecBone[boneIdx].keys[i1],
					animClip->vecBone[boneIdx].keys[i2], 
					animClip->vecBone[boneIdx].keys[i3], 
					normalizedTime,
					result);
				
				mtxBack = XMMatrixMultiply(
					XMMatrixRotationQuaternion(XMLoadFloat4(&result.xmf4QuatRotation)),
					XMMatrixTranslationFromVector(XMLoadFloat3(&result.xmf3Translation))
				);
				XMStoreFloat4x4(&m_pCBMappedBones->xmf4x4Transform[boneIdx], XMMatrixTranspose(XMMatrixMultiply(mtxFront, mtxBack)));
			}
			break;
		}
	}
#endif
}

XMMATRIX AnimationController::GetBoneMatrix(
	const vector<pair<string, float>> vecPair,
	int boneIdx,
	const float fTime)
{
#ifdef ANIMBLEND
	vector<AnimClip*> vecClipList;
	double normalizedTime;
	double time = fTime;
	float fAverageClipLength = 0;
	XMMATRIX mtxBack;
	XMMATRIX mtxBones = XMMatrixIdentity();
	Keyframe result;
	int i0, i1, i2, i3;

	for (int i = 0; i < vecPair.size(); i++) {
		vecClipList.push_back(gAnimMng.GetAnimClip(vecPair[i].first.c_str()));
		fAverageClipLength += vecClipList[i]->fClipLength * vecPair[i].second;
	}
	while (time > fAverageClipLength) time -= fAverageClipLength;

	for (int iClip = 0; iClip < vecClipList.size(); iClip++) {

		float fCurrClipNormalizedFactor = fAverageClipLength / vecClipList[iClip]->fClipLength;

		for (int timeIdx = 0; timeIdx < vecClipList[iClip]->vecTimes.size(); timeIdx++) {
			// 특정 Key와 시간이 같다면(맨 앞이거나, 맨 뒤거나, 중간에 어떤 Key에 걸렸거나)

			if (time == vecClipList[iClip]->vecTimes[timeIdx] * fCurrClipNormalizedFactor) {
				mtxBack = XMMatrixMultiply(
					XMMatrixRotationQuaternion(XMLoadFloat4(&vecClipList[iClip]->vecBone[boneIdx].keys[timeIdx].xmf4QuatRotation)),
					XMMatrixTranslationFromVector(XMLoadFloat3(&vecClipList[iClip]->vecBone[boneIdx].keys[timeIdx].xmf3Translation))
				);
				mtxBones += mtxBack * vecPair[iClip].second;
				break;
			}
			if (vecClipList[iClip]->vecTimes[timeIdx] * fCurrClipNormalizedFactor < time && time <= vecClipList[iClip]->vecTimes[timeIdx + 1] * fCurrClipNormalizedFactor) {
				i1 = timeIdx; i2 = timeIdx + 1;

				if (timeIdx != 0)									i0 = i1 - 1;
				else												i0 = 0;
				if (i2 != vecClipList[iClip]->vecTimes.size() - 1)	i3 = i2 + 1;
				else												i3 = i2;
				// KeySelect End

				normalizedTime = (time - vecClipList[iClip]->vecTimes[i1] * fCurrClipNormalizedFactor)
					/ (vecClipList[iClip]->vecTimes[i2] * fCurrClipNormalizedFactor - vecClipList[iClip]->vecTimes[i1] * fCurrClipNormalizedFactor);
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
				mtxBones += mtxBack * vecPair[iClip].second;
				break;
			}
		}
	}
	return mtxBones;
#else
	AnimClip* animClip = gAnimMng.GetAnimClip(vecPair.front().first.c_str());
	XMMATRIX mtxBack;
	Keyframe result;
	int i0, i1, i2, i3;
	float normalizedTime;
	float time = fTime;
	while (time > animClip->fClipLength) time -= animClip->fClipLength;

	for (int timeIdx = 0; timeIdx < animClip->vecTimes.size(); timeIdx++) {
		// 특정 Key와 시간이 같다면(맨 앞이거나, 맨 뒤거나, 중간에 어떤 Key에 걸렸거나)
		if (time == animClip->vecTimes[timeIdx]) {
			mtxBack = XMMatrixMultiply(
				XMMatrixRotationQuaternion(XMLoadFloat4(&animClip->vecBone[boneIdx].keys[timeIdx].xmf4QuatRotation)),
				XMMatrixTranslationFromVector(XMLoadFloat3(&animClip->vecBone[boneIdx].keys[timeIdx].xmf3Translation))
			);

			return mtxBack;
			break;
		}
		if (animClip->vecTimes[timeIdx] < time && time <= animClip->vecTimes[timeIdx + 1]) {
			i1 = timeIdx; i2 = timeIdx + 1;

			if (timeIdx != 0)							i0 = i1 - 1;
			else										i0 = 0;
			if (i2 != animClip->vecTimes.size() - 1)	i3 = i2 + 1;
			else										i3 = i2;
			// KeySelect End

			normalizedTime = (time - animClip->vecTimes[i1]) / (animClip->vecTimes[i2] - animClip->vecTimes[i1]);

			InterpolateKeyframe(
				animClip->vecBone[boneIdx].keys[i0],
				animClip->vecBone[boneIdx].keys[i1],
				animClip->vecBone[boneIdx].keys[i2],
				animClip->vecBone[boneIdx].keys[i3],
				normalizedTime,
				result);
			mtxBack = XMMatrixMultiply(
				XMMatrixRotationQuaternion(XMLoadFloat4(&result.xmf4QuatRotation)),
				XMMatrixTranslationFromVector(XMLoadFloat3(&result.xmf3Translation))
			);
			return mtxBack;

			break;
		}
	}
#endif

}

void AnimationController::MakeAnimationTransform(const vector<pair<string, float>> vecPairClipNWeight, const float fTime)
{
#ifdef ANIMBLEND
	vector<AnimClip*> vecClipList;
	float normalizedTime;
	//float time = fTime;
	//float fAverageClipLength = 0;
	XMMATRIX mtxFront, mtxBack;
	Keyframe result;
	int i0, i1, i2, i3;

	memset(m_pToWorldTransform, NULL, sizeof(XMFLOAT4X4) * 64);
	memset(m_pAnimationTransform, NULL, sizeof(XMFLOAT4X4) * 64);


	for (int i = 0; i < vecPairClipNWeight.size(); i++) {
		vecClipList.push_back(gAnimMng.GetAnimClip(vecPairClipNWeight[i].first.c_str()));
		//fAverageClipLength += vecClipList[i]->fClipLength * vecPairClipNWeight[i].second;
	}

	//while (time > fAverageClipLength) time -= fAverageClipLength;

	for (int iClip = 0; iClip < vecClipList.size(); iClip++) {
		float time = fTime;
		while (time > vecClipList[iClip]->fClipLength) time -= vecClipList[iClip]->fClipLength;

		//float fCurrClipNormalizedFactor = fAverageClipLength / vecClipList[iClip]->fClipLength;

		for (int timeIdx = 0; timeIdx < vecClipList[iClip]->vecTimes.size(); timeIdx++) {
			// 특정 Key와 시간이 같다면(맨 앞이거나, 맨 뒤거나, 중간에 어떤 Key에 걸렸거나)

			if (time == vecClipList[iClip]->vecTimes[timeIdx]/* * fCurrClipNormalizedFactor*/) {

				for (int i = 0; i < vecClipList[iClip]->vecBone.size(); i++) {

					mtxFront = XMLoadFloat4x4(&vecClipList[iClip]->vecBone[i].toDressposeInv);
					mtxBack = XMMatrixMultiply(
						XMMatrixRotationQuaternion(XMLoadFloat4(&vecClipList[iClip]->vecBone[i].keys[timeIdx].xmf4QuatRotation)),
						XMMatrixTranslationFromVector(XMLoadFloat3(&vecClipList[iClip]->vecBone[i].keys[timeIdx].xmf3Translation))
					);
					XMStoreFloat4x4(&m_pAnimationTransform[i], XMLoadFloat4x4(&m_pAnimationTransform[i]) + (XMMatrixMultiply(mtxFront, mtxBack) * vecPairClipNWeight[iClip].second));
					XMStoreFloat4x4(&m_pToWorldTransform[i], XMLoadFloat4x4(&m_pToWorldTransform[i]) + (mtxBack * vecPairClipNWeight[iClip].second));
				}
				break;
			}
			if (vecClipList[iClip]->vecTimes[timeIdx]/* * fCurrClipNormalizedFactor*/ < time && time <= vecClipList[iClip]->vecTimes[timeIdx + 1]/* * fCurrClipNormalizedFactor*/) {
				i1 = timeIdx; i2 = timeIdx + 1;

				if (timeIdx != 0)									i0 = i1 - 1;
				else												i0 = 0;
				if (i2 != vecClipList[iClip]->vecTimes.size() - 1)	i3 = i2 + 1;
				else												i3 = i2;
				// KeySelect End

				normalizedTime = (time - vecClipList[iClip]->vecTimes[i1]/* * fCurrClipNormalizedFactor*/)
					/ (vecClipList[iClip]->vecTimes[i2]/* * fCurrClipNormalizedFactor*/ - vecClipList[iClip]->vecTimes[i1]/* * fCurrClipNormalizedFactor*/);

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
					XMStoreFloat4x4(&m_pAnimationTransform[boneIdx], XMLoadFloat4x4(&m_pAnimationTransform[boneIdx]) + (XMMatrixMultiply(mtxFront, mtxBack) * vecPairClipNWeight[iClip].second));
					XMStoreFloat4x4(&m_pToWorldTransform[boneIdx], XMLoadFloat4x4(&m_pToWorldTransform[boneIdx]) + (mtxBack * vecPairClipNWeight[iClip].second));


				}
				break;
			}
		}
	}

#else
	AnimClip* animClip = gAnimMng.GetAnimClip(vecPairClipNWeight.front().first.c_str());
	XMMATRIX mtxFront, mtxBack;
	Keyframe result;
	int i0, i1, i2, i3;
	double normalizedTime;
	double time = dTime;
	while (time > animClip->fClipLength) time -= animClip->fClipLength;



	for (int timeIdx = 0; timeIdx < animClip->vecTimes.size(); timeIdx++) {
		if (time == animClip->vecTimes[timeIdx]) {

			for (int i = 0; i < animClip->vecBone.size(); i++) {

				mtxFront = XMLoadFloat4x4(&animClip->vecBone[i].toDressposeInv);
				mtxBack = XMMatrixMultiply(
					XMMatrixRotationQuaternion(XMLoadFloat4(&animClip->vecBone[i].keys[timeIdx].xmf4QuatRotation)),
					XMMatrixTranslationFromVector(XMLoadFloat3(&animClip->vecBone[i].keys[timeIdx].xmf3Translation))
				);
				XMStoreFloat4x4(&m_pCBMappedBones->xmf4x4Transform[i], XMMatrixTranspose(XMMatrixMultiply(mtxFront, mtxBack)));
				XMStoreFloat4x4(&m_vecToWorldTransform[i], mtxBack);

			}
			break;
		}
		if (animClip->vecTimes[timeIdx] < time && time <= animClip->vecTimes[timeIdx + 1]) {
			i1 = timeIdx; i2 = timeIdx + 1;

			if (timeIdx != 0)							i0 = i1 - 1;
			else										i0 = 0;
			if (i2 != animClip->vecTimes.size() - 1)	i3 = i2 + 1;
			else										i3 = i2;
			// KeySelect End

			normalizedTime = (time - animClip->vecTimes[i1]) / (animClip->vecTimes[i2] - animClip->vecTimes[i1]);

			for (int boneIdx = 0; boneIdx < animClip->vecBone.size(); boneIdx++) {

				mtxFront = XMLoadFloat4x4(&animClip->vecBone[boneIdx].toDressposeInv);
				InterpolateKeyframe(
					animClip->vecBone[boneIdx].keys[i0],
					animClip->vecBone[boneIdx].keys[i1],
					animClip->vecBone[boneIdx].keys[i2],
					animClip->vecBone[boneIdx].keys[i3],
					normalizedTime,
					result);

				mtxBack = XMMatrixMultiply(
					XMMatrixRotationQuaternion(XMLoadFloat4(&result.xmf4QuatRotation)),
					XMMatrixTranslationFromVector(XMLoadFloat3(&result.xmf3Translation))
				);
				XMStoreFloat4x4(&m_pCBMappedBones->xmf4x4Transform[boneIdx], XMMatrixTranspose(XMMatrixMultiply(mtxFront, mtxBack)));
				XMStoreFloat4x4(&m_vecToWorldTransform[boneIdx], mtxBack);
			}
			break;
		}
	}
#endif
}

XMMATRIX AnimationController::GetLatestToWorldTransformOfSpecificBone(const int boneIdx)
{
	return XMLoadFloat4x4( &m_pToWorldTransform[boneIdx] );
}

void AnimationController::SetAnimationTransform(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURE_ANIMTRANSFORM, m_d3dCbvGPUDescriptorHandle);
	UINT ncbElementBytes = ((sizeof(CB_BONE_INFO) + 255) & ~255);
	memset(m_pCBMappedBones, NULL, ncbElementBytes);

	for(int i = 0 ; i < 64; i++)
		XMStoreFloat4x4(&m_pCBMappedBones->xmf4x4Transform[i], XMMatrixTranspose( XMLoadFloat4x4(&m_pAnimationTransform[i]) ));
}

void AnimationController::InterpolateKeyframe(Keyframe k0, Keyframe k1, Keyframe k2, Keyframe k3, float t, Keyframe& out)
{
	XMStoreFloat4(&out.xmf4QuatRotation, 
	XMQuaternionSlerp(XMLoadFloat4(&k1.xmf4QuatRotation), XMLoadFloat4(&k2.xmf4QuatRotation), t));
	//out.xmf4QuatRotation = Interpolate(k0.xmf4QuatRotation, k1.xmf4QuatRotation, k2.xmf4QuatRotation, k3.xmf4QuatRotation, t);
	out.xmf3Translation = Interpolate(k0.xmf3Translation, k1.xmf3Translation, k2.xmf3Translation, k3.xmf3Translation, t);
}

XMFLOAT4 AnimationController::Interpolate(const XMFLOAT4 q0, const XMFLOAT4 q1, const XMFLOAT4 q2, const XMFLOAT4 q3, float t)
{
	XMVECTOR resultV = XMVectorCatmullRom(XMLoadFloat4(&q0), XMLoadFloat4(&q1), XMLoadFloat4(&q2), XMLoadFloat4(&q3), t);
	XMFLOAT4 result;

	XMStoreFloat4(&result, resultV);

	return result;
}

XMFLOAT3 AnimationController::Interpolate(const XMFLOAT3 q0, const XMFLOAT3 q1, const XMFLOAT3 q2, const XMFLOAT3 q3, float t)
{
	XMVECTOR resultV = XMVectorCatmullRom(XMLoadFloat3(&q0), XMLoadFloat3(&q1), XMLoadFloat3(&q2), XMLoadFloat3(&q3), t);
	XMFLOAT3 result;

	XMStoreFloat3(&result, resultV);
	return result;
}