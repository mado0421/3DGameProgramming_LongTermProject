#include "stdafx.h"
#include "Animation.h"
#include "Importer.h"

void AnimationManager::Initialize()
{
	m_uomAnimClip.clear();
}

void AnimationManager::SettingCB(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle)
{
	//for_each(m_uomAnimClip.begin(), m_uomAnimClip.end(), [&](AnimClip c) 
	//	{c.PrepareCB(pd3dDevice, pd3dCommandList, d3dCbvCPUDescriptorStartHandle, d3dCbvGPUDescriptorStartHandle); });
	for (auto i = m_uomAnimClip.begin(); i != m_uomAnimClip.end(); i++) {
		i->second.PrepareCB(pd3dDevice, pd3dCommandList, d3dCbvCPUDescriptorStartHandle, d3dCbvGPUDescriptorStartHandle);
	}
}

void AnimationManager::AddAnimClip(const char* fileName, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	AnimClipDataImporter importer;
	AnimClip animClip = importer.Load(fileName);

	m_uomAnimClip[fileName] = animClip;
}

void AnimationManager::Render(const char* name, double objectTime, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (prevAnimClipName == name) return;
	prevAnimClipName = name;

	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURE_ANIMTRANSFORM, m_uomAnimClip[name].m_d3dCbvGPUDescriptorHandle);
	UINT ncbElementBytes = ((sizeof(CB_BONE_INFO) + 255) & ~255);
	memset(m_uomAnimClip[name].m_pCBMappedBones, NULL, ncbElementBytes);
	for (int i = 0; i < nMaxBone; i++) {

		//m_uomAnimClip[name].vecBone[i][0].xmf3Translation;
		//m_uomAnimClip[name].vecBone[i][0].xmf4QuatRotation;
		//XMMatrixRotationQuaternion(XMLoadFloat4(&m_uomAnimClip[name].vecBone[i][0].xmf4QuatRotation));
		//XMMatrixTranslationFromVector(XMLoadFloat3(&m_uomAnimClip[name].vecBone[i][0].xmf3Translation));
		//XMMatrixMultiply(XMMatrixRotationQuaternion(
		//	XMLoadFloat4(&m_uomAnimClip[name].vecBone[i][0].xmf4QuatRotation)),
		//	XMMatrixTranslationFromVector(XMLoadFloat3(&m_uomAnimClip[name].vecBone[i][0].xmf3Translation)));
		XMStoreFloat4x4(&m_uomAnimClip[name].m_pCBMappedBones->xmf4x4Transform[i], 
			XMMatrixTranspose(XMMatrixMultiply(XMMatrixRotationQuaternion(
			XMLoadFloat4(&m_uomAnimClip[name].vecBone[i][0].xmf4QuatRotation)),
			XMMatrixTranslationFromVector(XMLoadFloat3(&m_uomAnimClip[name].vecBone[i][0].xmf3Translation)))));
	}
}

void AnimClip::PrepareCB(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle)
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
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorStartHandle);

		d3dCbvCPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
	}
	m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorStartHandle;
	d3dCbvGPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
}
