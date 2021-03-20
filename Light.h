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
	XMFLOAT4X4	xmf4x4lightSpaceViewProj;
	XMFLOAT3	xmf3Color;
	float		fFalloffStart;
	XMFLOAT3	xmf3Direction;
	float		fFalloffEnd;
	XMFLOAT3	xmf3Position;
	float		fSpotPower;
	UINT		uType;
	bool		bIsShadow;
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

	void SetShaderResource(ID3D12GraphicsCommandList* pd3dCommandList);

private:
	void CreateResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescHandle);
public:
	UINT			m_uLightType;
	bool			m_bIsEnable;

	XMFLOAT3		m_xmf3Color;
	XMFLOAT3		m_xmf3Position;
	XMFLOAT3		m_xmf3Direction;

	float			m_fFalloffStart;
	float			m_fFalloffEnd;
	float			m_fSpotPower;

	bool			m_bIsShadow;
	XMFLOAT4X4		m_xmf4x4lightSpaceViewProj;

	string			m_shadowMapName;
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
		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescHandle);
	UINT AddSpotLight(LIGHT_DESC desc, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescHandle);

	void TurnOn(UINT i) { m_vecLight[i]->m_bIsEnable = true; }
	void TurnOff(UINT i) { m_vecLight[i]->m_bIsEnable = false; }
	void SetPosition(XMFLOAT3 xmf3Position, UINT i) { m_vecLight[i]->m_xmf3Position = xmf3Position; }
	void SetDirection(XMFLOAT3 xmf3Direction, UINT i) { m_vecLight[i]->m_xmf3Direction = xmf3Direction; }
	void SetShadowMapName(const char* name, UINT i) { m_vecLight[i]->m_shadowMapName = name; }

	void SetShaderResource(ID3D12GraphicsCommandList* pd3dCommandList, UINT i) { m_vecLight[i]->SetShaderResource(pd3dCommandList); }

	UINT GetNumLight() { return (UINT)m_vecLight.size(); }
	bool GetIsShadow(UINT i) { return m_vecLight[i]->m_bIsShadow; }
	string GetShadowMapName(UINT i) { return m_vecLight[i]->m_shadowMapName; }
private:
	vector<Light*> m_vecLight;
};

//
//constexpr UINT SpotLightShadowIdxIncrement			= 1;
//constexpr UINT PointLightShadowIdxIncrement			= 6;
//constexpr UINT DirectionalLightShadowIdxIncrement	= 3;
//constexpr UINT MAXSHADOWIDX							= 64;
//
//typedef enum LIGHTTYPE {
//	LIGHT_NONE = (UINT)0,
//	LIGHT_POINT,
//	LIGHT_SPOT,
//	LIGHT_DIRECTIONAL
//}LightType;
//
//struct CB_LIGHT_INFO {
//	XMFLOAT4X4	xmf4x4lightSpaceViewProj;
//	XMFLOAT3	color;
//	float		falloffStart;
//	XMFLOAT3	direction;
//	float		falloffEnd;
//	XMFLOAT3	position;
//	float		spotPower;
//	UINT		type;
//	UINT		shadowIdx;
//	bool		isEnable;
//	bool		padding0;
//	short		padding1;
//	bool		isShadow;
//	bool		padding2;
//	short		padding3;
//};
//
//struct LIGHT_DESC {
//	LightType	lightType = LIGHTTYPE::LIGHT_NONE;
//	XMFLOAT3	xmf3Color = XMFLOAT3(0, 0, 0);
//	XMFLOAT3	xmf3Direction = XMFLOAT3(0, 0, 0);
//	XMFLOAT3	xmf3Position = XMFLOAT3(0, 0, 0);
//	XMFLOAT2	xmf2Falloff = XMFLOAT2(0, 0);
//	float		fSpotPower = 0;
//	bool		bIsShadow = false;
//};
//
//
//struct Light {
//public:
//	Light();
//public:
//	UINT		m_uLightType;
//	bool		m_bIsEnable;
//
//	XMFLOAT3	m_xmf3LightColor;
//	XMFLOAT3	m_xmf3Position;
//	XMFLOAT3	m_xmf3Direction;
//	
//	float		m_fFalloffStart;
//	float		m_fFalloffEnd;
//	float		m_fSpotPower;
//	
//	bool		m_bIsShadow;
//	UINT		m_uShadowIdx;
//	XMFLOAT4X4	m_xmf4x4lightSpaceViewProj;
//};
//
//class LightManager {
//public:
//	void Update(XMFLOAT3 camPos);
//	void UploadLightInfoToGpu(ID3D12GraphicsCommandList* pd3dCommandList);
//
//	void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
//		D3D12_CPU_DESCRIPTOR_HANDLE& cbvCpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& cbvGpuHandle);
//
//	UINT AddPointLight(
//		XMFLOAT3 color		= XMFLOAT3(1.0f, 0.0f, 0.0f),
//		XMFLOAT3 pos		= XMFLOAT3(0,0,0),
//		XMFLOAT2 falloff	= XMFLOAT2(300.0f, 500.0f));
//	UINT AddDirectionalLight(
//		XMFLOAT3 color		= XMFLOAT3(0.1f, 0.1f, 0.1f),
//		XMFLOAT3 dir		= XMFLOAT3(0, -1, 0));
//	UINT AddSpotLight(
//		XMFLOAT3 color		= XMFLOAT3(0.0f, 0.0f, 1.0f),
//		XMFLOAT3 pos		= XMFLOAT3(0, 0, -300),
//		XMFLOAT3 dir		= XMFLOAT3(0, 0, 1),
//		XMFLOAT2 falloff	= XMFLOAT2(200.0f, 500.0f),
//		float spotPower		= 1.0f,
//		bool isShadow		= false);
//
//	void SetColor(UINT idx, const XMFLOAT3 color)		{ m_vLightInfo[idx].color = color; }
//	void SetPos(UINT idx, const XMFLOAT3 pos)			{ m_vLightInfo[idx].position = pos; }
//	void SetFalloff(UINT idx, const XMFLOAT2 falloff)	{ m_vLightInfo[idx].falloffStart = falloff.x; m_vLightInfo[idx].falloffEnd = falloff.y; }
//	void SetDir(UINT idx, const XMFLOAT3 dir)			{ m_vLightInfo[idx].direction = dir; }
//
//	XMFLOAT3 GetPosition(UINT idx) { return m_vLightInfo[idx].position; }
//	XMFLOAT3 GetDirection(UINT idx) { return m_vLightInfo[idx].direction; }
//	vector<UINT> GetOnShadowLightIndices() {
//		vector<UINT> result;
//		for (int i = 0; i < m_vLightInfo.size(); i++) {
//			if (m_vLightInfo[i].isShadow) result.push_back(i);
//		}
//		return result;
//	}
//	UINT GetShadowIdx(UINT idx) { return m_vLightInfo[idx].shadowIdx; }
//
//private:
//	UINT						m_nMaxLight = 64;
//	UINT						m_nCurrShadowIdx = 0;
//	D3D12_GPU_DESCRIPTOR_HANDLE m_d3dCbvGPUDescriptorHandle;
//	ID3D12Resource*				m_pd3dCBResource;
//	CB_LIGHT_INFO*				m_pCBMappedlightInfo;
//
//	vector<CB_LIGHT_INFO>		m_vLightInfo;
//	vector<Light>				m_vLight;
//
//};