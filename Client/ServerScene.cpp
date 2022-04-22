#include "stdafx.h"
#include"Scene.h"
#include "ServerScene.h"
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
#include "ClientWsaModule.h"

void ServerScene::Init(Framework* pFramework, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	cout << "ServerScene\n";
	
	/*========================================================================
	* �ֿ� ���� �ʱ�ȭ
	*=======================================================================*/
	m_pd3dDevice = pd3dDevice;
	m_pd3dCommandList = pd3dCommandList;
	m_pd3dRootSignature = CreateRootSignature();

	/*========================================================================
	* ��Ʈ��ũ ����
	*=======================================================================*/
	m_pWsaModule = new ClientWsaModule;
	m_pWsaModule->Init(pFramework->GetHwnd());

	/*========================================================================
	* Ŀ�� ����
	*=======================================================================*/
	if (TEST_MOUSE_USABLE)
		SetCursorPos(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 2);


	/*========================================================================
	* ī�޶� ����
	*=======================================================================*/
	//m_pCamera = new FollowCamera();
	//m_pCamera->SetPosition(XMFLOAT3(0, 2, -3));
	//m_pCamera->SetLookAt(XMFLOAT3(0, 1, 1));

	/*========================================================================
	* ��ũ���� �� ����
	*=======================================================================*/
	CreateDescriptorHeap();


	/*==============================================================================
	* PassConstants ����
	*=============================================================================*/
	CreatePassInfoShaderResource();
	{
		int totalBckBufPixels
			= FRAME_BUFFER_WIDTH * 2;
		//= (FRAME_BUFFER_WIDTH * FRAME_BUFFER_WIDTH) / (16 * 1024);

		HRESULT hr = E_FAIL;



		//	��ó�� �ؽ�ó�� SRV�� UAV �� �� �������� ��.
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
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.NumElements = totalBckBufPixels * sizeof(float);
		uavDesc.Buffer.StructureByteStride = 1;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		m_pd3dDevice->CreateUnorderedAccessView(m_pd3duabHDRAvgLum, nullptr,
			&uavDesc, m_d3dSrvCPUDescriptorStartHandle);
		m_d3dSrvCPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
		m_d3dCbvGPUuabHDRAvgLumHandle = m_d3dSrvGPUDescriptorStartHandle;
		m_d3dSrvGPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;	// ������� ��
	}



	/*========================================================================
	* �ؽ���
	*=======================================================================*/
	g_TextureMng.Initialize(m_pd3dDevice);
	g_TextureMng.AddUnorderedAccessTexture("DownScaled", m_pd3dDevice, FRAME_BUFFER_WIDTH / 4, FRAME_BUFFER_HEIGHT / 4, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	g_TextureMng.AddUnorderedAccessTexture("Blur_Vertical", m_pd3dDevice, FRAME_BUFFER_WIDTH / 4, FRAME_BUFFER_HEIGHT / 4, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	g_TextureMng.AddUnorderedAccessTexture("Blur_Horizontal", m_pd3dDevice, FRAME_BUFFER_WIDTH / 4, FRAME_BUFFER_HEIGHT / 4, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	g_TextureMng.AddDepthBufferTexture("GBuffer_Depth", m_pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	g_TextureMng.AddRenderTargetTexture("GBuffer_Color", m_pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	g_TextureMng.AddRenderTargetTexture("GBuffer_Normal", m_pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	g_TextureMng.AddRenderTargetTexture("Screen", m_pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	//g_TextureMng.AddRenderTargetTexture("SmallScreen", m_pd3dDevice, FRAME_BUFFER_WIDTH/2, FRAME_BUFFER_HEIGHT/2, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);

	g_TextureMng.LoadFromFile("defaultDiffuseMap", m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	g_TextureMng.LoadFromFile("defaultNormalMap", m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	g_TextureMng.LoadFromFile("defaultSpecularMap", m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);

	/*========================================================================
	* ���׸���
	*=======================================================================*/
	MaterialDataImporter matDataImporter;
	matDataImporter.Load("Data/MaterialData.txt");


	/*========================================================================
	* ��
	*=======================================================================*/
	g_ModelMng.Initialize();


	/*========================================================================
	* ���� ����Ʈ �ε��ؼ� ���̶� �ؽ�ó�� ���ÿ� �ε�
	*=======================================================================*/
	AssetListDataImporter assetImporter;
	assetImporter.Load(m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);

	/*========================================================================
	* Pass 1 ���� ������Ʈ ������ �ε� �� ����
	*=======================================================================*/

	BuildObject();

	/*========================================================================
	* Pass 2 ���� ��ü ȭ�� �簢��
	*=======================================================================*/
	Screen* tempScreen
		= new Screen(m_pd3dDevice, m_pd3dCommandList,
			m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle,
			1.0f, 1.0f);
	m_vecScreenObject.push_back(tempScreen);

	/*========================================================================
	* ������ ������ �׸��� �ؽ�ó ����
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
	* PSO ����
	*=======================================================================*/
	CreatePSO();

	TransformComponent* t = m_pPlayer->FindComponent<TransformComponent>();
	
}

void ServerScene::CheckCollsion()
{
	static bool first = true;
	if (first) { first = false; return; }
	for (int i = 0; i < m_vecObject.size(); i++) {
		for (int j = i + 1; j < m_vecObject.size(); j++) {
			m_vecObject[i]->CheckCollision(m_vecObject[j]);
		}
	}
}

void ServerScene::SolveConstraint()
{
	for_each(m_vecObject.begin(), m_vecObject.end(), [](Object* o) {o->SolveConstraint(); });
}

void ServerScene::Input(UCHAR* pKeyBuffer)
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
void ServerScene::Update(float fTimeElapsed)
{
	m_fCurrentTime += fTimeElapsed;
	::memcpy(&m_pcbMappedPassInfo->m_xmfCurrentTime, &m_fCurrentTime, sizeof(float));

	CheckCollsion();
	SolveConstraint();

	for_each(m_vecObject.begin(), m_vecObject.end(), [fTimeElapsed](Object* o) {o->Update(fTimeElapsed); });
}
void ServerScene::Render(D3D12_CPU_DESCRIPTOR_HANDLE hBckBufRtv, D3D12_CPU_DESCRIPTOR_HANDLE hBckBufDsv)
{
	/*========================================================================
	* ��Ʈ �ñ״�ó, ��ũ���� ��, ���������� ������Ʈ, �޽� �������� ����
	*=======================================================================*/
	m_pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature);
	m_pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);
	m_pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	/*========================================================================
	* PassInfo ����
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
	* Pass 1. �޽� ���� To Color, Normal, Depth
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
	* Pass 1. ������ �׸��ڸ� ����
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
	* ��Ʈ �ñ״�ó, ��ũ���� ��, �޽� �������� ����
	*
	* �ٵ� PrevRender���� �ߴ� �������� ������� ������?
	*=======================================================================*/
	//m_pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature);				
	//m_pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);			
	//m_pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/*========================================================================
	* PassInfo ����
	*=======================================================================*/
	cam->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	cam->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	cam->SetViewportsAndScissorRects(m_pd3dCommandList);



	/*========================================================================
	* Pass 2. ��ũ�� ����
	*=======================================================================*/
	//m_pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature);

	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["ColorFromGBuffer"]);
	g_TextureMng.UseForShaderResource("GBuffer_Normal", m_pd3dCommandList, ROOTSIGNATURE_NORMAL_TEXTURE);
	g_TextureMng.UseForShaderResource("GBuffer_Depth", m_pd3dCommandList, ROOTSIGNATURE_DEPTH_TEXTURE);
	g_TextureMng.UseForShaderResource("GBuffer_Color", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
	m_vecScreenObject[0]->Render(m_pd3dCommandList);


	/*========================================================================
	* Pass 2. ��ƼŬ & ����Ʈ ����?
	*=======================================================================*/
	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Effect"]);
	for (auto iter = m_vecEffectRenderGroup.begin(); iter != m_vecEffectRenderGroup.end(); iter++) (*iter)->Render(m_pd3dCommandList);
	m_pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Particle"]);
	for (auto iter = m_vecParticleEmitter.begin(); iter != m_vecParticleEmitter.end(); iter++) (*iter)->Render(m_pd3dCommandList);

	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Text"]);
	for (auto iter = m_vecUIRenderGroup.begin(); iter != m_vecUIRenderGroup.end(); iter++) (*iter)->Render(m_pd3dCommandList);
	m_pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/*========================================================================
	* Pass 2. ������ ����
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
		�� �κ��� �𸣴� ���¿��� ���� ���� ��ƴ�.
		���� �޸� ����д�.

		�� ������ �׷��� [numthreads(1024, 1, 1,)]�� �Ǿ��ִ�.
		CS_DownScaleFirstPass()�� x�� �������� 1024���� �����ϰ�,
		���μ��� 1/4������ ������ �Ǳ� ������ 1920 / 4, 1080 / 4 �� 480, 270
		480�� 1024 �� ���� ��.
		270�� 1024, 1, 1�̴ϱ� 270��

		�̷��� �ϸ� gfAvgLum[270]���� ������ ����.
		*/

		/*===========================================================================
		* Calc FinalAvgLum and store to 'gfAvgLum' Buffer's [0].
		*==========================================================================*/
		m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["HDR_Second"]);

		m_pd3dCommandList->Dispatch(1, 1, 1);
		/*
		270���� �� ������ �ٿ���� �ؼ� �ϳ��� ����� �װ� gfAvgLum[0]�� �־���.
		*/

		/*===========================================================================
		* Bloom Rendering to 'Blur_Horizontal' Texture.
		*==========================================================================*/
		m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["PP_Bloom"]);

		d3dResourceBarrier[0].Transition.pResource = g_TextureMng.GetTextureResource("DownScaled");
		d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		d3dResourceBarrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		d3dResourceBarrier[1].Transition.pResource = g_TextureMng.GetTextureResource("Blur_Horizontal");
		d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
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
		d3dResourceBarrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
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
		d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
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

void ServerScene::ProcessSocket(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pWsaModule)
	{
		m_pWsaModule->ProcessSocketMessage(hWnd, nMessageID, wParam, lParam);
		
	}
}

void ServerScene::Release()
{
	if (m_pd3dcbPassInfo)
	{
		m_pd3dcbPassInfo->Unmap(0, NULL);
		m_pd3dcbPassInfo->Release();
	}
}

ID3D12RootSignature* ServerScene::CreateRootSignature()
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
void ServerScene::CreateDescriptorHeap()
{
	// heap ������ �Ϸ��� �̸� �����ڸ� �� ���� ������ �� �˾ƾ� �ϱ� ������
	// obj ������ �߿������� srv�� �� texture ������ �߿���..
	// �̸� �� �� ������ �� �� ������ �װ� ���صΰ� �� ������ �ϰڴµ�
	// ���� �������� ���ҽ� �ε�&��ε带 �Ѵٸ�?
	// �׷� ��쿡�� ���� �̸� �ѵ��� �ɾ�ΰ� ����� �͵� ����� �� ����
	// (���� ���, obj�� �� ��, ����ü�� �� ��, �ؽ��Ĵ� �� �� ������ �дٴ���)
	// ����, ������ ����. ������Ʈ�� ����ü, ���� ���� �� ���ļ� 1,024�� ������ �� ��.
	// �ؽ��ĵ� 128�� ������ ����� ��.
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
void ServerScene::CreatePSO()
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

	TextPSO TextPso = TextPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["Text"] = TextPso.GetPipelineState();

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

void ServerScene::CreatePassInfoShaderResource()
{
	UINT ncbElementBytes = ((sizeof(CB_PASS_INFO) + 255) & ~255); //256�� ���
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
void ServerScene::UpdatePassInfoAboutCamera()
{
	CameraComponent* cam = m_pCameraObject->FindComponent<CameraComponent>();
	XMFLOAT4X4 xmf4x4Temp;

	//xmf4x4Temp = m_pCamera->GetViewMatrix();
	xmf4x4Temp = cam->GetViewMatrix();
	XMStoreFloat4x4(&m_pcbMappedPassInfo->m_xmf4x4CameraView, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));
	xmf4x4Temp = Matrix4x4::Inverse(xmf4x4Temp);
	XMStoreFloat4x4(&m_pcbMappedPassInfo->m_xmf4x4CameraViewInv, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));

	//xmf4x4Temp = m_pCamera->GetProjectionMatrix();
	xmf4x4Temp = cam->GetProjectionMatrix();
	XMStoreFloat4x4(&m_pcbMappedPassInfo->m_xmf4x4CameraProjection, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));
	xmf4x4Temp = Matrix4x4::Inverse(xmf4x4Temp);
	XMStoreFloat4x4(&m_pcbMappedPassInfo->m_xmf4x4CameraProjectionInv, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));

	//::memcpy(&m_pcbMappedPassInfo->m_xmf3CameraPosition, &m_pCamera->GetPosition(), sizeof(XMFLOAT3));
	XMFLOAT3 xmf3WorldPos = m_pCameraObject->FindComponent<TransformComponent>()->GetPosition(Space::world);
	::memcpy(&m_pcbMappedPassInfo->m_xmf3CameraPosition, &xmf3WorldPos, sizeof(XMFLOAT3));
}

#define MAX_PARTICLE_NUM 1000

void ServerScene::BuildObject()
{
	// Particle Pool Initialize
	for (int i = 0; i < MAX_PARTICLE_NUM; i++) {
		Object* ptc = new Object();

		TransformComponent* t = new TransformComponent(ptc);
		ParticleComponent* pc = new ParticleComponent(ptc, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

		ptc->SetActive(false);

		m_vecParticlePool.push_back(ptc);
	}
	//{
	//	Object* pe = new Object("particleEmitter");
	//	
	//	TransformComponent* t = new TransformComponent(pe);
	//	ParticleEmitterComponent* pec = new ParticleEmitterComponent(pe, &m_vecParticlePool);
	//	t->Translate(3, 2, -1);
	//	pec->SetMaterialByName("ParticleTestMat");
	//	m_vecObject.push_back(pe);
	//	m_vecParticleEmitter.push_back(pe);
	//}

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

	// player
	{
		Object* player = new Object("player");
		m_pPlayer = player;

		TransformComponent* transform = new TransformComponent(player);
		RigidbodyComponent* rigidbody = new RigidbodyComponent(player);
		SkinnedMeshRendererComponent* skinnedMeshRenderer = new SkinnedMeshRendererComponent(player, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		HumanoidControllerComponent* humanoidController = new HumanoidControllerComponent(player, m_vecNonAnimObjectRenderGroup[0]);
		HumanoidAnimatorComponent* humanoidAnimator = new HumanoidAnimatorComponent(player, "Humanoid_Idle");
		NetInputManagerComponent* controller = new NetInputManagerComponent(player, m_pWsaModule);
		//InputManagerComponent* controller = new InputManagerComponent(player);
		SphereColliderComponent* sphereCollider = new SphereColliderComponent(player, XMFLOAT3(0, 0.5f, 0), 0.5f);

		skinnedMeshRenderer->SetModelByName("human");
		skinnedMeshRenderer->SetMaterialByName("DefaultMaterial");
		transform->Translate(-3, 0, -3);
		transform->RotateXYZDegree(0, 180, 0);

		m_vecObject.push_back(player);
		m_vecAnimObjectRenderGroup.push_back(player);
		FindObjectByName("pistol")->m_pParent = player;
	}
	{
		Object* head = new Object("head");

		TransformComponent* transform = new TransformComponent(head);
		transform->Translate(0, 1.2f, 5.0f);
		m_vecObject.push_back(head);

		head->m_pParent = FindObjectByName("player");
	}
	{
		Object* camera = new Object("camera");

		TransformComponent* transform = new TransformComponent(camera);
		CameraComponent* cam = new CameraComponent(camera);

		transform->Translate(0.6f, 1.8f, -2.2f);
		cam->SetFocusObject(FindObjectByName("head"));

		camera->m_pParent = FindObjectByName("player");

		m_pCameraObject = camera;
		m_vecObject.push_back(camera);
	}

	// Other Test
	/*{
		Object* player = new Object("other");

		TransformComponent* transform = new TransformComponent(player);
		RigidbodyComponent* rigidbody = new RigidbodyComponent(player);
		SkinnedMeshRendererComponent* skinnedMeshRenderer = new SkinnedMeshRendererComponent(player, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		HumanoidControllerComponent* humanoidController = new HumanoidControllerComponent(player, m_vecNonAnimObjectRenderGroup[0]);
		HumanoidAnimatorComponent* humanoidAnimator = new HumanoidAnimatorComponent(player, "Humanoid_Idle");
		InputManagerComponent* controller = new NetInputManagerComponent(player);
		SphereColliderComponent* sphereCollider = new SphereColliderComponent(player, XMFLOAT3(0, 0.5f, 0), 0.5f);

		skinnedMeshRenderer->SetModelByName("human");
		skinnedMeshRenderer->SetMaterialByName("DefaultMaterial");
		transform->Translate(-5, 0, -3);
		transform->RotateXYZDegree(0, 90, 0);

		m_vecObject.push_back(player);
		m_vecAnimObjectRenderGroup.push_back(player);
	}*/


	{
		Object* targetBoard = new Object("TB0");

		TransformComponent* transform = new TransformComponent(targetBoard);
		SkinnedMeshRendererComponent* skinnedMeshRenderer = new SkinnedMeshRendererComponent(
			targetBoard, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		// SkinnedMeshRenderer�� �۵��Ϸ��� Animator�� �־�� ��!
		// TBAC�� �۵��Ϸ��� TargetBoardController�� �־�� ��!
		TargetBoardControllerComponent* TCC = new TargetBoardControllerComponent(targetBoard);
		TargetBoardAnimatorComponent* TAC = new TargetBoardAnimatorComponent(targetBoard, "targetBoardStand");
		BoxColliderComponent* bodyCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.3f, 0.5f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);
		BoxColliderComponent* headCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 1.2f, 0), XMFLOAT3(0.15f, 0.2f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);

		skinnedMeshRenderer->SetModelByName("targetBoardStand");
		skinnedMeshRenderer->SetMaterialByName("TargetBoardMat");
		transform->Translate(-4, 0.5f, -20);
		transform->RotateXYZDegree(0, 270, 0);
		TCC->Die();

		m_vecObject.push_back(targetBoard);
		m_vecAnimObjectRenderGroup.push_back(targetBoard);
	}

	{
		Object* targetBoard = new Object("TB1");

		TransformComponent* transform = new TransformComponent(targetBoard);
		SkinnedMeshRendererComponent* skinnedMeshRenderer = new SkinnedMeshRendererComponent(
			targetBoard, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		// SkinnedMeshRenderer�� �۵��Ϸ��� Animator�� �־�� ��!
		// TBAC�� �۵��Ϸ��� TargetBoardController�� �־�� ��!
		TargetBoardControllerComponent* TCC = new TargetBoardControllerComponent(targetBoard);
		TargetBoardAnimatorComponent* TAC = new TargetBoardAnimatorComponent(targetBoard, "targetBoardStand");
		BoxColliderComponent* bodyCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.3f, 0.5f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);
		BoxColliderComponent* headCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 1.2f, 0), XMFLOAT3(0.15f, 0.2f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);

		skinnedMeshRenderer->SetModelByName("targetBoardStand");
		skinnedMeshRenderer->SetMaterialByName("TargetBoardMat");
		transform->Translate(-10.5, 0.5f, -24);
		transform->RotateXYZDegree(0, 300, 0);
		TCC->Die();

		m_vecObject.push_back(targetBoard);
		m_vecAnimObjectRenderGroup.push_back(targetBoard);
	}
	{
		Object* targetBoard = new Object("TB2");

		TransformComponent* transform = new TransformComponent(targetBoard);
		SkinnedMeshRendererComponent* skinnedMeshRenderer = new SkinnedMeshRendererComponent(
			targetBoard, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		// SkinnedMeshRenderer�� �۵��Ϸ��� Animator�� �־�� ��!
		// TBAC�� �۵��Ϸ��� TargetBoardController�� �־�� ��!
		TargetBoardControllerComponent* TCC = new TargetBoardControllerComponent(targetBoard);
		TargetBoardAnimatorComponent* TAC = new TargetBoardAnimatorComponent(targetBoard, "targetBoardStand");
		BoxColliderComponent* bodyCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.3f, 0.5f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);
		BoxColliderComponent* headCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 1.2f, 0), XMFLOAT3(0.15f, 0.2f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);

		skinnedMeshRenderer->SetModelByName("targetBoardStand");
		skinnedMeshRenderer->SetMaterialByName("TargetBoardMat");
		transform->Translate(-13, 0.5f, -22);
		transform->RotateXYZDegree(0, 270, 0);
		TCC->Die();

		m_vecObject.push_back(targetBoard);
		m_vecAnimObjectRenderGroup.push_back(targetBoard);
	}
	{
		Object* targetBoard = new Object("TB3");

		TransformComponent* transform = new TransformComponent(targetBoard);
		SkinnedMeshRendererComponent* skinnedMeshRenderer = new SkinnedMeshRendererComponent(
			targetBoard, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		// SkinnedMeshRenderer�� �۵��Ϸ��� Animator�� �־�� ��!
		// TBAC�� �۵��Ϸ��� TargetBoardController�� �־�� ��!
		TargetBoardControllerComponent* TCC = new TargetBoardControllerComponent(targetBoard);
		TargetBoardAnimatorComponent* TAC = new TargetBoardAnimatorComponent(targetBoard, "targetBoardStand");
		BoxColliderComponent* bodyCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.3f, 0.5f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);
		BoxColliderComponent* headCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 1.2f, 0), XMFLOAT3(0.15f, 0.2f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);

		skinnedMeshRenderer->SetModelByName("targetBoardStand");
		skinnedMeshRenderer->SetMaterialByName("TargetBoardMat");
		transform->Translate(-11.8, 0.5f, -19.7);
		transform->RotateXYZDegree(0, 240, 0);
		TCC->Die();

		m_vecObject.push_back(targetBoard);
		m_vecAnimObjectRenderGroup.push_back(targetBoard);
	}

	//room 3
	{
		Object* targetBoard = new Object("TB4");

		TransformComponent* transform = new TransformComponent(targetBoard);
		SkinnedMeshRendererComponent* skinnedMeshRenderer = new SkinnedMeshRendererComponent(
			targetBoard, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		// SkinnedMeshRenderer�� �۵��Ϸ��� Animator�� �־�� ��!
		// TBAC�� �۵��Ϸ��� TargetBoardController�� �־�� ��!
		TargetBoardControllerComponent* TCC = new TargetBoardControllerComponent(targetBoard);
		TargetBoardAnimatorComponent* TAC = new TargetBoardAnimatorComponent(targetBoard, "targetBoardStand");
		BoxColliderComponent* bodyCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.3f, 0.5f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);
		BoxColliderComponent* headCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 1.2f, 0), XMFLOAT3(0.15f, 0.2f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);

		skinnedMeshRenderer->SetModelByName("targetBoardStand");
		skinnedMeshRenderer->SetMaterialByName("TargetBoardMat");
		transform->Translate(-17.1, 0.5f, -22);
		TCC->Die();

		m_vecObject.push_back(targetBoard);
		m_vecAnimObjectRenderGroup.push_back(targetBoard);
	}
	{
		Object* targetBoard = new Object("TB5");

		TransformComponent* transform = new TransformComponent(targetBoard);
		SkinnedMeshRendererComponent* skinnedMeshRenderer = new SkinnedMeshRendererComponent(
			targetBoard, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		// SkinnedMeshRenderer�� �۵��Ϸ��� Animator�� �־�� ��!
		// TBAC�� �۵��Ϸ��� TargetBoardController�� �־�� ��!
		TargetBoardControllerComponent* TCC = new TargetBoardControllerComponent(targetBoard);
		TargetBoardAnimatorComponent* TAC = new TargetBoardAnimatorComponent(targetBoard, "targetBoardStand");
		BoxColliderComponent* bodyCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.3f, 0.5f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);
		BoxColliderComponent* headCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 1.2f, 0), XMFLOAT3(0.15f, 0.2f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);

		skinnedMeshRenderer->SetModelByName("targetBoardStand");
		skinnedMeshRenderer->SetMaterialByName("TargetBoardMat");
		transform->Translate(-19.5, 0.5f, -25.3);
		TCC->Die();

		m_vecObject.push_back(targetBoard);
		m_vecAnimObjectRenderGroup.push_back(targetBoard);
	}
	{
		Object* targetBoard = new Object("TB6");

		TransformComponent* transform = new TransformComponent(targetBoard);
		SkinnedMeshRendererComponent* skinnedMeshRenderer = new SkinnedMeshRendererComponent(
			targetBoard, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		// SkinnedMeshRenderer�� �۵��Ϸ��� Animator�� �־�� ��!
		// TBAC�� �۵��Ϸ��� TargetBoardController�� �־�� ��!
		TargetBoardControllerComponent* TCC = new TargetBoardControllerComponent(targetBoard);
		TargetBoardAnimatorComponent* TAC = new TargetBoardAnimatorComponent(targetBoard, "targetBoardStand");
		BoxColliderComponent* bodyCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.3f, 0.5f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);
		BoxColliderComponent* headCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 1.2f, 0), XMFLOAT3(0.15f, 0.2f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);

		skinnedMeshRenderer->SetModelByName("targetBoardStand");
		skinnedMeshRenderer->SetMaterialByName("TargetBoardMat");
		transform->Translate(-23.8, 0.5f, -25.3);
		TCC->Die();

		m_vecObject.push_back(targetBoard);
		m_vecAnimObjectRenderGroup.push_back(targetBoard);
	}
	{
		Object* targetBoard = new Object("TB7");

		TransformComponent* transform = new TransformComponent(targetBoard);
		SkinnedMeshRendererComponent* skinnedMeshRenderer = new SkinnedMeshRendererComponent(
			targetBoard, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		// SkinnedMeshRenderer�� �۵��Ϸ��� Animator�� �־�� ��!
		// TBAC�� �۵��Ϸ��� TargetBoardController�� �־�� ��!
		TargetBoardControllerComponent* TCC = new TargetBoardControllerComponent(targetBoard);
		TargetBoardAnimatorComponent* TAC = new TargetBoardAnimatorComponent(targetBoard, "targetBoardStand");
		BoxColliderComponent* bodyCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.3f, 0.5f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);
		BoxColliderComponent* headCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 1.2f, 0), XMFLOAT3(0.15f, 0.2f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);

		skinnedMeshRenderer->SetModelByName("targetBoardStand");
		skinnedMeshRenderer->SetMaterialByName("TargetBoardMat");
		transform->Translate(-26.5, 0.5f, -21.7);
		TCC->Die();

		m_vecObject.push_back(targetBoard);
		m_vecAnimObjectRenderGroup.push_back(targetBoard);
	}

	//room 4
	{
		Object* targetBoard = new Object("TB8");

		TransformComponent* transform = new TransformComponent(targetBoard);
		SkinnedMeshRendererComponent* skinnedMeshRenderer = new SkinnedMeshRendererComponent(
			targetBoard, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		// SkinnedMeshRenderer�� �۵��Ϸ��� Animator�� �־�� ��!
		// TBAC�� �۵��Ϸ��� TargetBoardController�� �־�� ��!
		TargetBoardControllerComponent* TCC = new TargetBoardControllerComponent(targetBoard);
		TargetBoardAnimatorComponent* TAC = new TargetBoardAnimatorComponent(targetBoard, "targetBoardStand");
		BoxColliderComponent* bodyCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.3f, 0.5f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);
		BoxColliderComponent* headCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 1.2f, 0), XMFLOAT3(0.15f, 0.2f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);

		skinnedMeshRenderer->SetModelByName("targetBoardStand");
		skinnedMeshRenderer->SetMaterialByName("TargetBoardMat");
		transform->Translate(-11.8, 0.5f, -13);
		transform->RotateXYZDegree(0, 70, 0);
		TCC->Die();

		m_vecObject.push_back(targetBoard);
		m_vecAnimObjectRenderGroup.push_back(targetBoard);
	}
	{
		Object* targetBoard = new Object("TB9");

		TransformComponent* transform = new TransformComponent(targetBoard);
		SkinnedMeshRendererComponent* skinnedMeshRenderer = new SkinnedMeshRendererComponent(
			targetBoard, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		// SkinnedMeshRenderer�� �۵��Ϸ��� Animator�� �־�� ��!
		// TBAC�� �۵��Ϸ��� TargetBoardController�� �־�� ��!
		TargetBoardControllerComponent* TCC = new TargetBoardControllerComponent(targetBoard);
		TargetBoardAnimatorComponent* TAC = new TargetBoardAnimatorComponent(targetBoard, "targetBoardStand");
		BoxColliderComponent* bodyCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.3f, 0.5f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);
		BoxColliderComponent* headCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 1.2f, 0), XMFLOAT3(0.15f, 0.2f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);

		skinnedMeshRenderer->SetModelByName("targetBoardStand");
		skinnedMeshRenderer->SetMaterialByName("TargetBoardMat");
		transform->Translate(-10.6, 0.5f, -7.1);
		transform->RotateXYZDegree(0, 70, 0);
		TCC->Die();

		m_vecObject.push_back(targetBoard);
		m_vecAnimObjectRenderGroup.push_back(targetBoard);
	}
	{
		Object* targetBoard = new Object("TB10");

		TransformComponent* transform = new TransformComponent(targetBoard);
		SkinnedMeshRendererComponent* skinnedMeshRenderer = new SkinnedMeshRendererComponent(
			targetBoard, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		// SkinnedMeshRenderer�� �۵��Ϸ��� Animator�� �־�� ��!
		// TBAC�� �۵��Ϸ��� TargetBoardController�� �־�� ��!
		TargetBoardControllerComponent* TCC = new TargetBoardControllerComponent(targetBoard);
		TargetBoardAnimatorComponent* TAC = new TargetBoardAnimatorComponent(targetBoard, "targetBoardStand");
		BoxColliderComponent* bodyCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.3f, 0.5f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);
		BoxColliderComponent* headCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 1.2f, 0), XMFLOAT3(0.15f, 0.2f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);

		skinnedMeshRenderer->SetModelByName("targetBoardStand");
		skinnedMeshRenderer->SetMaterialByName("TargetBoardMat");
		transform->Translate(-14.9, 0.5f, -8.2);
		transform->RotateXYZDegree(0, 90, 0);
		TCC->Die();

		m_vecObject.push_back(targetBoard);
		m_vecAnimObjectRenderGroup.push_back(targetBoard);
	}
	{
		Object* targetBoard = new Object("TB11");

		TransformComponent* transform = new TransformComponent(targetBoard);
		SkinnedMeshRendererComponent* skinnedMeshRenderer = new SkinnedMeshRendererComponent(
			targetBoard, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		// SkinnedMeshRenderer�� �۵��Ϸ��� Animator�� �־�� ��!
		// TBAC�� �۵��Ϸ��� TargetBoardController�� �־�� ��!
		TargetBoardControllerComponent* TCC = new TargetBoardControllerComponent(targetBoard);
		TargetBoardAnimatorComponent* TAC = new TargetBoardAnimatorComponent(targetBoard, "targetBoardStand");
		BoxColliderComponent* bodyCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.3f, 0.5f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);
		BoxColliderComponent* headCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 1.2f, 0), XMFLOAT3(0.15f, 0.2f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);

		skinnedMeshRenderer->SetModelByName("targetBoardStand");
		skinnedMeshRenderer->SetMaterialByName("TargetBoardMat");
		transform->Translate(-19.5, 0.5f, -13);
		transform->RotateXYZDegree(0, 80, 0);
		TCC->Die();

		m_vecObject.push_back(targetBoard);
		m_vecAnimObjectRenderGroup.push_back(targetBoard);
	}
	{
		Object* targetBoard = new Object("TB12");

		TransformComponent* transform = new TransformComponent(targetBoard);
		SkinnedMeshRendererComponent* skinnedMeshRenderer = new SkinnedMeshRendererComponent(
			targetBoard, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		// SkinnedMeshRenderer�� �۵��Ϸ��� Animator�� �־�� ��!
		// TBAC�� �۵��Ϸ��� TargetBoardController�� �־�� ��!
		TargetBoardControllerComponent* TCC = new TargetBoardControllerComponent(targetBoard);
		TargetBoardAnimatorComponent* TAC = new TargetBoardAnimatorComponent(targetBoard, "targetBoardStand");
		BoxColliderComponent* bodyCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.3f, 0.5f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);
		BoxColliderComponent* headCollider = new BoxColliderComponent(
			targetBoard, XMFLOAT3(0, 1.2f, 0), XMFLOAT3(0.15f, 0.2f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);

		skinnedMeshRenderer->SetModelByName("targetBoardStand");
		skinnedMeshRenderer->SetMaterialByName("TargetBoardMat");
		transform->Translate(-22.6, 0.5f, -7.1);
		transform->RotateXYZDegree(0, 90, 0);
		TCC->Die();

		m_vecObject.push_back(targetBoard);
		m_vecAnimObjectRenderGroup.push_back(targetBoard);
	}

	// Env
	{
		Object* env = new Object();

		TransformComponent* transform = new TransformComponent(env);
		MeshRendererComponent* mrc = new MeshRendererComponent(env, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

		mrc->SetModelByName("tutorialHouse_floor");
		mrc->SetMaterialByName("floorMat");

		m_vecObject.push_back(env);
		m_vecNonAnimObjectRenderGroup.push_back(env);
	}
	{
		Object* env = new Object();

		TransformComponent* transform = new TransformComponent(env);
		MeshRendererComponent* mrc = new MeshRendererComponent(env, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

		mrc->SetModelByName("tutorialHouse_wall");
		mrc->SetMaterialByName("wallMat");

		m_vecObject.push_back(env);
		m_vecNonAnimObjectRenderGroup.push_back(env);
	}
	{
		Object* env = new Object();

		TransformComponent* transform = new TransformComponent(env);
		MeshRendererComponent* mrc = new MeshRendererComponent(env, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

		mrc->SetModelByName("tutorialHouse_ceiling");
		mrc->SetMaterialByName("ceilingMat");

		m_vecObject.push_back(env);
		m_vecNonAnimObjectRenderGroup.push_back(env);
	}
	{
		Object* env = new Object();

		TransformComponent* transform = new TransformComponent(env);
		MeshRendererComponent* mrc = new MeshRendererComponent(env, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

		mrc->SetModelByName("tutorialHouse_doorFrame");
		mrc->SetMaterialByName("Concrete_Door");

		m_vecObject.push_back(env);
		m_vecNonAnimObjectRenderGroup.push_back(env);
	}
	{
		Object* env = new Object();

		TransformComponent* transform = new TransformComponent(env);
		MeshRendererComponent* mrc = new MeshRendererComponent(env, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

		mrc->SetModelByName("tutorialHouse_fence");
		mrc->SetMaterialByName("Wooden_fence");
		transform->Translate(0, -0.3f, 0);

		m_vecObject.push_back(env);
		m_vecNonAnimObjectRenderGroup.push_back(env);
	}
	{
		Object* env = new Object();

		TransformComponent* transform = new TransformComponent(env);
		MeshRendererComponent* mrc = new MeshRendererComponent(env, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

		mrc->SetModelByName("tutorialHouse_outerFloor");
		mrc->SetMaterialByName("outerFloorMat");

		m_vecObject.push_back(env);
		m_vecNonAnimObjectRenderGroup.push_back(env);
	}
	{
		Object* env = new Object();

		TransformComponent* transform = new TransformComponent(env);
		MeshRendererComponent* mrc = new MeshRendererComponent(env, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		mrc->SetModelByName("tutorialHouse_outerWall");
		mrc->SetMaterialByName("outerWallMat");

		m_vecObject.push_back(env);
		m_vecNonAnimObjectRenderGroup.push_back(env);
	}

	// Door
	{
		Object* env = new Object("door00");

		TransformComponent* transform = new TransformComponent(env);
		BoxColliderComponent* boxCollider = new BoxColliderComponent(env, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(1.5f, 1.5f, 0.15f), XMFLOAT4(0, 0, 0, 1));
		transform->Translate(-1.5, 0, -9.15);
		MeshRendererComponent* mrc = new MeshRendererComponent(env, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		mrc->SetModelByName("tutorialHouse_door");
		mrc->SetMaterialByName("doorMat");
		DoorComponent* dc = new DoorComponent(env, false);


		m_vecObject.push_back(env);
		m_vecNonAnimObjectRenderGroup.push_back(env);
	}
	{
		Object* env = new Object("door01");

		TransformComponent* transform = new TransformComponent(env);
		BoxColliderComponent* boxCollider = new BoxColliderComponent(env, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(1.5f, 1.5f, 0.15f), XMFLOAT4(0, 0, 0, 1));
		transform->Translate(-1.5, 0, -16.85);
		MeshRendererComponent* mrc = new MeshRendererComponent(env, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		mrc->SetModelByName("tutorialHouse_door");
		mrc->SetMaterialByName("doorMat");
		DoorComponent* dc = new DoorComponent(env);


		m_vecObject.push_back(env);
		m_vecNonAnimObjectRenderGroup.push_back(env);
	}
	{
		Object* env = new Object("door02");

		TransformComponent* transform = new TransformComponent(env);
		BoxColliderComponent* boxCollider = new BoxColliderComponent(env, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(1.5f, 1.5f, 0.15f), XMFLOAT4(0, 0, 0, 1));
		transform->Translate(-5.85, 0, -23.5);
		transform->RotateXYZDegree(0, 90, 0);
		MeshRendererComponent* mrc = new MeshRendererComponent(env, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		mrc->SetModelByName("tutorialHouse_door");
		mrc->SetMaterialByName("doorMat");
		DoorComponent* dc = new DoorComponent(env);


		m_vecObject.push_back(env);
		m_vecNonAnimObjectRenderGroup.push_back(env);
	}
	{
		Object* env = new Object("door03");

		TransformComponent* transform = new TransformComponent(env);
		BoxColliderComponent* boxCollider = new BoxColliderComponent(env, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(1.5f, 1.5f, 0.15f), XMFLOAT4(0, 0, 0, 1));
		transform->Translate(-14.85, 0, -16.5);
		transform->RotateXYZDegree(0, 270, 0);
		MeshRendererComponent* mrc = new MeshRendererComponent(env, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		mrc->SetModelByName("tutorialHouse_door");
		mrc->SetMaterialByName("doorMat");
		DoorComponent* dc = new DoorComponent(env);


		m_vecObject.push_back(env);
		m_vecNonAnimObjectRenderGroup.push_back(env);
	}
	{
		Object* env = new Object("door04");

		TransformComponent* transform = new TransformComponent(env);
		BoxColliderComponent* boxCollider = new BoxColliderComponent(env, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(1.5f, 1.5f, 0.15f), XMFLOAT4(0, 0, 0, 1));
		transform->Translate(-27.5, 0, -14.85);
		transform->RotateXYZDegree(0, 180, 0);
		MeshRendererComponent* mrc = new MeshRendererComponent(env, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		mrc->SetModelByName("tutorialHouse_door");
		mrc->SetMaterialByName("doorMat");
		DoorComponent* dc = new DoorComponent(env);


		m_vecObject.push_back(env);
		m_vecNonAnimObjectRenderGroup.push_back(env);
	}
	{
		Object* env = new Object("door05");

		TransformComponent* transform = new TransformComponent(env);
		BoxColliderComponent* boxCollider = new BoxColliderComponent(env, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(1.5f, 1.5f, 0.15f), XMFLOAT4(0, 0, 0, 1));
		transform->Translate(-7.85, 0, -1.5);
		transform->RotateXYZDegree(0, 270, 0);
		MeshRendererComponent* mrc = new MeshRendererComponent(env, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		mrc->SetModelByName("tutorialHouse_door");
		mrc->SetMaterialByName("doorMat");
		DoorComponent* dc = new DoorComponent(env);


		m_vecObject.push_back(env);
		m_vecNonAnimObjectRenderGroup.push_back(env);
	}

	// Trigger
	{
		Object* trig = new Object();

		TransformComponent* transform = new TransformComponent(trig);
		transform->Translate(-1.5, 0, -7.5);
		BoxColliderComponent* boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(1.5, 1.5, 1.5), XMFLOAT4(0, 0, 0, 1), true);
		EventComponent* trigEvent = new EventComponent(trig);
		trigEvent->AddDoorOpen(FindObjectByName("door00")->FindComponent<DoorComponent>());

		m_vecObject.push_back(trig);
	}
	{
		Object* trig = new Object();

		TransformComponent* transform = new TransformComponent(trig);
		transform->Translate(-1.5, 0, -15);
		BoxColliderComponent* boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(1.5, 1.5, 2), XMFLOAT4(0, 0, 0, 1), true);
		EventComponent* trigEvent = new EventComponent(trig);
		trigEvent->AddDoorOpen(FindObjectByName("door01")->FindComponent<DoorComponent>());

		m_vecObject.push_back(trig);
	}
	{
		Object* trig = new Object();

		TransformComponent* transform = new TransformComponent(trig);
		transform->Translate(-1.5, 0, -20);
		BoxColliderComponent* boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(1.5, 1.5, 2), XMFLOAT4(0, 0, 0, 1), true);
		EventComponent* trigEvent = new EventComponent(trig);
		trigEvent->AddDoorClose(FindObjectByName("door02")->FindComponent<DoorComponent>());
		trigEvent->AddSpawn(FindObjectByName("TB0")->FindComponent<Character>());

		m_vecObject.push_back(trig);
	}
	{
		Object* trig = new Object();

		TransformComponent* transform = new TransformComponent(trig);
		transform->Translate(-7.5, 0, -21);
		BoxColliderComponent* boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(1.5, 1.5, 2), XMFLOAT4(0, 0, 0, 1), true);
		EventComponent* trigEvent = new EventComponent(trig);
		trigEvent->AddDoorClose(FindObjectByName("door03")->FindComponent<DoorComponent>());
		trigEvent->AddDoorClose(FindObjectByName("door02")->FindComponent<DoorComponent>());
		trigEvent->AddSpawn(FindObjectByName("TB1")->FindComponent<Character>());
		trigEvent->AddSpawn(FindObjectByName("TB2")->FindComponent<Character>());
		trigEvent->AddSpawn(FindObjectByName("TB3")->FindComponent<Character>());

		m_vecObject.push_back(trig);
	}
	{
		Object* trig = new Object();

		TransformComponent* transform = new TransformComponent(trig);
		transform->Translate(-19, 0, -17);
		BoxColliderComponent* boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(1.5, 1.5, 2), XMFLOAT4(0, 0, 0, 1), true);
		EventComponent* trigEvent = new EventComponent(trig);
		trigEvent->AddDoorClose(FindObjectByName("door03")->FindComponent<DoorComponent>());
		trigEvent->AddDoorClose(FindObjectByName("door04")->FindComponent<DoorComponent>());
		trigEvent->AddSpawn(FindObjectByName("TB4")->FindComponent<Character>());
		trigEvent->AddSpawn(FindObjectByName("TB5")->FindComponent<Character>());
		trigEvent->AddSpawn(FindObjectByName("TB6")->FindComponent<Character>());
		trigEvent->AddSpawn(FindObjectByName("TB7")->FindComponent<Character>());

		m_vecObject.push_back(trig);
	}
	{
		Object* trig = new Object();

		TransformComponent* transform = new TransformComponent(trig);
		transform->Translate(-27, 0, -11.5);
		BoxColliderComponent* boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(1.5, 1.5, 2), XMFLOAT4(0, 0, 0, 1), true);
		EventComponent* trigEvent = new EventComponent(trig);
		trigEvent->AddDoorClose(FindObjectByName("door04")->FindComponent<DoorComponent>());
		trigEvent->AddDoorClose(FindObjectByName("door05")->FindComponent<DoorComponent>());
		trigEvent->AddSpawn(FindObjectByName("TB8")->FindComponent<Character>());
		trigEvent->AddSpawn(FindObjectByName("TB9")->FindComponent<Character>());
		trigEvent->AddSpawn(FindObjectByName("TB10")->FindComponent<Character>());
		trigEvent->AddSpawn(FindObjectByName("TB11")->FindComponent<Character>());
		trigEvent->AddSpawn(FindObjectByName("TB12")->FindComponent<Character>());

		m_vecObject.push_back(trig);
	}


	//{
	//	Object* box = new Object();

	//	TransformComponent* transform = new TransformComponent(box);
	//	transform->Translate(XMFLOAT3(-5.55, 0, -12));
	//	BoxColliderComponent* boxCollider00 = new BoxColliderComponent(box, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(6.5 * 0.5, 1.5f, 5 * 0.5), XMFLOAT4(0, 0, 0, 1));

	//	m_vecObject.push_back(box);
	//	m_vecNonAnimObjectRenderGroup.push_back(box);
	//}
	//{
	//	Object* box = new Object();

	//	TransformComponent* transform = new TransformComponent(box);
	//	transform->Translate(XMFLOAT3(-4.7, 0, -16));
	//	BoxColliderComponent* boxCollider00 = new BoxColliderComponent(box, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(2.0 * 0.5, 1.5f, 3.3 * 0.5), XMFLOAT4(0, 0, 0, 1));

	//	m_vecObject.push_back(box);
	//	m_vecNonAnimObjectRenderGroup.push_back(box);
	//}
	//{
	//	Object* box = new Object();

	//	TransformComponent* transform = new TransformComponent(box);
	//	transform->Translate(XMFLOAT3(-5.9, 0, -19.5));
	//	BoxColliderComponent* boxCollider00 = new BoxColliderComponent(box, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(5.0 * 0.5, 1.5f, 0.7 * 0.5), XMFLOAT4(0, 0, 0, 1));

	//	m_vecObject.push_back(box);
	//	m_vecNonAnimObjectRenderGroup.push_back(box);
	//}


	//{
	//	Object* box = new Object();

	//	TransformComponent* transform = new TransformComponent(box);
	//	transform->Translate(XMFLOAT3(-0.319, 0, 12.337));
	//	BoxColliderComponent* boxCollider00 = new BoxColliderComponent(box, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(25.954 * 0.5, 1.5f, 0.53 * 0.5), XMFLOAT4(0, 0, 0, 1));

	//	m_vecObject.push_back(box);
	//	m_vecNonAnimObjectRenderGroup.push_back(box);
	//} 
	//{
	//	Object* box = new Object();

	//	TransformComponent* transform = new TransformComponent(box);
	//	transform->Translate(XMFLOAT3(14.959, 0, -0.293));
	//	BoxColliderComponent* boxCollider01 = new BoxColliderComponent(box, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(0.563 * 0.5, 1.5f, 30.026 * 0.5), XMFLOAT4(0, 0, 0, 1));

	//	m_vecObject.push_back(box);
	//	m_vecNonAnimObjectRenderGroup.push_back(box);
	//}
	//{
	//	Object* box = new Object();

	//	TransformComponent* transform = new TransformComponent(box);
	//	transform->Translate(XMFLOAT3(7.907, 0, 9.01));
	//	BoxColliderComponent* boxCollider02 = new BoxColliderComponent(box, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(11.752 * 0.5, 1.5f, 0.497 * 0.5), XMFLOAT4(0, 0, 0, 1));

	//	m_vecObject.push_back(box);
	//	m_vecNonAnimObjectRenderGroup.push_back(box);
	//}
	//{
	//	Object* box = new Object();

	//	TransformComponent* transform = new TransformComponent(box);
	//	transform->Translate(XMFLOAT3(6.865, 0, 9.374));
	//	BoxColliderComponent* boxCollider03 = new BoxColliderComponent(box, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(0.695 * 0.5, 1.5f, 1.655 * 0.5), XMFLOAT4(0, 0, 0, 1));

	//	m_vecObject.push_back(box);
	//	m_vecNonAnimObjectRenderGroup.push_back(box);
	//}
	//{
	//	Object* box = new Object();

	//	TransformComponent* transform = new TransformComponent(box);
	//	transform->Translate(XMFLOAT3(-4.531, 0, -12.999));
	//	BoxColliderComponent* boxCollider04 = new BoxColliderComponent(box, XMFLOAT3(0, 0, 0), XMFLOAT3(7.945 * 0.5, 1.5f, 3.012 * 0.5), XMFLOAT4(0, 0, 0, 1));

	//	m_vecObject.push_back(box);
	//	m_vecNonAnimObjectRenderGroup.push_back(box);
	//}
	//{
	//	Object* box = new Object();

	//	TransformComponent* transform = new TransformComponent(box);
	//	transform->Translate(XMFLOAT3(4.531, 0, 12.999));
	//	BoxColliderComponent* boxCollider04 = new BoxColliderComponent(box, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(7.945 * 0.5, 1.5f, 3.012 * 0.5), XMFLOAT4(0, 0, 0, 1));

	//	m_vecObject.push_back(box);
	//	m_vecNonAnimObjectRenderGroup.push_back(box);
	//}
	//{
	//	Object* box = new Object();

	//	TransformComponent* transform = new TransformComponent(box);
	//	transform->Translate(XMFLOAT3(5.855, 0, 19.471));
	//	BoxColliderComponent* boxCollider05 = new BoxColliderComponent(box, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(5.065 * 0.5, 1.5f, 0.563 * 0.5), XMFLOAT4(0, 0, 0, 1));

	//	m_vecObject.push_back(box);
	//	m_vecNonAnimObjectRenderGroup.push_back(box);
	//}
	//{
	//	Object* box = new Object();

	//	TransformComponent* transform = new TransformComponent(box);
	//	transform->Translate(XMFLOAT3(7.494, 0, 25.396));
	//	BoxColliderComponent* boxCollider06 = new BoxColliderComponent(box, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(0.761 * 0.5, 1.5f, 14.963 * 0.5), XMFLOAT4(0, 0, 0, 1));

	//	m_vecObject.push_back(box);
	//	m_vecNonAnimObjectRenderGroup.push_back(box);
	//}
	//{
	//	Object* box = new Object();

	//	TransformComponent* transform = new TransformComponent(box);
	//	transform->Translate(XMFLOAT3(14.826, 0, 21.573));
	//	BoxColliderComponent* boxCollider07 = new BoxColliderComponent(box, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(6.952 * 0.5, 1.5f, 0.563 * 0.5), XMFLOAT4(0, 0, 0, 1));

	//	m_vecObject.push_back(box);
	//	m_vecNonAnimObjectRenderGroup.push_back(box);
	//}
	//{
	//	Object* box = new Object();

	//	TransformComponent* transform = new TransformComponent(box);
	//	transform->Translate(XMFLOAT3(16.018, 0, 14.836));
	//	BoxColliderComponent* boxCollider08 = new BoxColliderComponent(box, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(0.364 * 0.5, 1.5f, 19.829 * 0.5), XMFLOAT4(0, 0, 0, 1));

	//	m_vecObject.push_back(box);
	//	m_vecNonAnimObjectRenderGroup.push_back(box);
	//}
	//{
	//	Object* box = new Object();

	//	TransformComponent* transform = new TransformComponent(box);
	//	transform->Translate(XMFLOAT3(29.342, 0, 14.919));
	//	BoxColliderComponent* boxCollider09 = new BoxColliderComponent(box, XMFLOAT3(0, 1.5f,0), XMFLOAT3(29.794 * 0.5, 1.5f, 0.662 * 0.5), XMFLOAT4(0, 0, 0, 1));

	//	m_vecObject.push_back(box);
	//	m_vecNonAnimObjectRenderGroup.push_back(box);
	//}



	/*
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
	*/



	{
		Object* text = new Object("TextInfo");

		TransformComponent* transform = new TransformComponent(text);
		TextRendererComponent* TRC = new TextRendererComponent(text, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

		TRC->Initialize("consolas");
		TRC->SetMaterialByName("font_consolasMat");
		TRC->SetText("Consolas. Hello, World!");
		TRC->SetSize(12);
		transform->Translate(0, 1080, 0);

		m_vecObject.push_back(text);
		m_vecUIRenderGroup.push_back(text);
	}

	//{
	//	Object* text = new Object("TextAmmoInfo");

	//	TransformComponent* transform = new TransformComponent(text);
	//	TextRendererComponent* TRC = new TextRendererComponent(text, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
	//	TRC->Initialize("consolas");
	//	TRC->SetMaterialByName("font_consolasMat");
	//	TRC->SetSize(12);
	//	TextUIAmmoComponent* tuiAmmo = new TextUIAmmoComponent(text, FindObjectByName("player"));
	//	transform->Translate(800, 300, 0);

	//	m_vecObject.push_back(text);
	//	m_vecUIRenderGroup.push_back(text);
	//}
	//{
	//	Object* textCounter = new Object("textCounter");
	//	TransformComponent* transform = new TransformComponent(textCounter);
	//	TextRendererComponent* TRC = new TextRendererComponent(textCounter, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
	//	EnemyDownCounter* EDC = new EnemyDownCounter(textCounter);
	//	TRC->Initialize("consolas");
	//	TRC->SetMaterialByName("font_consolasMat");
	//	TRC->SetText("");
	//	TRC->SetSize(16);
	//	transform->Translate(0, 1040, 0);
	//	EDC->AddTarget(FindObjectByName("TB0"));
	//	EDC->AddTarget(FindObjectByName("TB1"));
	//	EDC->AddTarget(FindObjectByName("TB2"));
	//	m_vecObject.push_back(textCounter);
	//	m_vecUIRenderGroup.push_back(textCounter);
	//}
}

void ServerScene::ReloadLight()
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

Object* ServerScene::FindObjectByName(const char* strName)
{
	for (auto iter = m_vecObject.begin(); iter != m_vecObject.end(); iter++)
		if (strName == (*iter)->m_strName) return *iter;
	return nullptr;
}
