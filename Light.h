#pragma once

constexpr UINT SpotLightShadowIdxIncrement = 1;
constexpr UINT PointLightShadowIdxIncrement = 6;
constexpr UINT DirectionalLightShadowIdxIncrement = 3;
constexpr UINT MAXSHADOWIDX = 64;

typedef enum LIGHTTYPE {
	LIGHT_NONE,
	LIGHT_POINT = (UINT)1,
	LIGHT_SPOT = (UINT)2,
	LIGHT_DIRECTIONAL = (UINT)3
}LightType;

struct CB_LIGHT_INFO {
	XMFLOAT4X4	m_xmf4x4ViewProj[6];
	XMFLOAT3	xmf3Color;
	float		fFalloffStart;
	XMFLOAT3	xmf3Direction;
	float		fFalloffEnd;
	XMFLOAT3	xmf3Position;
	float		fSpotPower;
	UINT		uType;
	bool		bIsShadow;
	//bool		padding0;
	//bool		padding1;
	//bool		padding2;
	////UINT		padding3;
	////UINT		padding4;
	//XMFLOAT4X4	padding3;
};

struct LIGHT_DESC {
	LightType	lightType = LightType::LIGHT_NONE;
	XMFLOAT3	xmf3Color = XMFLOAT3(0, 0, 0);
	XMFLOAT3	xmf3Direction = XMFLOAT3(0, 0, 0);
	XMFLOAT3	xmf3Position = XMFLOAT3(0, 0, 0);
	XMFLOAT2	xmf2Falloff = XMFLOAT2(0, 0);
	float		fSpotPower = 0;
	bool		bIsShadow = false;
};

struct Light {
public:
	Light() = delete;
	Light(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescHandle);

	~Light() {
		if (m_pd3dCBResource) {
			m_pd3dCBResource->Unmap(0, NULL);
			m_pd3dCBResource->Release();
		}
	}

	void SetShaderResource(ID3D12GraphicsCommandList* pd3dCommandList);
	LightType GetLightType() { return m_uLightType; }

	void UpdateDirectionalLightOrthographicLH(XMFLOAT4X4 xmf4x4CameraView) {
		XMFLOAT4X4 xmf4x4CameraViewInv = Matrix4x4::Inverse(xmf4x4CameraView);
		for (int i = 0; i < m_nCascade; i++) {
			float minX = FLT_MAX;
			float minY = FLT_MAX;
			float minZ = FLT_MAX;
			float maxX = -FLT_MAX;
			float maxY = -FLT_MAX;
			float maxZ = -FLT_MAX;
			for (int j = 0; j < 8; j++) {
				XMVECTOR temp = XMVector4Transform(XMLoadFloat4(&m_xmf4FrustumCorners[i][j]), XMLoadFloat4x4(&xmf4x4CameraViewInv));
				temp = XMVector4Transform(temp, XMLoadFloat4x4(&m_xmf4x4LightView));
				XMFLOAT4 xmf4Temp;
				XMStoreFloat4(&xmf4Temp, temp);
				minX = min(minX, xmf4Temp.x);
				minY = min(minY, xmf4Temp.y);
				minZ = min(minZ, xmf4Temp.z);
				maxX = max(maxX, xmf4Temp.x);
				maxY = max(maxY, xmf4Temp.y);
				maxZ = max(maxZ, xmf4Temp.z);
			}
			XMFLOAT4X4 xmf4x4Proj = Matrix4x4::OrthographicLH(maxX - minX, maxY - minY, -1000.0, /*maxZ - minZ*/1000.0f);
			XMFLOAT3 newLightPos = Vector3::Multiply(-1, XMFLOAT3(maxX - (maxX - minX) * 0.5f, maxY - (maxY - minY) * 0.5f, minZ));
			//newLightPos = Vector3::Add(newLightPos, Vector3::Multiply(-1, m_xmf3Direction));
			XMFLOAT4X4 xmf4x4Transfrom;
			Matrix4x4::ToTransform(&xmf4x4Transfrom, newLightPos, XMFLOAT4(0, 0, 0, 0));

			m_xmf4x4ViewProj[i] = Matrix4x4::Multiply(Matrix4x4::Multiply(m_xmf4x4LightView, xmf4x4Transfrom), xmf4x4Proj);
		}
	}

private:
	void CreateResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescHandle);
public:
	LightType		m_uLightType;
	bool			m_bIsEnable;

	XMFLOAT3		m_xmf3Color;
	XMFLOAT3		m_xmf3Position;
	XMFLOAT3		m_xmf3Direction;

	float			m_fFalloffStart;
	float			m_fFalloffEnd;
	float			m_fSpotPower;

	bool			m_bIsShadow;
	XMFLOAT4X4		m_xmf4x4ViewProj[6];

	string			m_shadowMapName;

	// For Directional Light Shadow
	float			m_fZ[4];
	XMFLOAT4		m_xmf4FrustumCorners[3][8];
	int			m_nCascade;
	XMFLOAT4X4		m_xmf4x4LightView;
	XMFLOAT4X4		m_xmf4x4LightViewInv;
private:
	ID3D12Resource* m_pd3dCBResource;
	CB_LIGHT_INFO* m_pCBMappedLight;
	D3D12_GPU_DESCRIPTOR_HANDLE m_d3dCbvGPUDescHandle;
};

class LightManager {
public:
	UINT AddPointLight(LIGHT_DESC desc, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescHandle);
	UINT AddDirectionalLight(LIGHT_DESC desc, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescHandle, UINT nCascade = 3);
	UINT AddSpotLight(LIGHT_DESC desc, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescHandle);

	void TurnOn(UINT i) { m_vecLight[i]->m_bIsEnable = true; }
	void TurnOff(UINT i) { m_vecLight[i]->m_bIsEnable = false; }
	void SetPosition(XMFLOAT3 xmf3Position, UINT i) { m_vecLight[i]->m_xmf3Position = xmf3Position; }
	void SetDirection(XMFLOAT3 xmf3Direction, UINT i) { m_vecLight[i]->m_xmf3Direction = xmf3Direction; }
	void SetShadowMapName(const char* name, UINT i) { m_vecLight[i]->m_shadowMapName = name; }

	void SetShaderResource(ID3D12GraphicsCommandList* pd3dCommandList, UINT i) { m_vecLight[i]->SetShaderResource(pd3dCommandList); }
	void UpdateDirectionalLightOrthographicLH(XMFLOAT4X4 xmf4x4CameraView, UINT i) { m_vecLight[i]->UpdateDirectionalLightOrthographicLH(xmf4x4CameraView); }

	UINT GetNumLight() { return (UINT)m_vecLight.size(); }
	bool GetIsShadow(UINT i) { return m_vecLight[i]->m_bIsShadow; }
	string GetShadowMapName(UINT i) { return m_vecLight[i]->m_shadowMapName; }
	LightType GetLightType(UINT i) { return m_vecLight[i]->GetLightType(); }


	void DeleteAll() {
		m_vecLight.clear();
	}
private:
	vector<Light*> m_vecLight;
};
