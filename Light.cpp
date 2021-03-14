#include "stdafx.h"
#include "Light.h"

//void Light::UploadToGpu(ID3D12GraphicsCommandList* pd3dCommandList)
//{
//	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATUREOBJECTIDX, m_d3dCbvGPUDescriptorHandle);
//	UINT ncbElementBytes = ((sizeof(CB_LIGHT_INFO) + 255) & ~255);
//	memset(m_pCBMappedlightInfo, NULL, ncbElementBytes);
//	//XMStoreFloat4(&m_pCBMappedlightInfo->m_xmf4Diffuse,		XMLoadFloat4(&m_LightInfo.m_xmf4Diffuse));
//	//XMStoreFloat4(&m_pCBMappedlightInfo->m_xmf4Specular,	XMLoadFloat4(&m_LightInfo.m_xmf4Specular));
//	//XMStoreFloat4(&m_pCBMappedlightInfo->m_xmf4Ambient,		XMLoadFloat4(&m_LightInfo.m_xmf4Ambient));
//	//XMStoreFloat3(&m_pCBMappedlightInfo->m_xmf3Position,	XMLoadFloat3(&m_LightInfo.m_xmf3Position));
//	//XMStoreFloat(&m_pCBMappedlightInfo->m_fRange,			XMLoadFloat(&m_LightInfo.m_fRange));
//	//XMStoreFloat3(&m_pCBMappedlightInfo->m_xmf3Direction, XMLoadFloat3(&m_LightInfo.m_xmf3Direction));
//	//XMStoreFloat(&m_pCBMappedlightInfo->m_fFalloff, XMLoadFloat(&m_LightInfo.m_fFalloff));
//}

void LightManager::Update(XMFLOAT3 camPos)
{
}

void LightManager::UploadLightInfoToGpu(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(5, m_d3dCbvGPUDescriptorHandle);
	UINT ncbElementBytes = (((sizeof(CB_LIGHT_INFO) * m_nMaxLight) + 255) & ~255);
	memset(m_pCBMappedlightInfo, NULL, ncbElementBytes);

	for (int i = 0; i < m_vLightInfo.size(); i++) {
		m_pCBMappedlightInfo[i].color			= m_vLightInfo[i].color;
		m_pCBMappedlightInfo[i].falloffStart	= m_vLightInfo[i].falloffStart;
		m_pCBMappedlightInfo[i].direction		= m_vLightInfo[i].direction;
		m_pCBMappedlightInfo[i].falloffEnd		= m_vLightInfo[i].falloffEnd;
		m_pCBMappedlightInfo[i].position		= m_vLightInfo[i].position;
		m_pCBMappedlightInfo[i].spotPower		= m_vLightInfo[i].spotPower;
		m_pCBMappedlightInfo[i].type			= m_vLightInfo[i].type;
	}

}

void LightManager::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& cbvCpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& cbvGpuHandle)
{
	D3D12_GPU_VIRTUAL_ADDRESS		d3dGpuVirtualAddress;
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	UINT ncbElementBytes = (((sizeof(CB_LIGHT_INFO) * m_nMaxLight) + 255) & ~255);

	m_pd3dCBResource = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	if (nullptr != m_pd3dCBResource) {
		m_pd3dCBResource->Map(0, NULL, (void**)&m_pCBMappedlightInfo);
		d3dGpuVirtualAddress = m_pd3dCBResource->GetGPUVirtualAddress();
		d3dCBVDesc.SizeInBytes = ncbElementBytes;
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress;
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, cbvCpuHandle);

		m_d3dCbvGPUDescriptorHandle = cbvGpuHandle;
		cbvCpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;
		cbvGpuHandle.ptr += gnCbvSrvDescriptorIncrementSize;
	}

}

UINT LightManager::AddPointLight(XMFLOAT3 color, XMFLOAT3 pos, XMFLOAT2 falloff)
{
	CB_LIGHT_INFO light;
	memset(&light, NULL, sizeof(CB_LIGHT_INFO));
	light.type			= LIGHTTYPE::LIGHT_POINT;
	light.color			= color;
	light.position		= pos;
	light.falloffStart	= falloff.x;
	light.falloffEnd	= falloff.y;
	if (m_vLightInfo.size() < m_nMaxLight) {
		m_vLightInfo.push_back(light);
		return m_vLightInfo.size() - 1;
	}
	assert(!"조명 개수 초과!");
	return m_nMaxLight;
}

UINT LightManager::AddDirectionalLight(XMFLOAT3 color, XMFLOAT3 dir)
{
	CB_LIGHT_INFO light;
	memset(&light, NULL, sizeof(CB_LIGHT_INFO));
	light.type			= LIGHTTYPE::LIGHT_DIRECTIONAL;
	light.direction		= dir;
	light.color			= color;
	if (m_vLightInfo.size() < m_nMaxLight) {
		m_vLightInfo.push_back(light);
		return m_vLightInfo.size() - 1;
	}
	assert(!"조명 개수 초과!");
	return m_nMaxLight;
}

UINT LightManager::AddSpotLight(XMFLOAT3 color, XMFLOAT3 pos, XMFLOAT3 dir, XMFLOAT2 falloff)
{
	CB_LIGHT_INFO light;
	memset(&light, NULL, sizeof(CB_LIGHT_INFO));
	light.type			= LIGHTTYPE::LIGHT_SPOT;
	light.color			= color;
	light.direction		= dir;
	light.position		= pos;
	light.falloffStart	= falloff.x;
	light.falloffEnd	= falloff.y;
	if (m_vLightInfo.size() < m_nMaxLight) {
		m_vLightInfo.push_back(light);
		return m_vLightInfo.size() - 1;
	}
	assert(!"조명 개수 초과!");
	return m_nMaxLight;
}
