#include "stdafx.h"
#include "Light.h"


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
		m_pCBMappedlightInfo[i].shadowIdx		= m_vLightInfo[i].shadowIdx;
		m_pCBMappedlightInfo[i].isEnable		= m_vLightInfo[i].isEnable;
		m_pCBMappedlightInfo[i].isShadow		= m_vLightInfo[i].isShadow;
		XMStoreFloat4x4(&m_pCBMappedlightInfo[i].xmf4x4lightSpaceViewProj, XMMatrixTranspose(XMLoadFloat4x4(&m_vLightInfo[i].xmf4x4lightSpaceViewProj)));
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
	light.isEnable		= true;
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
	light.isEnable		= true;
	light.type			= LIGHTTYPE::LIGHT_DIRECTIONAL;
	light.direction		= Vector3::Normalize(dir);
	light.color			= color;
	if (m_vLightInfo.size() < m_nMaxLight) {
		m_vLightInfo.push_back(light);
		return m_vLightInfo.size() - 1;
	}
	assert(!"조명 개수 초과!");
	return m_nMaxLight;
}

UINT LightManager::AddSpotLight(XMFLOAT3 color, XMFLOAT3 pos, XMFLOAT3 dir, XMFLOAT2 falloff, float spotPower, bool isShadow)
{

	//Light l;
	//l.m_bIsEnable = true;
	//l.m_uLightType = LIGHTTYPE::LIGHT_SPOT;
	//l.m_xmf3LightColor = color;
	//l.m_xmf3Direction = Vector3::Normalize(dir);
	//l.m_xmf3Position = pos;
	//l.m_fFalloffStart = falloff.x;
	//l.m_fFalloffEnd = falloff.y;
	//l.m_fSpotPower = spotPower;
	//if (isShadow) {
	//	l.m_bIsShadow = true;
	//	l.m_uShadowIdx = m_nCurrShadowIdx;
	//	m_nCurrShadowIdx += SpotLightShadowIdxIncrement;
	//}

	CB_LIGHT_INFO light;
	memset(&light, NULL, sizeof(CB_LIGHT_INFO));
	light.isEnable		= true;
	light.type			= LIGHTTYPE::LIGHT_SPOT;
	light.color			= color;
	light.direction		= Vector3::Normalize( dir );
	light.position		= pos;
	light.falloffStart	= falloff.x;
	light.falloffEnd	= falloff.y;
	light.spotPower		= spotPower;
	if (isShadow && (m_nCurrShadowIdx + SpotLightShadowIdxIncrement) < MAXSHADOWIDX) {
		light.isShadow	= true;
		light.shadowIdx = m_nCurrShadowIdx;
		m_nCurrShadowIdx += SpotLightShadowIdxIncrement;

		/*=======================================================================
		* 먼저 뷰 변환 행렬을 만듦.
		* 위치랑 보는 방향, Up 벡터
		* 그 다음 투영 변환 행렬. 이건 다 똑같은거 같음.
		* 10.0f, 1000.f, ASPECT_RATIO, 60.0f
		* 
		* 
		*/
		XMFLOAT4X4 xmf4x4View			= Matrix4x4::LookAtLH(pos, Vector3::Add(pos, dir), XMFLOAT3(0,1,0));
		XMFLOAT4X4 xmf4x4Projection		= Matrix4x4::PerspectiveFovLH(XMConvertToRadians(60.0f), (float)256 / (float)256, 10.0f, 1000.0f);
		light.xmf4x4lightSpaceViewProj	= Matrix4x4::Multiply(xmf4x4View, xmf4x4Projection);
	}

	if (m_vLightInfo.size() < m_nMaxLight) {
		m_vLightInfo.push_back(light);
		return m_vLightInfo.size() - 1;
	}
	assert(!"조명 개수 초과!");
	return m_nMaxLight;
}

Light::Light()
{
	m_uLightType				= 0;
	m_bIsEnable					= true;

	m_xmf3LightColor			= XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_xmf3Position				= XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Direction				= XMFLOAT3(0.0f, -1.0f, 0.0f);;

	m_fFalloffStart				= 300.0f;
	m_fFalloffEnd				= 700.0f;
	m_fSpotPower				= 1.0f;

	m_bIsShadow					= false;
	m_uShadowIdx				= 0;
	m_xmf4x4lightSpaceViewProj	= Matrix4x4::Identity();
}
