#include "stdafx.h"
#include "Scene.h"
#include "Object.h"
#include "PipelineStateObject.h"
#include "Camera.h"
#include "Framework.h"
#include "Texture.h"
#include "Light.h"
#include "Importer.h"
#include "Vertex.h"
#include "Model.h"

ID3D12RootSignature* Scene::CreateRootSignature()
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;
	D3D12_DESCRIPTOR_RANGE d3dDescriptorRange[9];

	d3dDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	d3dDescriptorRange[0].NumDescriptors = 1;
	d3dDescriptorRange[0].BaseShaderRegister = ROOTSIGNATURE_OBJECTS;
	d3dDescriptorRange[0].RegisterSpace = 0;
	d3dDescriptorRange[0].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	d3dDescriptorRange[1].NumDescriptors = 1;
	d3dDescriptorRange[1].BaseShaderRegister = ROOTSIGNATURE_LIGHTS;
	d3dDescriptorRange[1].RegisterSpace = 0;
	d3dDescriptorRange[1].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	d3dDescriptorRange[2].NumDescriptors = 1;
	d3dDescriptorRange[2].BaseShaderRegister = ROOTSIGNATURE_COLOR_TEXTURE;
	d3dDescriptorRange[2].RegisterSpace = 0;
	d3dDescriptorRange[2].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	d3dDescriptorRange[3].NumDescriptors = 1;
	d3dDescriptorRange[3].BaseShaderRegister = ROOTSIGNATURE_NORMAL_TEXTURE;
	d3dDescriptorRange[3].RegisterSpace = 0;
	d3dDescriptorRange[3].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	d3dDescriptorRange[4].NumDescriptors = 1;
	d3dDescriptorRange[4].BaseShaderRegister = ROOTSIGNATURE_DEPTH_TEXTURE;
	d3dDescriptorRange[4].RegisterSpace = 0;
	d3dDescriptorRange[4].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	d3dDescriptorRange[5].NumDescriptors = 1;
	d3dDescriptorRange[5].BaseShaderRegister = ROOTSIGNATURE_SHADOW_TEXTURE;
	d3dDescriptorRange[5].RegisterSpace = 0;
	d3dDescriptorRange[5].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[6].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	d3dDescriptorRange[6].NumDescriptors = 1;
	d3dDescriptorRange[6].BaseShaderRegister = ROOTSIGNATURE_CUBE_TEXTURE;
	d3dDescriptorRange[6].RegisterSpace = 0;
	d3dDescriptorRange[6].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[7].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	d3dDescriptorRange[7].NumDescriptors = 1;
	d3dDescriptorRange[7].BaseShaderRegister = ROOTSIGNATURE_SHADOWARRAY_TEXTURE;
	d3dDescriptorRange[7].RegisterSpace = 0;
	d3dDescriptorRange[7].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[8].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	d3dDescriptorRange[8].NumDescriptors = 1;
	d3dDescriptorRange[8].BaseShaderRegister = ROOTSIGNATURE_ANIMTRANSFORM;
	d3dDescriptorRange[8].RegisterSpace = 0;
	d3dDescriptorRange[8].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[10];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = ROOTSIGNATURE_PASSCONSTANTS;
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[1].DescriptorTable.pDescriptorRanges = &d3dDescriptorRange[0];
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &d3dDescriptorRange[1];
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[3].DescriptorTable.pDescriptorRanges = &d3dDescriptorRange[2];
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &d3dDescriptorRange[3];
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[5].DescriptorTable.pDescriptorRanges = &d3dDescriptorRange[4];
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &d3dDescriptorRange[5];
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[7].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[7].DescriptorTable.pDescriptorRanges = &d3dDescriptorRange[6];
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[8].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[8].DescriptorTable.pDescriptorRanges = &d3dDescriptorRange[7];
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[9].DescriptorTable.pDescriptorRanges = &d3dDescriptorRange[8];
	pd3dRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc[2];
	::ZeroMemory(&d3dSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC) * 2);
	d3dSamplerDesc[0].Filter = D3D12_FILTER_ANISOTROPIC;
	d3dSamplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc[0].MipLODBias = 0;
	d3dSamplerDesc[0].MaxAnisotropy = 1;
	d3dSamplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc[0].MinLOD = 0;
	d3dSamplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc[0].ShaderRegister = 0;
	d3dSamplerDesc[0].RegisterSpace = 0;
	d3dSamplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	d3dSamplerDesc[1].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[1].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	d3dSamplerDesc[1].MipLODBias = 0;
	d3dSamplerDesc[1].MaxAnisotropy = 1;
	d3dSamplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dSamplerDesc[1].MinLOD = 0;
	d3dSamplerDesc[1].MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc[1].ShaderRegister = 1;
	d3dSamplerDesc[1].RegisterSpace = 0;
	d3dSamplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags
		= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT 
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 2;
	d3dRootSignatureDesc.pStaticSamplers = &d3dSamplerDesc[0];
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	HRESULT isSuccess = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &pd3dSignatureBlob, &pd3dErrorBlob);
	isSuccess = m_pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return pd3dGraphicsRootSignature;
}

void Scene::Init(Framework* pFramework, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	/*========================================================================
	* 주요 변수 초기화
	*=======================================================================*/
	m_pd3dDevice		= pd3dDevice;
	m_pd3dCommandList	= pd3dCommandList;
	m_pd3dRootSignature = CreateRootSignature();

	/*========================================================================
	* 카메라 설정
	*=======================================================================*/
	m_pCamera = new FollowCamera();
	m_pCamera->SetPosition(XMFLOAT3(0, 0, -1));
	m_pCamera->SetLookAt(XMFLOAT3(0, 0, 0));

	/*========================================================================
	* 디스크립터 힙 생성
	*=======================================================================*/
	CreateDescriptorHeap();


	/*========================================================================
	* PassConstants 생성
	*=======================================================================*/
	CreatePassInfoShaderResource();




	/*========================================================================
	* 텍스쳐
	*=======================================================================*/
	gTextureMng.Initialize(m_pd3dDevice);
	gTextureMng.AddDepthBufferTexture("GBuffer_Depth", m_pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	gTextureMng.AddRenderTargetTexture("GBuffer_Color", m_pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	gTextureMng.AddRenderTargetTexture("GBuffer_Normal", m_pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);

	gTextureMng.LoadFromFile("defaultDiffuseMap", m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	gTextureMng.LoadFromFile("defaultNormalMap", m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	gTextureMng.LoadFromFile("defaultSpecularMap", m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	//gTextureMng.LoadFromFile("mech_diff", m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	//gTextureMng.LoadFromFile("specularTest",  m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);


	/*========================================================================
	* 마테리얼
	*=======================================================================*/
	MaterialDataImporter matDataImporter;
	matDataImporter.Load("Data/MaterialData.txt");


	/*========================================================================
	* 모델
	*=======================================================================*/
	gModelMng.Initialize();
	//gModelMng.AddModel("mech", m_pd3dDevice, m_pd3dCommandList);
	//gModelMng.AddModel("floor", m_pd3dDevice, m_pd3dCommandList);


	/*========================================================================
	* 에셋 리스트 로드해서 모델이랑 텍스처를 동시에 로드
	*=======================================================================*/
	AssetListDataImporter assetImporter;
	assetImporter.Load(m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);

	/*========================================================================
	* Pass 1 전용 오브젝트 데이터 로드 및 생성
	*=======================================================================*/
	ObjectDataImporter objDataImporter;
	vector<OBJECT_DESC> vecObjDesc = objDataImporter.Load("Data/ObjectData.txt");

	for (int i = 0; i < vecObjDesc.size(); i++) {
		if (strcmp(vecObjDesc[i].model.c_str(), "") != 0) {
			/*========================================================================
			* 여기까지 작업했음
			* 
			* - 오브젝트 데이터에서 마테리얼 이름을 읽어오는 부분까지 함.
			* - 마테리얼을 오브젝트가 가지게 하고
			* - 오브젝트 렌더를 할 때, gModelMng와 gMaterialMng를 통해서 렌더링 할 수 있도록 구조 변경할 것.
			* 
			*=======================================================================*/
			if (vecObjDesc[i].isAnimated) {
				AnimatedObject* tempObj = new AnimatedObject(m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
				tempObj->Move(vecObjDesc[i].position);
				tempObj->SetModel(vecObjDesc[i].model.c_str());
				tempObj->SetMaterial(vecObjDesc[i].material.c_str());
				m_vecAnimObject.push_back(tempObj);
			}
			else {
				Object* tempObj = new Object(m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
				tempObj->Move(vecObjDesc[i].position);
				tempObj->SetModel(vecObjDesc[i].model.c_str());
				tempObj->SetMaterial(vecObjDesc[i].material.c_str());
				m_vecObject.push_back(tempObj);
			}
		}
	}

	/*========================================================================
	* Pass 2 전용 디버그 윈도우 생성
	*=======================================================================*/
	for (int i = 0; i < 3; i++) {
		DebugWindowObject* temp = new DebugWindowObject(m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		temp->Move(XMFLOAT3(-0.5f * i, 1, 0));

		m_vecDebugWindow.push_back(temp);
	}

	/*========================================================================
	* Pass 2 전용 전체 화면 사각형
	*=======================================================================*/
	DebugWindowObject* tempScreen = new DebugWindowObject(m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle, true);
	m_vecDebugWindow.push_back(tempScreen);




	/*========================================================================
	* 광원과 광원별 그림자 텍스처 생성
	*=======================================================================*/
	LightDataImporter lightDataImporter;
	vector<LIGHT_DESC> vecLightDesc = lightDataImporter.Load("Data/LightData.txt");
	string shadow("ShadowMap_");
	m_LightMng = new LightManager();
	for (int i = 0; i < vecLightDesc.size(); i++) {
		switch (vecLightDesc[i].lightType)
		{
		case LightType::LIGHT_POINT:		
			m_LightMng->AddPointLight(vecLightDesc[i], m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);		
			
			break;
		case LightType::LIGHT_DIRECTIONAL:	
			m_LightMng->AddDirectionalLight(vecLightDesc[i], m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle, 3);	
			
			break;
		case LightType::LIGHT_SPOT:			
			m_LightMng->AddSpotLight(vecLightDesc[i], m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);		
			
			break;
		case LightType::LIGHT_NONE:
		default:
			break;
		}

		if (vecLightDesc[i].bIsShadow) {
			string temp = to_string(i);
			temp = shadow + temp;
			switch (vecLightDesc[i].lightType)
			{
			case LightType::LIGHT_POINT:
				gTextureMng.AddDepthBufferTextureCube(temp.c_str(), m_pd3dDevice, SHADOWMAPSIZE, SHADOWMAPSIZE, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);

				break;

			case LightType::LIGHT_SPOT:
				gTextureMng.AddDepthBufferTexture(temp.c_str(), m_pd3dDevice, SHADOWMAPSIZE, SHADOWMAPSIZE, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);

				break;
			case LightType::LIGHT_DIRECTIONAL:
				gTextureMng.AddDepthBufferTextureArray(temp.c_str(), 3, m_pd3dDevice, SHADOWMAPSIZE, SHADOWMAPSIZE, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);


				break;
			case LightType::LIGHT_NONE:
			default:
				break;
			}
			m_LightMng->SetShadowMapName(temp.c_str(), i);
		}
	}

	/*========================================================================
	* PSO 생성
	*=======================================================================*/
	CreatePSO();
}

void Scene::RenderPass1()
{
	/*========================================================================
	* 루트 시그니처, 디스크립터 힙, 파이프라인 스테이트, 메쉬 토폴로지 설정
	*=======================================================================*/
	m_pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature);
	m_pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);
	m_pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	/*========================================================================
	* PassInfo 설정
	*=======================================================================*/
	m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	m_pCamera->SetViewportsAndScissorRects(m_pd3dCommandList);
	UpdatePassInfoAboutCamera();

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbPassInfo->GetGPUVirtualAddress();
	m_pd3dCommandList->SetGraphicsRootConstantBufferView(ROOTSIGNATURE_PASSCONSTANTS, d3dGpuVirtualAddress);

	XMFLOAT4X4 texture = {
	0.5f,		0,		0,		0,
	0,		-0.5f,		0,		0,
	0,			0,	 1.0f,		0,
	0.5f,	 0.5f,		0,	 1.0f };
	XMStoreFloat4x4(&m_pcbMappedPassInfo->m_xmf4x4TextureTransform, XMMatrixTranspose(XMLoadFloat4x4(&texture)));




	/*========================================================================
	* Pass 1. 메쉬 렌더 To Color, Normal, Depth
	*=======================================================================*/
	D3D12_RESOURCE_BARRIER d3dResourceBarrier[3];
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier[0].Transition.pResource = gTextureMng.GetTextureResource("GBuffer_Depth");
	d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
	d3dResourceBarrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	d3dResourceBarrier[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	d3dResourceBarrier[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier[1].Transition.pResource = gTextureMng.GetTextureResource("GBuffer_Color");
	d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	d3dResourceBarrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	d3dResourceBarrier[2].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier[2].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier[2].Transition.pResource = gTextureMng.GetTextureResource("GBuffer_Normal");
	d3dResourceBarrier[2].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	d3dResourceBarrier[2].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier[2].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(3, d3dResourceBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = gTextureMng.GetDsvCPUHandle("GBuffer_Depth");
	m_pd3dCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[2];
	rtvHandle[0] = gTextureMng.GetRtvCPUHandle("GBuffer_Color");
	rtvHandle[1] = gTextureMng.GetRtvCPUHandle("GBuffer_Normal");

	float pfClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_pd3dCommandList->ClearRenderTargetView(rtvHandle[0], pfClearColor, 0, NULL);
	m_pd3dCommandList->ClearRenderTargetView(rtvHandle[1], pfClearColor, 0, NULL);

	m_pd3dCommandList->OMSetRenderTargets(2, rtvHandle, FALSE, &dsvHandle);

	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["PackGBuffer"]);
	for (auto iter = m_vecObject.begin(); iter != m_vecObject.end(); iter++) (*iter)->Render(m_pd3dCommandList);
	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["AnimatedObject"]);
	for (auto iter = m_vecAnimObject.begin(); iter != m_vecAnimObject.end(); iter++) (*iter)->Render(m_pd3dCommandList);

	d3dResourceBarrier[0].Transition.pResource = gTextureMng.GetTextureResource("GBuffer_Depth");
	d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	d3dResourceBarrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	d3dResourceBarrier[1].Transition.pResource = gTextureMng.GetTextureResource("GBuffer_Color");
	d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	d3dResourceBarrier[2].Transition.pResource = gTextureMng.GetTextureResource("GBuffer_Normal");
	d3dResourceBarrier[2].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier[2].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	m_pd3dCommandList->ResourceBarrier(3, d3dResourceBarrier);

	/*========================================================================
	* Pass 1. 광원별 그림자맵 렌더
	*=======================================================================*/
	m_pCamera->SetViewport(0, 0, SHADOWMAPSIZE, SHADOWMAPSIZE, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, SHADOWMAPSIZE, SHADOWMAPSIZE);

	m_pCamera->SetViewportsAndScissorRects(m_pd3dCommandList);

	for (UINT i = 0; i < m_LightMng->GetNumLight(); i++) {
		if (m_LightMng->GetIsShadow(i)) {
			m_LightMng->SetShaderResource(m_pd3dCommandList, i);

			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = gTextureMng.GetDsvCPUHandle(m_LightMng->GetShadowMapName(i).c_str());
			m_pd3dCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
			m_pd3dCommandList->OMSetRenderTargets(0, NULL, TRUE, &dsvHandle);

			switch (m_LightMng->GetLightType(i))
			{
			case LightType::LIGHT_SPOT: 

				m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["SpotLightShadow"]); 
				for (int i = 0; i < m_vecObject.size(); i++) m_vecObject[i]->Render(m_pd3dCommandList);

				m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["SpotLightShadowAnim"]);
				for (auto iter = m_vecAnimObject.begin(); iter != m_vecAnimObject.end(); iter++) (*iter)->Render(m_pd3dCommandList);
				break;
			case LightType::LIGHT_POINT: 

				m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["PointLightShadow"]); 
				for (int i = 0; i < m_vecObject.size(); i++) m_vecObject[i]->Render(m_pd3dCommandList);

				m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["PointLightShadowAnim"]);
				for (auto iter = m_vecAnimObject.begin(); iter != m_vecAnimObject.end(); iter++) (*iter)->Render(m_pd3dCommandList);
				break;
			case LightType::LIGHT_DIRECTIONAL:

				m_LightMng->UpdateDirectionalLightOrthographicLH(m_pCamera->GetViewMatrix(), i);
				m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["DirectionalLightShadow"]);
				for (int i = 0; i < m_vecObject.size(); i++) m_vecObject[i]->Render(m_pd3dCommandList);

				m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["DirectionalLightShadowAnim"]);
				for (auto iter = m_vecAnimObject.begin(); iter != m_vecAnimObject.end(); iter++) (*iter)->Render(m_pd3dCommandList);
				break;
			case LightType::LIGHT_NONE:
			default:
				break;
			}

		}
	}
}
void Scene::RenderPass2()
{
	/*========================================================================
	* 루트 시그니처, 디스크립터 힙, 메쉬 토폴로지 설정
	* 
	* 근데 PrevRender에서 했던 설정값이 저장되진 않을까?
	*=======================================================================*/
	//m_pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature);				
	//m_pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);			
	//m_pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/*========================================================================
	* PassInfo 설정
	*=======================================================================*/
	m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

	m_pCamera->SetViewportsAndScissorRects(m_pd3dCommandList);

	/*========================================================================
	* Pass 2. 스크린 렌더
	*=======================================================================*/
	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["ColorFromGBuffer"]);
	gTextureMng.UseForShaderResource("GBuffer_Normal", m_pd3dCommandList, ROOTSIGNATURE_NORMAL_TEXTURE);
	gTextureMng.UseForShaderResource("GBuffer_Depth", m_pd3dCommandList, ROOTSIGNATURE_DEPTH_TEXTURE);
	gTextureMng.UseForShaderResource("GBuffer_Color", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
	m_vecDebugWindow[3]->Render(m_pd3dCommandList);


	/*========================================================================
	* Pass 2. 광원별 렌더
	*=======================================================================*/
	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["AddLight"]);

	for (UINT i = 0; i < m_LightMng->GetNumLight(); i++) {
		m_LightMng->SetShaderResource(m_pd3dCommandList, i);

		if (m_LightMng->GetIsShadow(i)) {
			switch (m_LightMng->GetLightType(i))
			{
			case LightType::LIGHT_SPOT: gTextureMng.UseForShaderResource(m_LightMng->GetShadowMapName(i).c_str(), m_pd3dCommandList, ROOTSIGNATURE_SHADOW_TEXTURE); break;
			case LightType::LIGHT_POINT: gTextureMng.UseForShaderResource(m_LightMng->GetShadowMapName(i).c_str(), m_pd3dCommandList, ROOTSIGNATURE_CUBE_TEXTURE); break;
			case LightType::LIGHT_DIRECTIONAL:gTextureMng.UseForShaderResource(m_LightMng->GetShadowMapName(i).c_str(), m_pd3dCommandList, ROOTSIGNATURE_SHADOWARRAY_TEXTURE); break;
			case LightType::LIGHT_NONE:
			default:
				break;
			}
		}

		m_vecDebugWindow[3]->Render(m_pd3dCommandList);
	}

	/*========================================================================
	* Pass 2. Debug Window 렌더
	*=======================================================================*/
	if (test) {
		m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["DebugColor"]);
		gTextureMng.UseForShaderResource("GBuffer_Color", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
		m_vecDebugWindow[0]->Render(m_pd3dCommandList);
		gTextureMng.UseForShaderResource("GBuffer_Normal", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
		m_vecDebugWindow[1]->Render(m_pd3dCommandList);

		gTextureMng.UseForShaderResource("GBuffer_Depth", m_pd3dCommandList, ROOTSIGNATURE_DEPTH_TEXTURE);
		m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["DebugDepth"]);
		m_vecDebugWindow[2]->Render(m_pd3dCommandList);
	}



}
void Scene::Update(float fTimeElapsed)
{
	m_pCamera->Update(fTimeElapsed);

	m_fCurrentTime += fTimeElapsed;
	::memcpy(&m_pcbMappedPassInfo->m_xmfCurrentTime, &m_fCurrentTime, sizeof(float));


	for (auto iter = m_vecObject.begin(); iter != m_vecObject.end(); iter++) (*iter)->Update(fTimeElapsed);
	for (auto iter = m_vecAnimObject.begin(); iter != m_vecAnimObject.end(); iter++) (*iter)->Update(fTimeElapsed);
	//for_each(m_vecObject.begin(), m_vecObject.end(), [fTimeElapsed](Object* o) {o->Rotate(XMFLOAT3(0, 30 * fTimeElapsed, 0)); });

}
void Scene::Input(UCHAR* pKeyBuffer, float fTimeElapsed)
{
	if (pKeyBuffer[KeyCode::_W] & 0xF0) { m_pCamera->MoveForward(2); }
	if (pKeyBuffer[KeyCode::_A] & 0xF0) { m_pCamera->MoveLeft(2); }
	if (pKeyBuffer[KeyCode::_S] & 0xF0) { m_pCamera->MoveBackward(2); }
	if (pKeyBuffer[KeyCode::_D] & 0xF0) { m_pCamera->MoveRight(2); }
	if (pKeyBuffer[KeyCode::_R] & 0xF0) { m_pCamera->MoveUp(2); }
	if (pKeyBuffer[KeyCode::_F] & 0xF0) { m_pCamera->MoveDown(2); }
	if (pKeyBuffer[KeyCode::_Q] & 0xF0) { m_pCamera->Rotate(0, -50 * fTimeElapsed, 0); }
	if (pKeyBuffer[KeyCode::_E] & 0xF0) { m_pCamera->Rotate(0, 50 * fTimeElapsed, 0); }
	if (pKeyBuffer[KeyCode::_Z] & 0xF0) { m_pCamera->Rotate(50 * fTimeElapsed, 0, 0); }
	if (pKeyBuffer[KeyCode::_X] & 0xF0) { m_pCamera->Rotate(-50 * fTimeElapsed, 0, 0); }

	if (pKeyBuffer[KeyCode::_1] & 0xF0) {
		m_pCamera->SetPosition(XMFLOAT3(0, 1, 0));
		m_pCamera->SetLookAt(XMFLOAT3(0, 1, 1));
	}
	if (pKeyBuffer[KeyCode::_2] & 0xF0) {
		m_pCamera->SetPosition(XMFLOAT3(0, 1, 3));
		m_pCamera->SetLookAt(XMFLOAT3(0, 1, 0));

	}
	if (pKeyBuffer[KeyCode::_3] & 0xF0) {
		m_pCamera->SetPosition(XMFLOAT3(0, 1, -3));
		m_pCamera->SetLookAt(XMFLOAT3(0, 1, 0));

	}
	if (pKeyBuffer[KeyCode::_4] & 0xF0) {
		m_pCamera->SetPosition(XMFLOAT3(3, 1, 3));
		m_pCamera->SetLookAt(XMFLOAT3(0, 1, 0));
	}
	if (pKeyBuffer[KeyCode::_5] & 0xF0) {
		m_pCamera->SetPosition(XMFLOAT3(3, 3, 3));
		m_pCamera->SetLookAt(XMFLOAT3(0, 0, 0));
	}
	if (pKeyBuffer[KeyCode::_6] & 0xF0) {
		m_pCamera->SetPosition(XMFLOAT3(3, 3, 3));
		m_pCamera->SetLookAt(XMFLOAT3(0, 0, 0));
	}

	if (pKeyBuffer[KeyCode::_N] & 0xF0) { test = true; }
	if (pKeyBuffer[KeyCode::_M] & 0xF0) { 
		//if (test) {
		//	ReloadLight();
		//	test = false; 
		//}
		test = false;
	}



}

void Scene::CreatePSO()
{
	PackGBufferPSO PackGBufferPso = PackGBufferPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["PackGBuffer"] = PackGBufferPso.GetPipelineState();

	AnimatedObjectPSO AnimatedObjectPso = AnimatedObjectPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["AnimatedObject"] = AnimatedObjectPso.GetPipelineState();

	RenderShadowPSO RenderShadowPso = RenderShadowPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["SpotLightShadow"] = RenderShadowPso.GetPipelineState();
	RenderSpotLightShadowAnimatedObjectPSO RenderSpotLightShadowAnimatedObjectPso = RenderSpotLightShadowAnimatedObjectPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["SpotLightShadowAnim"] = RenderSpotLightShadowAnimatedObjectPso.GetPipelineState();

	RenderPointLightShadowPSO RenderPointLightShadowPso = RenderPointLightShadowPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["PointLightShadow"] = RenderPointLightShadowPso.GetPipelineState();
	RenderPointLightShadowAnimatedObjectPSO RenderPointLightShadowAnimatedObjectPso = RenderPointLightShadowAnimatedObjectPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["PointLightShadowAnim"] = RenderPointLightShadowAnimatedObjectPso.GetPipelineState();

	RenderDirectionalShadowPSO RenderDirectionalShadowPso = RenderDirectionalShadowPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["DirectionalLightShadow"] = RenderDirectionalShadowPso.GetPipelineState();
	RenderDirectionalShadowAnimatedObjectPSO RenderDirectionalShadowAnimatedObjectPso = RenderDirectionalShadowAnimatedObjectPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["DirectionalLightShadowAnim"] = RenderDirectionalShadowAnimatedObjectPso.GetPipelineState();

	ColorFromGBufferPSO ColorFromGBufferPso = ColorFromGBufferPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["ColorFromGBuffer"] = ColorFromGBufferPso.GetPipelineState();

	AddLightPSO AddLightPso = AddLightPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["AddLight"] = AddLightPso.GetPipelineState();

	DebugColorPSO DebugColorPso = DebugColorPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["DebugColor"] = DebugColorPso.GetPipelineState();

	DebugDepthPSO DebugDepthPso = DebugDepthPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["DebugDepth"] = DebugDepthPso.GetPipelineState();
}
void Scene::UpdatePassInfoAboutCamera()
{
	XMFLOAT4X4 xmf4x4Temp;

	xmf4x4Temp = m_pCamera->GetViewMatrix();
	XMStoreFloat4x4(&m_pcbMappedPassInfo->m_xmf4x4CameraView,			XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));
	xmf4x4Temp = Matrix4x4::Inverse(xmf4x4Temp);
	XMStoreFloat4x4(&m_pcbMappedPassInfo->m_xmf4x4CameraViewInv,		XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));

	xmf4x4Temp = m_pCamera->GetProjectionMatrix();
	XMStoreFloat4x4(&m_pcbMappedPassInfo->m_xmf4x4CameraProjection,		XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));
	xmf4x4Temp = Matrix4x4::Inverse(xmf4x4Temp);
	XMStoreFloat4x4(&m_pcbMappedPassInfo->m_xmf4x4CameraProjectionInv,	XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));

	::memcpy(&m_pcbMappedPassInfo->m_xmf3CameraPosition, &m_pCamera->GetPosition(), sizeof(XMFLOAT3));


}
void Scene::CreatePassInfoShaderResource()
{
	UINT ncbElementBytes = ((sizeof(CB_PASS_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbPassInfo = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);


	D3D12_GPU_VIRTUAL_ADDRESS		d3dGpuVirtualAddress;
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;

	if (nullptr != m_pd3dcbPassInfo) {
		m_pd3dcbPassInfo->Map(0, NULL, (void**)&m_pcbMappedPassInfo);
		d3dGpuVirtualAddress = m_pd3dcbPassInfo->GetGPUVirtualAddress();
		d3dCBVDesc.SizeInBytes = ncbElementBytes;
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress;
		m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_d3dCbvCPUDescriptorStartHandle);

		m_d3dCbvCPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
	}

	m_d3dCbvGPUPassInfoHandle = m_d3dCbvGPUDescriptorStartHandle;
	m_d3dCbvGPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;

}
void Scene::ReloadLight()
{
	LightDataImporter lightDataImporter;
	vector<LIGHT_DESC> vecLightDesc = lightDataImporter.Load("Data/LightData.txt");
	string shadow("ShadowMap_");

	m_LightMng->DeleteAll();

	for (int i = 0; i < vecLightDesc.size(); i++) {
		switch (vecLightDesc[i].lightType)
		{
		case LightType::LIGHT_POINT:		m_LightMng->AddPointLight(vecLightDesc[i], m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);		break;
		case LightType::LIGHT_DIRECTIONAL:	m_LightMng->AddDirectionalLight(vecLightDesc[i], m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);	break;
		case LightType::LIGHT_SPOT:			m_LightMng->AddSpotLight(vecLightDesc[i], m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);			break;
		case LightType::LIGHT_NONE:
		default:
			break;
		}

		if (vecLightDesc[i].bIsShadow) {
			string temp = to_string(i);
			temp = shadow + temp;

			gTextureMng.DeleteTexture(temp.c_str());

			gTextureMng.AddDepthBufferTexture(temp.c_str(), m_pd3dDevice, SHADOWMAPSIZE, SHADOWMAPSIZE, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
			m_LightMng->SetShadowMapName(temp.c_str(), i);
		}
	}
}
void Scene::CreateDescriptorHeap() 
{
	// heap 생성을 하려면 미리 서술자를 몇 개나 생성할 지 알아야 하기 때문에
	// obj 개수도 중요하지만 srv에 들어갈 texture 개수도 중요함..
	// 미리 몇 개 만들지 알 수 있으면 그걸 정해두고 힙 생성을 하겠는데
	// 만약 동적으로 리소스 로드&언로드를 한다면?
	// 그럴 경우에는 힙을 미리 한도를 걸어두고 만드는 것도 방법일 수 있음
	// (예를 들어, obj는 몇 개, 투사체는 몇 개, 텍스쳐는 몇 개 제한을 둔다던지)
	// 좋다, 제한을 두자. 오브젝트와 투사체, 레벨 등을 다 합쳐서 1,024개 정도면 될 듯.
	// 텍스쳐도 128개 정도면 충분할 것.
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = MAXNUMCBV + MAXNUMSRV;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	HRESULT result = m_pd3dDevice->CreateDescriptorHeap(
		&d3dDescriptorHeapDesc,
		__uuidof(ID3D12DescriptorHeap),
		(void**)&m_pd3dCbvSrvDescriptorHeap);
	HRESULT reason = m_pd3dDevice->GetDeviceRemovedReason();

	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * MAXNUMCBV);
	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * MAXNUMCBV);

}

void Scene::Release()
{
	if (m_pd3dcbPassInfo)
	{
		m_pd3dcbPassInfo->Unmap(0, NULL);
		m_pd3dcbPassInfo->Release();
	}
}
