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

#include "Components.h"

bool TEST_MOUSE_USABLE = false;

void Scene::Init(Framework* pFramework, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	/*========================================================================
	* 주요 변수 초기화
	*=======================================================================*/
	m_pd3dDevice		= pd3dDevice;
	m_pd3dCommandList	= pd3dCommandList;
	m_pd3dRootSignature = CreateRootSignature();

	/*========================================================================
	* 커서 설정
	*=======================================================================*/
	if(TEST_MOUSE_USABLE)
		SetCursorPos(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 2);	


	/*========================================================================
	* 카메라 설정
	*=======================================================================*/
	//m_pCamera = new FollowCamera();
	//m_pCamera->SetPosition(XMFLOAT3(0, 2, -3));
	//m_pCamera->SetLookAt(XMFLOAT3(0, 1, 1));

	/*========================================================================
	* 디스크립터 힙 생성
	*=======================================================================*/
	CreateDescriptorHeap();


	/*==============================================================================
	* PassConstants 생성
	*=============================================================================*/
	CreatePassInfoShaderResource();
	{
		int totalBckBufPixels 
			= FRAME_BUFFER_WIDTH * 2;
		//= (FRAME_BUFFER_WIDTH * FRAME_BUFFER_WIDTH) / (16 * 1024);

		HRESULT hr = E_FAIL;



		//	후처리 텍스처는 SRV와 UAV 둘 다 만들어줘야 함.
		D3D12_HEAP_PROPERTIES hp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(totalBckBufPixels * sizeof(float),
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		hr = pd3dDevice->CreateCommittedResource(
			&hp,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&m_pd3duabHDRAvgLum));

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format						= DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension				= D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.NumElements			= totalBckBufPixels * sizeof(float);
		uavDesc.Buffer.StructureByteStride	= 1;
		uavDesc.Buffer.Flags				= D3D12_BUFFER_UAV_FLAG_NONE;

		m_pd3dDevice->CreateUnorderedAccessView(m_pd3duabHDRAvgLum, nullptr,
			&uavDesc, m_d3dSrvCPUDescriptorStartHandle);
		m_d3dSrvCPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
		m_d3dCbvGPUuabHDRAvgLumHandle = m_d3dSrvGPUDescriptorStartHandle;
		m_d3dSrvGPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;	// 여기까지 됨
	}



	/*========================================================================
	* 텍스쳐
	*=======================================================================*/
	g_TextureMng.Initialize(m_pd3dDevice);
	g_TextureMng.AddUnorderedAccessTexture("DownScaled", m_pd3dDevice, FRAME_BUFFER_WIDTH / 4, FRAME_BUFFER_HEIGHT / 4, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	g_TextureMng.AddUnorderedAccessTexture("Blur_Vertical", m_pd3dDevice, FRAME_BUFFER_WIDTH/4, FRAME_BUFFER_HEIGHT/4, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	g_TextureMng.AddUnorderedAccessTexture("Blur_Horizontal", m_pd3dDevice, FRAME_BUFFER_WIDTH/4, FRAME_BUFFER_HEIGHT/4, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	g_TextureMng.AddDepthBufferTexture("GBuffer_Depth", m_pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	g_TextureMng.AddRenderTargetTexture("GBuffer_Color", m_pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	g_TextureMng.AddRenderTargetTexture("GBuffer_Normal", m_pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	g_TextureMng.AddRenderTargetTexture("Screen", m_pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	//g_TextureMng.AddRenderTargetTexture("SmallScreen", m_pd3dDevice, FRAME_BUFFER_WIDTH/2, FRAME_BUFFER_HEIGHT/2, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);

	g_TextureMng.LoadFromFile("defaultDiffuseMap", m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	g_TextureMng.LoadFromFile("defaultNormalMap", m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	g_TextureMng.LoadFromFile("defaultSpecularMap", m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);

	/*========================================================================
	* 마테리얼
	*=======================================================================*/
	MaterialDataImporter matDataImporter;
	matDataImporter.Load("Data/MaterialData.txt");


	/*========================================================================
	* 모델
	*=======================================================================*/
	g_ModelMng.Initialize();


	/*========================================================================
	* 에셋 리스트 로드해서 모델이랑 텍스처를 동시에 로드
	*=======================================================================*/
	AssetListDataImporter assetImporter;
	assetImporter.Load(m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);

	/*========================================================================
	* Pass 1 전용 오브젝트 데이터 로드 및 생성
	*=======================================================================*/

	BuildObject();

	/*========================================================================
	* Pass 2 전용 전체 화면 사각형
	*=======================================================================*/
	Screen* tempScreen
		= new Screen(m_pd3dDevice, m_pd3dCommandList,
			m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle,
			1.0f, 1.0f);
	m_vecScreenObject.push_back(tempScreen);

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
				g_TextureMng.AddDepthBufferTextureCube(temp.c_str(), m_pd3dDevice, SHADOWMAPSIZE, SHADOWMAPSIZE, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);

				break;

			case LightType::LIGHT_SPOT:
				g_TextureMng.AddDepthBufferTexture(temp.c_str(), m_pd3dDevice, SHADOWMAPSIZE, SHADOWMAPSIZE, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);

				break;
			case LightType::LIGHT_DIRECTIONAL:
				g_TextureMng.AddDepthBufferTextureArray(temp.c_str(), 3, m_pd3dDevice, SHADOWMAPSIZE, SHADOWMAPSIZE, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);


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

void Scene::CheckCollsion()
{
	for (int i = 0; i < m_vecObject.size(); i++) {
		for (int j = i + 1; j < m_vecObject.size(); j++) {
			m_vecObject[i]->CheckCollision(m_vecObject[j]);
		}
	}
}

void Scene::SolveConstraint()
{
	for_each(m_vecObject.begin(), m_vecObject.end(), [](Object* o) {o->SolveConstraint(); });
}

void Scene::Input(UCHAR* pKeyBuffer)
{
	POINT ptCursorPos;
	GetCursorPos(&ptCursorPos);
	XMFLOAT2 xmf2MouseMovement;
	if (TEST_MOUSE_USABLE) {
		xmf2MouseMovement.x = (float)(ptCursorPos.x - FRAME_BUFFER_WIDTH / 2);
		xmf2MouseMovement.y = (float)(ptCursorPos.y - FRAME_BUFFER_HEIGHT / 2);
		SetCursorPos(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 2);
	}
	else {
		xmf2MouseMovement.x = 0;
		xmf2MouseMovement.y = 0;
	}

	for_each(m_vecObject.begin(), m_vecObject.end(), [&](Object* o) {o->Input(pKeyBuffer, xmf2MouseMovement); });

	if (pKeyBuffer[KeyCode::_M] & 0xF0) { TEST_MOUSE_USABLE = true; ShowCursor(false); }
	if (pKeyBuffer[KeyCode::_N] & 0xF0) { TEST_MOUSE_USABLE = false; ShowCursor(true); }
}
void Scene::Update(float fTimeElapsed)
{
	m_fCurrentTime += fTimeElapsed;
	::memcpy(&m_pcbMappedPassInfo->m_xmfCurrentTime, &m_fCurrentTime, sizeof(float));

	CheckCollsion();
	SolveConstraint();

	for_each(m_vecObject.begin(), m_vecObject.end(), [fTimeElapsed](Object* o) {o->Update(fTimeElapsed); });
}
void Scene::Render(D3D12_CPU_DESCRIPTOR_HANDLE hBckBufRtv, D3D12_CPU_DESCRIPTOR_HANDLE hBckBufDsv)
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
	CameraComponent* cam = m_pCameraObject->FindComponent<CameraComponent>();
	cam->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	cam->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	cam->SetViewportsAndScissorRects(m_pd3dCommandList);
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
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER) * 3);
	d3dResourceBarrier[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier[0].Transition.pResource = g_TextureMng.GetTextureResource("GBuffer_Depth");
	d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
	d3dResourceBarrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	d3dResourceBarrier[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	d3dResourceBarrier[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier[1].Transition.pResource = g_TextureMng.GetTextureResource("GBuffer_Color");
	d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	d3dResourceBarrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	d3dResourceBarrier[2].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier[2].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier[2].Transition.pResource = g_TextureMng.GetTextureResource("GBuffer_Normal");
	d3dResourceBarrier[2].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	d3dResourceBarrier[2].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier[2].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(3, d3dResourceBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = g_TextureMng.GetDsvCPUHandle("GBuffer_Depth");
	m_pd3dCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[2];
	rtvHandle[0] = g_TextureMng.GetRtvCPUHandle("GBuffer_Color");
	rtvHandle[1] = g_TextureMng.GetRtvCPUHandle("GBuffer_Normal");

	float pfClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_pd3dCommandList->ClearRenderTargetView(rtvHandle[0], pfClearColor, 0, NULL);
	m_pd3dCommandList->ClearRenderTargetView(rtvHandle[1], pfClearColor, 0, NULL);

	m_pd3dCommandList->OMSetRenderTargets(2, rtvHandle, FALSE, &dsvHandle);

	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["AnimatedObject"]);
	for (auto iter = m_vecAnimObjectRenderGroup.begin(); iter != m_vecAnimObjectRenderGroup.end(); iter++) (*iter)->Render(m_pd3dCommandList);
	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["PackGBuffer"]);
	for (auto iter = m_vecNonAnimObjectRenderGroup.begin(); iter != m_vecNonAnimObjectRenderGroup.end(); iter++) (*iter)->Render(m_pd3dCommandList);



	d3dResourceBarrier[0].Transition.pResource = g_TextureMng.GetTextureResource("GBuffer_Depth");
	d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	d3dResourceBarrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	d3dResourceBarrier[1].Transition.pResource = g_TextureMng.GetTextureResource("GBuffer_Color");
	d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	d3dResourceBarrier[2].Transition.pResource = g_TextureMng.GetTextureResource("GBuffer_Normal");
	d3dResourceBarrier[2].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier[2].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	m_pd3dCommandList->ResourceBarrier(3, d3dResourceBarrier);

	/*========================================================================
	* Pass 1. 광원별 그림자맵 렌더
	*=======================================================================*/
	cam->SetViewport(0, 0, SHADOWMAPSIZE, SHADOWMAPSIZE, 0.0f, 1.0f);
	cam->SetScissorRect(0, 0, SHADOWMAPSIZE, SHADOWMAPSIZE);
	cam->SetViewportsAndScissorRects(m_pd3dCommandList);

	for (UINT i = 0; i < m_LightMng->GetNumLight(); i++) {
		if (m_LightMng->GetIsShadow(i)) {
			m_LightMng->SetShaderResource(m_pd3dCommandList, i);

			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = g_TextureMng.GetDsvCPUHandle(m_LightMng->GetShadowMapName(i).c_str());
			m_pd3dCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
			m_pd3dCommandList->OMSetRenderTargets(0, NULL, TRUE, &dsvHandle);

			switch (m_LightMng->GetLightType(i))
			{
			case LightType::LIGHT_SPOT:

				m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["SpotLightShadow"]);
				for (int i = 0; i < m_vecNonAnimObjectRenderGroup.size(); i++) m_vecNonAnimObjectRenderGroup[i]->Render(m_pd3dCommandList);

				m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["SpotLightShadowAnim"]);
				for (int i = 0; i < m_vecAnimObjectRenderGroup.size(); i++) m_vecAnimObjectRenderGroup[i]->Render(m_pd3dCommandList);
				break;

			case LightType::LIGHT_POINT:

				m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["PointLightShadow"]);
				for (int i = 0; i < m_vecNonAnimObjectRenderGroup.size(); i++) m_vecNonAnimObjectRenderGroup[i]->Render(m_pd3dCommandList);

				m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["PointLightShadowAnim"]);
				for (int i = 0; i < m_vecAnimObjectRenderGroup.size(); i++) m_vecAnimObjectRenderGroup[i]->Render(m_pd3dCommandList);
				break;

			case LightType::LIGHT_DIRECTIONAL:

				m_LightMng->UpdateDirectionalLightOrthographicLH(cam->GetViewMatrix(), i);

				m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["DirectionalLightShadow"]);
				for (int i = 0; i < m_vecNonAnimObjectRenderGroup.size(); i++) m_vecNonAnimObjectRenderGroup[i]->Render(m_pd3dCommandList);

				m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["DirectionalLightShadowAnim"]);
				for (int i = 0; i < m_vecAnimObjectRenderGroup.size(); i++) m_vecAnimObjectRenderGroup[i]->Render(m_pd3dCommandList);
				break;

			case LightType::LIGHT_NONE:
			default:
				break;
			}

		}
	}
	/*
	OMSet(bckBuf);
	*/
	D3D12_CPU_DESCRIPTOR_HANDLE screenRtv = g_TextureMng.GetRtvCPUHandle("Screen");
	m_pd3dCommandList->ClearRenderTargetView(screenRtv, pfClearColor, 0, NULL);
	m_pd3dCommandList->OMSetRenderTargets(1, &screenRtv, TRUE, &dsvHandle);

	//m_pd3dCommandList->ClearDepthStencilView(hBckBufDsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	//m_pd3dCommandList->OMSetRenderTargets(1, &hBckBufRtv, TRUE, &hBckBufDsv);

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
	cam->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	cam->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	cam->SetViewportsAndScissorRects(m_pd3dCommandList);



	/*========================================================================
	* Pass 2. 스크린 렌더
	*=======================================================================*/
	//m_pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature);

	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["ColorFromGBuffer"]);
	g_TextureMng.UseForShaderResource("GBuffer_Normal", m_pd3dCommandList, ROOTSIGNATURE_NORMAL_TEXTURE);
	g_TextureMng.UseForShaderResource("GBuffer_Depth", m_pd3dCommandList, ROOTSIGNATURE_DEPTH_TEXTURE);
	g_TextureMng.UseForShaderResource("GBuffer_Color", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
	m_vecScreenObject[0]->Render(m_pd3dCommandList);


	/*========================================================================
	* Pass 2. 파티클 & 이펙트 렌더?
	*=======================================================================*/
	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Effect"]);
	for (auto iter = m_vecEffectRenderGroup.begin(); iter != m_vecEffectRenderGroup.end(); iter++) (*iter)->Render(m_pd3dCommandList);
	m_pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Particle"]);
	for (auto iter = m_vecParticleEmitter.begin(); iter != m_vecParticleEmitter.end(); iter++) (*iter)->Render(m_pd3dCommandList);
	m_pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/*========================================================================
	* Pass 2. 광원별 렌더
	*=======================================================================*/
	g_TextureMng.UseForShaderResource("GBuffer_Normal", m_pd3dCommandList, ROOTSIGNATURE_NORMAL_TEXTURE);
	g_TextureMng.UseForShaderResource("GBuffer_Depth", m_pd3dCommandList, ROOTSIGNATURE_DEPTH_TEXTURE);
	g_TextureMng.UseForShaderResource("GBuffer_Color", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["AddLight"]);

	for (UINT i = 0; i < m_LightMng->GetNumLight(); i++) {
		m_LightMng->SetShaderResource(m_pd3dCommandList, i);

		if (m_LightMng->GetIsShadow(i)) {
			switch (m_LightMng->GetLightType(i))
			{
			case LightType::LIGHT_SPOT:			g_TextureMng.UseForShaderResource(m_LightMng->GetShadowMapName(i).c_str(), m_pd3dCommandList, ROOTSIGNATURE_SHADOW_TEXTURE);		break;
			case LightType::LIGHT_POINT:		g_TextureMng.UseForShaderResource(m_LightMng->GetShadowMapName(i).c_str(), m_pd3dCommandList, ROOTSIGNATURE_CUBE_TEXTURE);			break;
			case LightType::LIGHT_DIRECTIONAL:	g_TextureMng.UseForShaderResource(m_LightMng->GetShadowMapName(i).c_str(), m_pd3dCommandList, ROOTSIGNATURE_SHADOWARRAY_TEXTURE);	break;
			case LightType::LIGHT_NONE:
			default:
				break;
			}
		}

		m_vecScreenObject[0]->Render(m_pd3dCommandList);
	}

	/*========================================================================
	* Pass 2. Post Process
	*=======================================================================*/
	//bool bPostProcessing = true;
	//if (gTestInt == 1) {
	//	/*========================================================================
	//	* DownScaling
	//	*=======================================================================*/
	//	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["SRToRt"]);

	//	d3dResourceBarrier[0].Transition.pResource = g_TextureMng.GetTextureResource("Screen");
	//	d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//	d3dResourceBarrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//	d3dResourceBarrier[1].Transition.pResource = g_TextureMng.GetTextureResource("SmallScreen");
	//	d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//	d3dResourceBarrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//	m_pd3dCommandList->ResourceBarrier(2, d3dResourceBarrier);
	//	g_TextureMng.UseForShaderResource("Screen", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);

	//	screenRtv = g_TextureMng.GetRtvCPUHandle("SmallScreen");
	//	m_pd3dCommandList->ClearRenderTargetView(screenRtv, pfClearColor, 0, NULL);
	//	m_pd3dCommandList->OMSetRenderTargets(1, &screenRtv, TRUE, NULL);

	//	m_vecDebugWindow[1]->Render(m_pd3dCommandList);

	//	d3dResourceBarrier[0].Transition.pResource = g_TextureMng.GetTextureResource("Screen");
	//	d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//	d3dResourceBarrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//	d3dResourceBarrier[1].Transition.pResource = g_TextureMng.GetTextureResource("SmallScreen");
	//	d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//	d3dResourceBarrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	//	m_pd3dCommandList->ResourceBarrier(2, d3dResourceBarrier);

	//	/*========================================================================
	//	* Vertical Blue
	//	*=======================================================================*/
	//	m_pd3dCommandList->SetComputeRootSignature(m_pd3dRootSignature);

	//	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Blur_Vertical"]);

	//	g_TextureMng.UseForComputeShaderResourceSRV("SmallScreen", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
	//	g_TextureMng.UseForComputeShaderResourceUAV("Blur_Vertical", m_pd3dCommandList, ROOTSIGNATURE_POSTPROCESS_TEXTURE);

	//	UINT numGroups = (UINT)ceilf((FRAME_BUFFER_WIDTH / 2) / 256.0f);
	//	m_pd3dCommandList->Dispatch(numGroups, FRAME_BUFFER_HEIGHT / 2, 1);

	//	/*========================================================================
	//	* Horizontal Blur
	//	*=======================================================================*/
	//	d3dResourceBarrier[0].Transition.pResource = g_TextureMng.GetTextureResource("Blur_Horizontal");
	//	d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//	d3dResourceBarrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	//	d3dResourceBarrier[1].Transition.pResource = g_TextureMng.GetTextureResource("Blur_Vertical");
	//	d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	//	d3dResourceBarrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//	m_pd3dCommandList->ResourceBarrier(2, d3dResourceBarrier);

	//	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Blur_Horizontal"]);

	//	g_TextureMng.UseForComputeShaderResourceSRV("Blur_Vertical", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
	//	g_TextureMng.UseForComputeShaderResourceUAV("Blur_Horizontal", m_pd3dCommandList, ROOTSIGNATURE_POSTPROCESS_TEXTURE);

	//	numGroups = (UINT)ceilf((FRAME_BUFFER_HEIGHT / 2) / 256.0f);
	//	m_pd3dCommandList->Dispatch(FRAME_BUFFER_WIDTH / 2, numGroups, 1);

	//	d3dResourceBarrier[0].Transition.pResource = g_TextureMng.GetTextureResource("Blur_Horizontal");
	//	d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	//	d3dResourceBarrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//	d3dResourceBarrier[1].Transition.pResource = g_TextureMng.GetTextureResource("Blur_Vertical");
	//	d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//	d3dResourceBarrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	//	d3dResourceBarrier[2].Transition.pResource = g_TextureMng.GetTextureResource("SmallScreen");
	//	d3dResourceBarrier[2].Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
	//	d3dResourceBarrier[2].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//	m_pd3dCommandList->ResourceBarrier(3, d3dResourceBarrier);

	//	/*===========================================================================
	//	* Final Result
	//	*==========================================================================*/
	//	m_pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature);
	//	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["SRToRt"]);

	//	g_TextureMng.UseForShaderResource("Blur_Horizontal", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);

	//	m_pd3dCommandList->ClearDepthStencilView(hBckBufDsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	//	m_pd3dCommandList->OMSetRenderTargets(1, &hBckBufRtv, TRUE, &hBckBufDsv);
	//	m_vecDebugWindow[0]->Render(m_pd3dCommandList);
	//}
	//else 
	if (gTestInt == 2) {

		/*===========================================================================
		* Post Process List
		* - Calc AvgLum
		* - HDR Rendering to 'Screen' Texture, 
			DownScaled Rendering to 'DownScaled' Texture.
		* - Bloom Threshold Rendering to 'PostProcess_Temp1' Texture.
		* - Vertical Blur Bloom Threshold Rendering to 'PostProcess_Temp2' Texture.
		* - Horizontal to 'PostProcess_Temp1' Texture.
		* - 'PostProcess_Temp1' Add to 'Screen'
		*==========================================================================*/
		m_pd3dCommandList->SetComputeRootSignature(m_pd3dRootSignature);


		/*===========================================================================
		* Calc AvgLum and store to 'gfAvgLum' Buffer.
		* Render to 'DownScaled' Texture.
		*==========================================================================*/
		m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["HDR_First"]);


		d3dResourceBarrier[0].Transition.pResource = g_TextureMng.GetTextureResource("DownScaled");
		d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		d3dResourceBarrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		d3dResourceBarrier[1].Transition.pResource = g_TextureMng.GetTextureResource("Screen");
		d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		d3dResourceBarrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		m_pd3dCommandList->ResourceBarrier(2, d3dResourceBarrier);

		g_TextureMng.UseForComputeShaderResourceSRV(
			"Screen", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);					// Render Result
		g_TextureMng.UseForComputeShaderResourceUAV(
			"DownScaled", m_pd3dCommandList, ROOTSIGNATURE_POSTPROCESS_TEXTURE);		// DownScaled
		m_pd3dCommandList->SetComputeRootUnorderedAccessView(
			ROOTSIGNATURE_HDRLUMBUFFER, m_pd3duabHDRAvgLum->GetGPUVirtualAddress());	// AvgLum Buff

		UINT numGroups;
		//UINT numGroups = (UINT)ceilf(FRAME_BUFFER_WIDTH / 1024.0f);
		m_pd3dCommandList->Dispatch(1, FRAME_BUFFER_HEIGHT / 4, 1);
		/*
		이 부분은 모르는 상태에서 보면 정말 어렵다.
		따라서 메모를 적어둔다.

		한 스레드 그룹은 [numthreads(1024, 1, 1,)]로 되어있다.
		CS_DownScaleFirstPass()는 x축 방향으로 1024개씩 접근하고,
		가로세로 1/4까지만 읽으면 되기 때문에 1920 / 4, 1080 / 4 인 480, 270
		480은 1024 한 번에 끝.
		270은 1024, 1, 1이니까 270번

		이렇게 하면 gfAvgLum[270]까지 내용이 찬다.
		*/

		/*===========================================================================
		* Calc FinalAvgLum and store to 'gfAvgLum' Buffer's [0].
		*==========================================================================*/
		m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["HDR_Second"]);

		m_pd3dCommandList->Dispatch(1, 1, 1);
		/*
		270까지 찬 내용을 다운스케일 해서 하나로 만들고 그걸 gfAvgLum[0]에 넣어줌.
		*/

		/*===========================================================================
		* Bloom Rendering to 'Blur_Horizontal' Texture.
		*==========================================================================*/
		m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["PP_Bloom"]);

		d3dResourceBarrier[0].Transition.pResource = g_TextureMng.GetTextureResource("DownScaled");
		d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		d3dResourceBarrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		d3dResourceBarrier[1].Transition.pResource = g_TextureMng.GetTextureResource("Blur_Horizontal");
		d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE ;
		d3dResourceBarrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		m_pd3dCommandList->ResourceBarrier(2, d3dResourceBarrier);

		g_TextureMng.UseForComputeShaderResourceSRV(
			"DownScaled", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
		g_TextureMng.UseForComputeShaderResourceUAV(
			"Blur_Horizontal", m_pd3dCommandList, ROOTSIGNATURE_POSTPROCESS_TEXTURE);

		numGroups = (UINT)ceilf((FRAME_BUFFER_WIDTH / 4) / 1024.0f);
		m_pd3dCommandList->Dispatch(numGroups, FRAME_BUFFER_HEIGHT / 4, 1);


		/*===========================================================================
		* Bloom Blur Vertical
		*==========================================================================*/
		m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Blur_Vertical"]);

		d3dResourceBarrier[0].Transition.pResource = g_TextureMng.GetTextureResource("Blur_Horizontal");
		d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		d3dResourceBarrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		d3dResourceBarrier[1].Transition.pResource = g_TextureMng.GetTextureResource("Blur_Vertical");
		d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		d3dResourceBarrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		m_pd3dCommandList->ResourceBarrier(2, d3dResourceBarrier);

		g_TextureMng.UseForComputeShaderResourceSRV(
			"Blur_Horizontal", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
		g_TextureMng.UseForComputeShaderResourceUAV(
			"Blur_Vertical", m_pd3dCommandList, ROOTSIGNATURE_POSTPROCESS_TEXTURE);

		numGroups = (UINT)ceilf((FRAME_BUFFER_WIDTH / 4) / 256.0f);
		m_pd3dCommandList->Dispatch(numGroups, FRAME_BUFFER_HEIGHT / 4, 1);

		/*===========================================================================
		* Bloom Blur Horizontal
		*==========================================================================*/
		m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Blur_Horizontal"]);

		d3dResourceBarrier[0].Transition.pResource = g_TextureMng.GetTextureResource("Blur_Vertical");
		d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		d3dResourceBarrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		d3dResourceBarrier[1].Transition.pResource = g_TextureMng.GetTextureResource("Blur_Horizontal");
		d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		d3dResourceBarrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		m_pd3dCommandList->ResourceBarrier(2, d3dResourceBarrier);

		g_TextureMng.UseForComputeShaderResourceSRV(
			"Blur_Vertical", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
		g_TextureMng.UseForComputeShaderResourceUAV(
			"Blur_Horizontal", m_pd3dCommandList, ROOTSIGNATURE_POSTPROCESS_TEXTURE);

		numGroups = (UINT)ceilf((FRAME_BUFFER_HEIGHT / 4) / 256.0f);
		m_pd3dCommandList->Dispatch(FRAME_BUFFER_WIDTH / 4, numGroups, 1);

		/*===========================================================================
		* Screen Render
		*==========================================================================*/
		m_pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature);
		m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["HDR_Last"]);

		d3dResourceBarrier[0].Transition.pResource = g_TextureMng.GetTextureResource("Blur_Horizontal");
		d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		d3dResourceBarrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE ;
		m_pd3dCommandList->ResourceBarrier(1, d3dResourceBarrier);

		g_TextureMng.UseForShaderResource(
			"Screen", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
		g_TextureMng.UseForShaderResource(
			"Blur_Horizontal", m_pd3dCommandList, ROOTSIGNATURE_NORMAL_TEXTURE);
		m_pd3dCommandList->SetGraphicsRootUnorderedAccessView(
			ROOTSIGNATURE_HDRLUMBUFFER, m_pd3duabHDRAvgLum->GetGPUVirtualAddress());

		m_pd3dCommandList->ClearDepthStencilView(hBckBufDsv, 
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
		m_pd3dCommandList->OMSetRenderTargets(1, &hBckBufRtv, TRUE, &hBckBufDsv);
		m_vecScreenObject[0]->Render(m_pd3dCommandList);

		d3dResourceBarrier[0].Transition.pResource = g_TextureMng.GetTextureResource("Screen");
		d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE ;
		d3dResourceBarrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		m_pd3dCommandList->ResourceBarrier(1, d3dResourceBarrier);
	}
	else {
		m_pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature);
		m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["SRToRt"]);

		g_TextureMng.UseForShaderResource("Screen", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);

		m_pd3dCommandList->ClearDepthStencilView(hBckBufDsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
		m_pd3dCommandList->OMSetRenderTargets(1, &hBckBufRtv, TRUE, &hBckBufDsv);
		m_vecScreenObject[0]->Render(m_pd3dCommandList);
	}

}

void Scene::Release()
{
	if (m_pd3dcbPassInfo)
	{
		m_pd3dcbPassInfo->Unmap(0, NULL);
		m_pd3dcbPassInfo->Release();
	}
}

ID3D12RootSignature* Scene::CreateRootSignature()
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;
	D3D12_DESCRIPTOR_RANGE d3dDescriptorRange[10];

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

	d3dDescriptorRange[9].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	d3dDescriptorRange[9].NumDescriptors = 1;
	d3dDescriptorRange[9].BaseShaderRegister = ROOTSIGNATURE_POSTPROCESS_TEXTURE;
	d3dDescriptorRange[9].RegisterSpace = 0;
	d3dDescriptorRange[9].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[12];

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

	pd3dRootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[10].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[10].DescriptorTable.pDescriptorRanges = &d3dDescriptorRange[9];
	pd3dRootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
	pd3dRootParameters[11].Descriptor.ShaderRegister = ROOTSIGNATURE_HDRLUMBUFFER;
	pd3dRootParameters[11].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

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

	EffectPSO EffectPso = EffectPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["Effect"] = EffectPso.GetPipelineState();

	ParticlePSO ParticlePso = ParticlePSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["Particle"] = ParticlePso.GetPipelineState();

	//DebugColorPSO DebugColorPso = DebugColorPSO(m_pd3dDevice, m_pd3dRootSignature);
	//m_uomPipelineStates["DebugColor"] = DebugColorPso.GetPipelineState();

	//DebugDepthPSO DebugDepthPso = DebugDepthPSO(m_pd3dDevice, m_pd3dRootSignature);
	//m_uomPipelineStates["DebugDepth"] = DebugDepthPso.GetPipelineState();


	/*============================================================================
	* Blur
	============================================================================*/
	VerticalBlurCPSO VerticalBlurPso = VerticalBlurCPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["Blur_Vertical"] = VerticalBlurPso.GetPipelineState();
	HorizontalBlurCPSO HorizontalBlurPso = HorizontalBlurCPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["Blur_Horizontal"] = HorizontalBlurPso.GetPipelineState();
	SRToRtPSO SRToRtPso = SRToRtPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["SRToRt"] = SRToRtPso.GetPipelineState();


	/*============================================================================
	* HDR
	============================================================================*/
	HDRFstPassCPSO HDRFstPassCpso = HDRFstPassCPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["HDR_First"] = HDRFstPassCpso.GetPipelineState();
	HDRScdPassCPSO HDRScdPassCpso = HDRScdPassCPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["HDR_Second"] = HDRScdPassCpso.GetPipelineState();
	HDRToneMappingPSO HDRToneMappingPso = HDRToneMappingPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["HDR_Last"] = HDRToneMappingPso.GetPipelineState();

	/*============================================================================
	* Bloom
	============================================================================*/
	BloomCPSO BloomCpso = BloomCPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["PP_Bloom"] = BloomCpso.GetPipelineState();


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
void Scene::UpdatePassInfoAboutCamera()
{
	CameraComponent* cam = m_pCameraObject->FindComponent<CameraComponent>();
	XMFLOAT4X4 xmf4x4Temp;

	//xmf4x4Temp = m_pCamera->GetViewMatrix();
	xmf4x4Temp = cam->GetViewMatrix();
	XMStoreFloat4x4(&m_pcbMappedPassInfo->m_xmf4x4CameraView,			XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));
	xmf4x4Temp = Matrix4x4::Inverse(xmf4x4Temp);
	XMStoreFloat4x4(&m_pcbMappedPassInfo->m_xmf4x4CameraViewInv,		XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));

	//xmf4x4Temp = m_pCamera->GetProjectionMatrix();
	xmf4x4Temp = cam->GetProjectionMatrix();
	XMStoreFloat4x4(&m_pcbMappedPassInfo->m_xmf4x4CameraProjection,		XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));
	xmf4x4Temp = Matrix4x4::Inverse(xmf4x4Temp);
	XMStoreFloat4x4(&m_pcbMappedPassInfo->m_xmf4x4CameraProjectionInv,	XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));

	//::memcpy(&m_pcbMappedPassInfo->m_xmf3CameraPosition, &m_pCamera->GetPosition(), sizeof(XMFLOAT3));
	XMFLOAT3 xmf3WorldPos = m_pCameraObject->FindComponent<TransformComponent>()->GetPosition(Space::world);
	::memcpy(&m_pcbMappedPassInfo->m_xmf3CameraPosition, &xmf3WorldPos, sizeof(XMFLOAT3));
}

#define MAX_PARTICLE_NUM 1000

void Scene::BuildObject()
{
	// Particle Pool Initialize
	for (int i = 0; i < MAX_PARTICLE_NUM; i++) {
		Object* ptc = new Object();

		TransformComponent* t = new TransformComponent(ptc);
		ParticleComponent* pc = new ParticleComponent(ptc, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

		ptc->SetActive(false);

		m_vecParticlePool.push_back(ptc);
	}
	{
		Object* pe = new Object("particleEmitter");
		
		TransformComponent* t = new TransformComponent(pe);
		ParticleEmitterComponent* pec = new ParticleEmitterComponent(pe, &m_vecParticlePool);

		t->Translate(3, 2, -1);

		pec->SetMaterialByName("ParticleTestMat");

		m_vecObject.push_back(pe);
		m_vecParticleEmitter.push_back(pe);
	}

	{
		// muzzle, empty object for weapon
		Object* muzzle = new Object("muzzle");
		TransformComponent* mTransform = new TransformComponent(muzzle);
		EffectComponent* effect = new EffectComponent(muzzle);
		MeshRendererComponent* mrcm = new MeshRendererComponent(muzzle, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

		mrcm->SetActive(false);
		mrcm->SetModelByName("muzzleFlash");
		mrcm->SetMaterialByName("MuzzleFlashMat");

		effect->SetDuration(0.05f);

		mTransform->Translate(0, 0.07f, 0.15f);
		m_vecObject.push_back(muzzle);
		m_vecEffectRenderGroup.push_back(muzzle);

		// weapon
		Object* weapon = new Object("pistol");

		TransformComponent* wTransform = new TransformComponent(weapon);
		WeaponControllerComponent* wcc = new WeaponControllerComponent(weapon, muzzle, nullptr);
		MeshRendererComponent* mrc = new MeshRendererComponent(weapon, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

		mrc->SetModelByName("pistol");
		mrc->SetMaterialByName("PistolMat");

		m_vecObject.push_back(weapon);
		m_vecNonAnimObjectRenderGroup.push_back(weapon);
		muzzle->m_pParent = weapon;
	}
	{
		// player
		Object* player = new Object("player");

		TransformComponent* transform = new TransformComponent(player);
		RigidbodyComponent* rigidbody = new RigidbodyComponent(player);
		SkinnedMeshRendererComponent* skinnedMeshRenderer = new SkinnedMeshRendererComponent(player, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		HumanoidControllerComponent* humanoidController = new HumanoidControllerComponent(player, m_vecNonAnimObjectRenderGroup[0]);
		HumanoidAnimatorComponent* humanoidAnimator = new HumanoidAnimatorComponent(player, "Humanoid_Idle");
		InputManagerComponent* controller = new InputManagerComponent(player);
		SphereColliderComponent* sphereCollider = new SphereColliderComponent(player, XMFLOAT3(0, 0.5f, 0), 0.5f);

		skinnedMeshRenderer->SetModelByName("human");
		skinnedMeshRenderer->SetMaterialByName("DefaultMaterial");
		transform->Translate(0, 0, -3);


		m_vecObject.push_back(player);
		m_vecAnimObjectRenderGroup.push_back(player);
		FindObjectByName("pistol")->m_pParent = player;
	}
	{
		Object* head = new Object("head");

		TransformComponent* transform = new TransformComponent(head);
		transform->Translate(0, 1.2f, 0.7f);
		m_vecObject.push_back(head);

		head->m_pParent = FindObjectByName("player");
	}
	
	{
		Object* box = new Object();

		TransformComponent* transform = new TransformComponent(box);
		MeshRendererComponent* mrc = new MeshRendererComponent(box, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		BoxColliderComponent* boxCollider = new BoxColliderComponent(box, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT4(0, 0, 0, 1));

		mrc->SetModelByName("1x1Box_20220104");
		mrc->SetMaterialByName("BoxMat");
		transform->Translate(0, 0, 3);

		m_vecObject.push_back(box);
		m_vecNonAnimObjectRenderGroup.push_back(box);
	}
	{
		Object* box = new Object();

		TransformComponent* transform = new TransformComponent(box);
		MeshRendererComponent* mrc = new MeshRendererComponent(box, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		BoxColliderComponent* boxCollider = new BoxColliderComponent(box, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT4(0, 0, 0, 1));

		mrc->SetModelByName("1x1Box_20220104");
		mrc->SetMaterialByName("BoxMat");
		transform->Translate(-2, 0, 2);

		m_vecObject.push_back(box);
		m_vecNonAnimObjectRenderGroup.push_back(box);
	}
	{
		Object* box = new Object();

		TransformComponent* transform = new TransformComponent(box);
		MeshRendererComponent* mrc = new MeshRendererComponent(box, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		BoxColliderComponent* boxCollider = new BoxColliderComponent(box, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT4(0, 0, 0, 1));

		mrc->SetModelByName("1x1Box_20220104");
		mrc->SetMaterialByName("BoxMat");
		transform->Translate(1, 0, 2);

		m_vecObject.push_back(box);
		m_vecNonAnimObjectRenderGroup.push_back(box);
	}
	{
		Object* box = new Object();

		TransformComponent* transform = new TransformComponent(box);
		MeshRendererComponent* mrc = new MeshRendererComponent(box, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

		mrc->SetModelByName("1x1Box_20220104");
		mrc->SetMaterialByName("BoxMat");
		transform->Translate(1, 1.8f, 2);

		m_vecObject.push_back(box);
		m_vecNonAnimObjectRenderGroup.push_back(box);
	}

	{
		Object* box = new Object();

		TransformComponent* transform = new TransformComponent(box);
		MeshRendererComponent* mrc = new MeshRendererComponent(box, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

		mrc->SetModelByName("1x1Box_20220104");
		mrc->SetMaterialByName("BoxMat");
		transform->Translate(2.7f, 1.8f, -3.0f);

		m_vecObject.push_back(box);
		m_vecNonAnimObjectRenderGroup.push_back(box);
	}
	{
		Object* room = new Object();

		TransformComponent* transform = new TransformComponent(room);
		MeshRendererComponent* mrc0 = new MeshRendererComponent(room, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		MeshRendererComponent* mrc1 = new MeshRendererComponent(room, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		// 0: front, 1: left, 2: back, 3: right
		BoxColliderComponent* boxCollider0 = new BoxColliderComponent(room, XMFLOAT3(0, 0.5f, 5.0f),	XMFLOAT3(5.0f, 1.0f, 0.5f), XMFLOAT4(0, 0, 0, 1));
		BoxColliderComponent* boxCollider1 = new BoxColliderComponent(room, XMFLOAT3(5.0f, 0.5f, 0),	XMFLOAT3(0.5f, 1.0f, 5.0f), XMFLOAT4(0, 0, 0, 1));
		BoxColliderComponent* boxCollider2 = new BoxColliderComponent(room, XMFLOAT3(0, 0.5f, -5.0f),	XMFLOAT3(5.0f, 1.0f, 0.5f), XMFLOAT4(0, 0, 0, 1));
		BoxColliderComponent* boxCollider3 = new BoxColliderComponent(room, XMFLOAT3(-5.0f, 0.5f, 0),	XMFLOAT3(0.5f, 1.0f, 5.0f), XMFLOAT4(0, 0, 0, 1));

		mrc0->SetModelByName("10x10Floor_20220104");
		mrc0->SetMaterialByName("CobblestoneMat");
		mrc1->SetModelByName("10x1wall_around_20220104");
		mrc1->SetMaterialByName("WallBottomMat");

		m_vecObject.push_back(room);
		m_vecNonAnimObjectRenderGroup.push_back(room);
	}
	{
		Object* room = new Object();

		TransformComponent* transform = new TransformComponent(room);
		MeshRendererComponent* mrc = new MeshRendererComponent(room, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

		mrc->SetModelByName("10x1wall_around_20220104");
		mrc->SetMaterialByName("WallTopMat");
		transform->Translate(0, 1, 0);

		m_vecObject.push_back(room);
		m_vecNonAnimObjectRenderGroup.push_back(room);
	}

	{
		Object* room = new Object();

		TransformComponent* transform = new TransformComponent(room);
		MeshRendererComponent* mrc = new MeshRendererComponent(room, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

		mrc->SetModelByName("10x1wall_around_20220104");
		mrc->SetMaterialByName("WallTopMat");
		transform->Translate(0, 2, 0);

		m_vecObject.push_back(room);
		m_vecNonAnimObjectRenderGroup.push_back(room);
	}

	{
		Object* room = new Object();

		TransformComponent* transform = new TransformComponent(room);
		MeshRendererComponent* mrc = new MeshRendererComponent(room, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

		mrc->SetModelByName("10x1wall_around_20220104");
		mrc->SetMaterialByName("WallTopMat");
		transform->Translate(0, 3, 0);

		m_vecObject.push_back(room);
		m_vecNonAnimObjectRenderGroup.push_back(room);
	}
	
	{
		Object* camera = new Object("camera");

		TransformComponent* transform = new TransformComponent(camera);
		CameraComponent* cam = new CameraComponent(camera);

		transform->Translate(0, 2, -2.8f);
		cam->SetFocusObject(FindObjectByName("head"));

		camera->m_pParent = FindObjectByName("player");

		m_pCameraObject = camera;
		m_vecObject.push_back(camera);
	}
	{
		Object* targetBoard = new Object();

		TransformComponent* transform = new TransformComponent(targetBoard);
		SkinnedMeshRendererComponent* skinnedMeshRenderer = new SkinnedMeshRendererComponent(
			targetBoard, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		// SkinnedMeshRenderer가 작동하려면 Animator가 있어야 함!
		// TBAC가 작동하려면 TargetBoardController가 있어야 함!
		TargetBoardControllerComponent* TCC = new TargetBoardControllerComponent(targetBoard);
		TargetBoardAnimatorComponent* TAC = new TargetBoardAnimatorComponent(targetBoard, "targetBoardStand");
		BoxColliderComponent* bodyCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.3f, 0.5f, 0.1f), XMFLOAT4(0, 0, 0, 1), TAC, 1);
		BoxColliderComponent* headCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 1.2f, 0), XMFLOAT3(0.15f, 0.2f, 0.1f), XMFLOAT4(0, 0, 0, 1), TAC, 1);

		skinnedMeshRenderer->SetModelByName("targetBoardStand");
		skinnedMeshRenderer->SetMaterialByName("TargetBoardMat");
		transform->Translate(-2, 0.5f, -0.8f);
		transform->RotateXYZDegree(0, 240, 0);

		m_vecObject.push_back(targetBoard);
		m_vecAnimObjectRenderGroup.push_back(targetBoard);
	}

	{
		Object* targetBoard = new Object();

		TransformComponent* transform = new TransformComponent(targetBoard);
		SkinnedMeshRendererComponent* skinnedMeshRenderer = new SkinnedMeshRendererComponent(
			targetBoard, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		// SkinnedMeshRenderer가 작동하려면 Animator가 있어야 함!
		// TBAC가 작동하려면 TargetBoardController가 있어야 함!
		TargetBoardControllerComponent* TCC = new TargetBoardControllerComponent(targetBoard);
		TargetBoardAnimatorComponent* TAC = new TargetBoardAnimatorComponent(targetBoard, "targetBoardStand");
		BoxColliderComponent* bodyCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.3f, 0.5f, 0.1f), XMFLOAT4(0, 0, 0, 1), TAC, 1);
		BoxColliderComponent* headCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 1.2f, 0), XMFLOAT3(0.15f, 0.2f, 0.1f), XMFLOAT4(0, 0, 0, 1), TAC, 1);

		skinnedMeshRenderer->SetModelByName("targetBoardStand");
		skinnedMeshRenderer->SetMaterialByName("TargetBoardMat");
		transform->Translate(-2.3f, 0.5f, -2);
		transform->RotateXYZDegree(0, 270, 0);

		m_vecObject.push_back(targetBoard);
		m_vecAnimObjectRenderGroup.push_back(targetBoard);
	}
	{
		Object* targetBoard = new Object();

		TransformComponent* transform = new TransformComponent(targetBoard);
		SkinnedMeshRendererComponent* skinnedMeshRenderer = new SkinnedMeshRendererComponent(
			targetBoard, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		// SkinnedMeshRenderer가 작동하려면 Animator가 있어야 함!
		// TBAC가 작동하려면 TargetBoardController가 있어야 함!
		TargetBoardControllerComponent* TCC = new TargetBoardControllerComponent(targetBoard);
		TargetBoardAnimatorComponent* TAC = new TargetBoardAnimatorComponent(targetBoard, "targetBoardStand");
		BoxColliderComponent* bodyCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.3f, 0.5f, 0.1f), XMFLOAT4(0, 0, 0, 1), TAC, 1);
		BoxColliderComponent* headCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 1.2f, 0), XMFLOAT3(0.15f, 0.2f, 0.1f), XMFLOAT4(0, 0, 0, 1), TAC, 1);

		skinnedMeshRenderer->SetModelByName("targetBoardStand");
		skinnedMeshRenderer->SetMaterialByName("TargetBoardMat");
		transform->Translate(-1.9f, 0.5f, -3.2f);
		transform->RotateXYZDegree(0, 300, 0);

		m_vecObject.push_back(targetBoard);
		m_vecAnimObjectRenderGroup.push_back(targetBoard);
	}
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

			g_TextureMng.DeleteTexture(temp.c_str());

			g_TextureMng.AddDepthBufferTexture(temp.c_str(), m_pd3dDevice, SHADOWMAPSIZE, SHADOWMAPSIZE, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
			m_LightMng->SetShadowMapName(temp.c_str(), i);
		}
	}
}

Object* Scene::FindObjectByName(const char* strName)
{
	for (auto iter = m_vecObject.begin(); iter != m_vecObject.end(); iter++) 
		if (strName == (*iter)->m_strName) return *iter;
	return nullptr;
}
