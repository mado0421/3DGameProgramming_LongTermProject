#include "stdafx.h"
#include "Light.h"


Light::Light(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescHandle)
	: m_uLightType(LightType::LIGHT_NONE)
	, m_bIsEnable(true)
	, m_xmf3Color(XMFLOAT3(1.0f, 1.0f, 1.0f))
	, m_xmf3Position(XMFLOAT3(0, 0, 0))
	, m_xmf3Direction(XMFLOAT3(0, 0, 1))
	, m_fFalloffStart(300.0f)
	, m_fFalloffEnd(500.0f)
	, m_fSpotPower(1.0f)
	, m_bIsShadow(false)
	, m_pd3dCBResource(nullptr)
	, m_pCBMappedLight(nullptr)
{
	for(int i = 0 ; i < 6; i++)	m_xmf4x4ViewProj[i] = Matrix4x4::Identity();

	CreateResource(pd3dDevice, pd3dCommandList);
	CreateConstantBufferView(pd3dDevice, d3dCbvCPUDescHandle);
	m_d3dCbvGPUDescHandle = d3dCbvGPUDescHandle;
	d3dCbvGPUDescHandle.ptr += gnCbvSrvDescriptorIncrementSize;
}

void Light::CreateResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT nConstantBufferBytes = ((sizeof(CB_LIGHT_INFO) + 255) & ~255);

	m_pd3dCBResource = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, nConstantBufferBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
}

void Light::CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescHandle)
{
	D3D12_GPU_VIRTUAL_ADDRESS		d3dGpuVirtualAddress;
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	UINT nConstantBufferBytes = ((sizeof(CB_LIGHT_INFO) + 255) & ~255);

	if (nullptr != m_pd3dCBResource) {
		m_pd3dCBResource->Map(0, NULL, (void**)&m_pCBMappedLight);
		d3dGpuVirtualAddress = m_pd3dCBResource->GetGPUVirtualAddress();
		d3dCBVDesc.SizeInBytes = nConstantBufferBytes;
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress;
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescHandle);

		d3dCbvCPUDescHandle.ptr += gnCbvSrvDescriptorIncrementSize;
	}
}

void Light::SetShaderResource(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURE_LIGHTS, m_d3dCbvGPUDescHandle);

	UINT nConstantBufferBytes = ((sizeof(CB_LIGHT_INFO) + 255) & ~255);
	memset(m_pCBMappedLight, NULL, nConstantBufferBytes);
	XMStoreFloat4x4(&m_pCBMappedLight->m_xmf4x4ViewProj[0], XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4ViewProj[0])));
	XMStoreFloat4x4(&m_pCBMappedLight->m_xmf4x4ViewProj[1], XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4ViewProj[1])));
	XMStoreFloat4x4(&m_pCBMappedLight->m_xmf4x4ViewProj[2], XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4ViewProj[2])));
	XMStoreFloat4x4(&m_pCBMappedLight->m_xmf4x4ViewProj[3], XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4ViewProj[3])));
	XMStoreFloat4x4(&m_pCBMappedLight->m_xmf4x4ViewProj[4], XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4ViewProj[4])));
	XMStoreFloat4x4(&m_pCBMappedLight->m_xmf4x4ViewProj[5], XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4ViewProj[5])));
	memcpy(&m_pCBMappedLight->xmf3Color, &m_xmf3Color, sizeof(XMFLOAT3));
	memcpy(&m_pCBMappedLight->fFalloffStart, &m_fFalloffStart, sizeof(float));
	memcpy(&m_pCBMappedLight->xmf3Direction, &m_xmf3Direction, sizeof(XMFLOAT3));
	memcpy(&m_pCBMappedLight->fFalloffEnd, &m_fFalloffEnd, sizeof(float));
	memcpy(&m_pCBMappedLight->xmf3Position, &m_xmf3Position, sizeof(XMFLOAT3));
	memcpy(&m_pCBMappedLight->fSpotPower, &m_fSpotPower, sizeof(float));
	memcpy(&m_pCBMappedLight->uType, &m_uLightType, sizeof(UINT));
	memcpy(&m_pCBMappedLight->bIsShadow, &m_bIsShadow, sizeof(bool));

}

UINT LightManager::AddPointLight(LIGHT_DESC desc, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescHandle)
{
	Light* temp = new Light(pd3dDevice, pd3dCommandList, d3dCbvCPUDescHandle, d3dCbvGPUDescHandle);
	temp->m_uLightType = LIGHTTYPE::LIGHT_POINT;
	temp->m_xmf3Position = desc.xmf3Position;
	temp->m_fFalloffStart = desc.xmf2Falloff.x;
	temp->m_fFalloffEnd = desc.xmf2Falloff.y;
	temp->m_xmf3Color = desc.xmf3Color;
	temp->m_bIsShadow = desc.bIsShadow;
	temp->m_bIsEnable = true;

	XMFLOAT4X4 xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(90.0f), 1, 0.1f, 1000.0f);

	XMFLOAT3 x = XMFLOAT3(1, 0, 0);
	XMFLOAT3 y = XMFLOAT3(0, 1, 0);
	XMFLOAT3 z = XMFLOAT3(0, 0, 1);
	XMFLOAT4X4 m_xmf4x4Temp = Matrix4x4::Identity();
	XMFLOAT3 look, up, right;

	int i = 0;

	/*========================================================================
	* +x
	*=======================================================================*/
	look = x;
	up = y;
	right = Vector3::Multiply(-1, z);

	m_xmf4x4Temp._11 = right.x; m_xmf4x4Temp._12 = up.x; m_xmf4x4Temp._13 = look.x;
	m_xmf4x4Temp._21 = right.y; m_xmf4x4Temp._22 = up.y; m_xmf4x4Temp._23 = look.y;
	m_xmf4x4Temp._31 = right.z; m_xmf4x4Temp._32 = up.z; m_xmf4x4Temp._33 = look.z;
	m_xmf4x4Temp._41 = -Vector3::DotProduct(desc.xmf3Position, right);
	m_xmf4x4Temp._42 = -Vector3::DotProduct(desc.xmf3Position, up);
	m_xmf4x4Temp._43 = -Vector3::DotProduct(desc.xmf3Position, look);

	temp->m_xmf4x4ViewProj[i++] = Matrix4x4::Multiply(m_xmf4x4Temp, xmf4x4Projection);

	/*========================================================================
	* -x
	*=======================================================================*/
	look = Vector3::Multiply(-1, x);
	up = y;
	right = z;

	m_xmf4x4Temp._11 = right.x; m_xmf4x4Temp._12 = up.x; m_xmf4x4Temp._13 = look.x;
	m_xmf4x4Temp._21 = right.y; m_xmf4x4Temp._22 = up.y; m_xmf4x4Temp._23 = look.y;
	m_xmf4x4Temp._31 = right.z; m_xmf4x4Temp._32 = up.z; m_xmf4x4Temp._33 = look.z;
	m_xmf4x4Temp._41 = -Vector3::DotProduct(desc.xmf3Position, right);
	m_xmf4x4Temp._42 = -Vector3::DotProduct(desc.xmf3Position, up);
	m_xmf4x4Temp._43 = -Vector3::DotProduct(desc.xmf3Position, look);

	temp->m_xmf4x4ViewProj[i++] = Matrix4x4::Multiply(m_xmf4x4Temp, xmf4x4Projection);

	/*========================================================================
	* +y
	*=======================================================================*/
	look = y;
	up = Vector3::Multiply(-1, z);
	right = x;

	m_xmf4x4Temp._11 = right.x; m_xmf4x4Temp._12 = up.x; m_xmf4x4Temp._13 = look.x;
	m_xmf4x4Temp._21 = right.y; m_xmf4x4Temp._22 = up.y; m_xmf4x4Temp._23 = look.y;
	m_xmf4x4Temp._31 = right.z; m_xmf4x4Temp._32 = up.z; m_xmf4x4Temp._33 = look.z;
	m_xmf4x4Temp._41 = -Vector3::DotProduct(desc.xmf3Position, right);
	m_xmf4x4Temp._42 = -Vector3::DotProduct(desc.xmf3Position, up);
	m_xmf4x4Temp._43 = -Vector3::DotProduct(desc.xmf3Position, look);

	temp->m_xmf4x4ViewProj[i++] = Matrix4x4::Multiply(m_xmf4x4Temp, xmf4x4Projection);

	/*========================================================================
	* -y
	*=======================================================================*/
	look = Vector3::Multiply(-1, y);
	up = z;
	right = x;

	m_xmf4x4Temp._11 = right.x; m_xmf4x4Temp._12 = up.x; m_xmf4x4Temp._13 = look.x;
	m_xmf4x4Temp._21 = right.y; m_xmf4x4Temp._22 = up.y; m_xmf4x4Temp._23 = look.y;
	m_xmf4x4Temp._31 = right.z; m_xmf4x4Temp._32 = up.z; m_xmf4x4Temp._33 = look.z;
	m_xmf4x4Temp._41 = -Vector3::DotProduct(desc.xmf3Position, right);
	m_xmf4x4Temp._42 = -Vector3::DotProduct(desc.xmf3Position, up);
	m_xmf4x4Temp._43 = -Vector3::DotProduct(desc.xmf3Position, look);

	temp->m_xmf4x4ViewProj[i++] = Matrix4x4::Multiply(m_xmf4x4Temp, xmf4x4Projection);

	/*========================================================================
	* +z
	*=======================================================================*/
	look = z;
	up = y;
	right = x;

	m_xmf4x4Temp._11 = right.x; m_xmf4x4Temp._12 = up.x; m_xmf4x4Temp._13 = look.x;
	m_xmf4x4Temp._21 = right.y; m_xmf4x4Temp._22 = up.y; m_xmf4x4Temp._23 = look.y;
	m_xmf4x4Temp._31 = right.z; m_xmf4x4Temp._32 = up.z; m_xmf4x4Temp._33 = look.z;
	m_xmf4x4Temp._41 = -Vector3::DotProduct(desc.xmf3Position, right);
	m_xmf4x4Temp._42 = -Vector3::DotProduct(desc.xmf3Position, up);
	m_xmf4x4Temp._43 = -Vector3::DotProduct(desc.xmf3Position, look);

	temp->m_xmf4x4ViewProj[i++] = Matrix4x4::Multiply(m_xmf4x4Temp, xmf4x4Projection);

	/*========================================================================
	* -z
	*=======================================================================*/
	look = Vector3::Multiply(-1, z);
	up = y;
	right = Vector3::Multiply(-1, x);

	m_xmf4x4Temp._11 = right.x; m_xmf4x4Temp._12 = up.x; m_xmf4x4Temp._13 = look.x;
	m_xmf4x4Temp._21 = right.y; m_xmf4x4Temp._22 = up.y; m_xmf4x4Temp._23 = look.y;
	m_xmf4x4Temp._31 = right.z; m_xmf4x4Temp._32 = up.z; m_xmf4x4Temp._33 = look.z;
	m_xmf4x4Temp._41 = -Vector3::DotProduct(desc.xmf3Position, right);
	m_xmf4x4Temp._42 = -Vector3::DotProduct(desc.xmf3Position, up);
	m_xmf4x4Temp._43 = -Vector3::DotProduct(desc.xmf3Position, look);

	temp->m_xmf4x4ViewProj[i++] = Matrix4x4::Multiply(m_xmf4x4Temp, xmf4x4Projection);

	m_vecLight.push_back(temp);
	return ((UINT)m_vecLight.size() - 1);
}

UINT LightManager::AddDirectionalLight(LIGHT_DESC desc, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescHandle, UINT nCascade)
{
	Light* temp = new Light(pd3dDevice, pd3dCommandList, d3dCbvCPUDescHandle, d3dCbvGPUDescHandle);
	temp->m_uLightType = LIGHTTYPE::LIGHT_DIRECTIONAL;
	temp->m_xmf3Direction = desc.xmf3Direction;
	temp->m_xmf3Color = desc.xmf3Color;
	temp->m_bIsShadow = desc.bIsShadow;
	temp->m_bIsEnable = true;

	/*
	* 필요한 것: m_pCamera의 m_xmf4x4ViewInv, light의 ViewMatrix, Cascade별 Zn, Zf, FOV, Cascade별 행렬을 담을 array
	* 생각해보니까 Z가 (0~1)로 정규화되는건 NDC 상황에서였음. Z[] = { 0.0f, 0.2f, 0.5f, 1.0f } 일 때,
	* Zni = 0.1f + Z[i] * ( 1000.0f - 0.1f );
	* Zfi = 0.1f + Z[i + 1] * ( 1000.0f - 0.1f );
	* (지금 내가 Zn이랑 Zf를 0.1f 랑 1000.0f으로 주고 있으니까)
	* 
	* 
	* ㅋㅋ 지금은 nCascade 안 쓰고 무조건 3으로 할거임 아 ㅋㅋ
	*/

	temp->m_nCascade = nCascade;
	temp->m_fZ[0] = 0.0f;
	temp->m_fZ[1] = 0.006f;
	temp->m_fZ[2] = 0.02f;
	temp->m_fZ[3] = 0.06f;
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(temp->m_xmf3Position, Vector3::Add(temp->m_xmf3Position, temp->m_xmf3Direction), XMFLOAT3(0, 1, 0));

	for (int i = 0; i < 3; i++) {
		float Zn = 0.1f + temp->m_fZ[i] * (1000.0f - 0.1f);
		float Zf = 0.1f + temp->m_fZ[i + 1] * (1000.0f - 0.1f);
		float Xn = Zn * tan(XMConvertToRadians(60.0f) * 0.5f);
		float Yn = Zn * tan(XMConvertToRadians(60.0f) * 0.5f);
		float Xf = Zf * tan(XMConvertToRadians(60.0f) * 0.5f);
		float Yf = Zf * tan(XMConvertToRadians(60.0f) * 0.5f);

		// Near Side
		temp->m_xmf4FrustumCorners[i][0] = XMFLOAT4(Xn, Yn, Zn, 1.0f);
		temp->m_xmf4FrustumCorners[i][1] = XMFLOAT4(-Xn, Yn, Zn, 1.0f);
		temp->m_xmf4FrustumCorners[i][2] = XMFLOAT4(Xn, -Yn, Zn, 1.0f);
		temp->m_xmf4FrustumCorners[i][3] = XMFLOAT4(-Xn, -Yn, Zn, 1.0f);

		// Far Side
		temp->m_xmf4FrustumCorners[i][4] = XMFLOAT4(Xf, Yf, Zf, 1.0f);
		temp->m_xmf4FrustumCorners[i][5] = XMFLOAT4(-Xf, Yf, Zf, 1.0f);
		temp->m_xmf4FrustumCorners[i][6] = XMFLOAT4(Xf, -Yf, Zf, 1.0f);
		temp->m_xmf4FrustumCorners[i][7] = XMFLOAT4(-Xf, -Yf, Zf, 1.0f);
	}
	temp->m_xmf4x4View = Matrix4x4::LookAtLH(XMFLOAT3(0, 0, 0), temp->m_xmf3Direction, XMFLOAT3(0, 1, 0));

	m_vecLight.push_back(temp);
	return ((UINT)m_vecLight.size() - 1);
}

UINT LightManager::AddSpotLight(LIGHT_DESC desc, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescHandle)
{
	Light* temp = new Light(pd3dDevice, pd3dCommandList, d3dCbvCPUDescHandle, d3dCbvGPUDescHandle);
	temp->m_uLightType = LIGHTTYPE::LIGHT_SPOT;
	temp->m_xmf3Direction = desc.xmf3Direction;
	temp->m_xmf3Position = desc.xmf3Position;
	temp->m_xmf3Color = desc.xmf3Color;
	temp->m_fFalloffStart = desc.xmf2Falloff.x;
	temp->m_fFalloffEnd = desc.xmf2Falloff.y;
	temp->m_fSpotPower = desc.fSpotPower;
	temp->m_bIsShadow = desc.bIsShadow;
	temp->m_bIsEnable = true;

	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(temp->m_xmf3Position, Vector3::Add(temp->m_xmf3Position, temp->m_xmf3Direction), XMFLOAT3(0, 1, 0));
	XMFLOAT4X4 xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(120.0f), 1, 0.1f, 1000.0f);
	temp->m_xmf4x4ViewProj[0] = Matrix4x4::Multiply(xmf4x4View, xmf4x4Projection);

	m_vecLight.push_back(temp);
	return ((UINT)m_vecLight.size() - 1);
}
