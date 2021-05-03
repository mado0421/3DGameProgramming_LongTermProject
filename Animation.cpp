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
	m_fTime = 0;

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

void AnimationController::Update(float fTimeElapsed)
{
	m_fTime += fTimeElapsed * 0.1f;
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

void AnimationController::SetMatrix(ID3D12GraphicsCommandList* pd3dCommandList)
{

	AnimClip* temp = gAnimMng.GetAnimClip("animTest_YUP");

	XMMATRIX mtxFront, mtxBack, result;


	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURE_ANIMTRANSFORM, m_d3dCbvGPUDescriptorHandle);
	UINT ncbElementBytes = ((sizeof(CB_BONE_INFO) + 255) & ~255);
	memset(m_pCBMappedBones, NULL, ncbElementBytes);

	float normalizedTime = m_fTime;
	while (normalizedTime > temp->fClipLength) {
		normalizedTime -= temp->fClipLength;
	}

	Sleep(10);
	system("cls");


	goto A;
	/*========================================================================
	* 각 Bone마다 InterpolationIndex를 찾아야 함
	========================================================================*/
	for (int iBone = 0; iBone < temp->vecBone.size(); iBone++) {
		mtxFront = XMLoadFloat4x4(&temp->vecBone[iBone].globalMtx);

		/*========================================================================
		* 만약, 키프레임이 하나밖에 없으면 그 값을 그대로 넣는다.
		========================================================================*/
		if (temp->vecBone[iBone].keys.size() == 1) {
			mtxBack = XMMatrixMultiply(
						XMMatrixTranslationFromVector(XMLoadFloat3(&temp->vecBone[iBone].keys[0].xmf3Translation)),
						XMMatrixRotationQuaternion(XMLoadFloat4(&temp->vecBone[iBone].keys[0].xmf4QuatRotation))
						);

		}
		/*========================================================================
		* 키프레임이 두 개 이상이면 index를 찾아야 한다.
		* 
		========================================================================*/
		else {
			Keyframe k0, k1, k2, k3, result;
			double t1, t2;
			
			for (int i = 0; i < temp->vecBone[iBone].keys.size(); i++) {
				if (normalizedTime <= temp->vecTimes[iBone]) {
					if (i == 0) {
						k0 = k1 = k2 = k3 = temp->vecBone[iBone].keys.front();
						t1 = t2 = temp->vecTimes[iBone];
					}
					else {
						k1 = temp->vecBone[iBone].keys[i - 1];
						k2 = temp->vecBone[iBone].keys[i];
						t1 = temp->vecTimes[iBone];
						t2 = temp->vecTimes[iBone + 1];


						if (i != 1) k0 = temp->vecBone[iBone].keys[i - 2];
						else		k0 = temp->vecBone[iBone].keys[i - 1];

						if (i != temp->vecBone[iBone].keys.size() - 1)	k3 = temp->vecBone[iBone].keys[i + 1];
						else											k3 = temp->vecBone[iBone].keys[i];
					}
					goto AnimKeySelectEnd;
				}
			}
			k0 = k1 = k2 = k3 = temp->vecBone[iBone].keys.back();
			t1 = t2 = temp->vecTimes[iBone];
			AnimKeySelectEnd:

			if (t1 == t2) {

				result = k2;
			}
			else {
				float timeBetweenkey12 = ( normalizedTime - t1) / (t2 - t1);

				InterpolateKeyframe(k0, k1, k2, k3, timeBetweenkey12, result);
			}

			mtxBack = XMMatrixMultiply(
				XMMatrixTranslationFromVector(XMLoadFloat3(&result.xmf3Translation)),
				XMMatrixRotationQuaternion(XMLoadFloat4(&result.xmf4QuatRotation))
			);			

		}
		result = XMMatrixMultiply(mtxFront, mtxBack);

		XMStoreFloat4x4(&m_pCBMappedBones->xmf4x4Transform[iBone], XMMatrixTranspose(result));
	}


A:;
	AnimClip* animClip = gAnimMng.GetAnimClip("animTest_YUP");
	XMVECTOR p0 = XMVectorSet(0.5, 3, 0.5, 1);

	for (int i = 0; i < animClip->vecBone.size(); i++) {
		//cout << "Bone_" << i << "\n";
		//cout << " - Global\n";
		mtxFront = XMLoadFloat4x4(&animClip->vecBone[i].globalMtx);
		//PrintMtx(mtxBack);

		mtxBack = XMMatrixMultiply(
			XMMatrixRotationQuaternion(XMLoadFloat4(&animClip->vecBone[i].keys[gTestInt].xmf4QuatRotation)),
			XMMatrixTranslationFromVector(XMLoadFloat3(&animClip->vecBone[i].keys[gTestInt].xmf3Translation))
		);
		//cout << " - elt\n";
		//PrintMtx(mtxFront);

		result = XMMatrixMultiply(mtxFront, mtxBack);
		XMVECTOR p = XMVector3Transform(p0, result);
		cout << "P :" << maxEpsilon(XMVectorGetX(p)) << ", " << maxEpsilon(XMVectorGetY(p)) << ", " << maxEpsilon(XMVectorGetZ(p)) << ", " << maxEpsilon(XMVectorGetW(p)) << "\n\n";

		//cout << " - result\n";
		//PrintMtx(result);
		//cout << "\n";

		//XMStoreFloat4x4(&m_pCBMappedBones->xmf4x4Transform[i], result);
		XMStoreFloat4x4(&m_pCBMappedBones->xmf4x4Transform[i],  XMMatrixTranspose(result));
	}


}

void AnimationController::InterpolateKeyframe(Keyframe k0, Keyframe k1, Keyframe k2, Keyframe k3, float t, Keyframe& out)
{
	out.xmf4QuatRotation = Interpolate(k0.xmf4QuatRotation, k1.xmf4QuatRotation, k2.xmf4QuatRotation, k3.xmf4QuatRotation, t);
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
