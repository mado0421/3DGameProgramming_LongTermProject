#include "stdafx.h"
#include "Scene.h"
#include "Object.h"
#include "PipelineStateObject.h"
#include "Camera.h"
#include "RenderToTextureClass.h"
#include "Framework.h"
#include "Texture.h"
#include "Light.h"
#include "Importer.h"

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
	m_pCamera->SetPosition(XMFLOAT3(0, 0, -200));
	m_pCamera->SetLookAt(XMFLOAT3(0, 0, 0));

	CreateDescriptorHeap();
	CreatePassInfoShaderResource();


	/*========================================================================
	* Pass 1 전용 오브젝트 데이터 로드 및 생성
	*=======================================================================*/
	ObjectDataImporter objDataImporter;
	vector<OBJECT_DESC> vecObjDesc = objDataImporter.Load("Data/ObjectData.txt");

	for (int i = 0; i < vecObjDesc.size(); i++) {
		Object* tempObj = new Object(m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		
		tempObj->Move(vecObjDesc[i].position);
		m_vecObject.push_back(tempObj);
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
	* RenderToTexture 클래스 생성
	*=======================================================================*/
	m_pRTTClass = new RenderToTextureClass();
	m_pRTTClass->Init(m_pd3dDevice, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle, 2);

	m_pShadowMapRenderer = new ShadowMapRenderer();
	m_pShadowMapRenderer->Init(m_pd3dDevice, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle, 32);

	/*========================================================================
	* 일반 텍스처 로드
	*=======================================================================*/
	Texture* temp = new Texture();
	temp->LoadFromFile(m_pd3dDevice, m_pd3dCommandList, L"test.dds", m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	m_uomTextures["Test"] = temp;

	/*========================================================================
	* 광원 생성
	*=======================================================================*/
	m_lightMng = new LightManager();
	m_lightMng->Init(m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

	m_lightMng->AddDirectionalLight();
	m_lightMng->AddPointLight();
	camLightIdx = m_lightMng->AddSpotLight(XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(150, 500, -200), XMFLOAT3(0, -1, 1), XMFLOAT2(600, 700), 0.5f, true);

	/*========================================================================
	* PSO 생성
	*=======================================================================*/
	CreatePSO();
}

ID3D12RootSignature* Scene::CreateRootSignature()
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;
	D3D12_DESCRIPTOR_RANGE d3dDescriptorRange[6];

	d3dDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	d3dDescriptorRange[0].NumDescriptors = 1;
	d3dDescriptorRange[0].BaseShaderRegister = 1;
	d3dDescriptorRange[0].RegisterSpace = 0;
	d3dDescriptorRange[0].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	d3dDescriptorRange[1].NumDescriptors = 1;
	d3dDescriptorRange[1].BaseShaderRegister = 2;
	d3dDescriptorRange[1].RegisterSpace = 0;
	d3dDescriptorRange[1].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	d3dDescriptorRange[2].NumDescriptors = 1;
	d3dDescriptorRange[2].BaseShaderRegister = 3;
	d3dDescriptorRange[2].RegisterSpace = 0;
	d3dDescriptorRange[2].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	d3dDescriptorRange[3].NumDescriptors = 1;
	d3dDescriptorRange[3].BaseShaderRegister = 4;
	d3dDescriptorRange[3].RegisterSpace = 0;
	d3dDescriptorRange[3].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	d3dDescriptorRange[4].NumDescriptors = 1;
	d3dDescriptorRange[4].BaseShaderRegister = 5;
	d3dDescriptorRange[4].RegisterSpace = 0;
	d3dDescriptorRange[4].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	d3dDescriptorRange[5].NumDescriptors = 1;
	d3dDescriptorRange[5].BaseShaderRegister = 6;
	d3dDescriptorRange[5].RegisterSpace = 0;
	d3dDescriptorRange[5].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[7];

	//Camera
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; //b0
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//Objects
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[1].DescriptorTable.pDescriptorRanges = &d3dDescriptorRange[0];
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//Texture
	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &d3dDescriptorRange[1];
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//Texture2
	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[3].DescriptorTable.pDescriptorRanges = &d3dDescriptorRange[2];
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//Texture3
	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &d3dDescriptorRange[3];
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//Lights
	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[5].DescriptorTable.pDescriptorRanges = &d3dDescriptorRange[4];
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//ShadowMap TextureArray
	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &d3dDescriptorRange[5];
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

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

	d3dSamplerDesc[1].Filter = D3D12_FILTER_ANISOTROPIC;
	d3dSamplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[1].MipLODBias = 0;
	d3dSamplerDesc[1].MaxAnisotropy = 16;
	d3dSamplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	d3dSamplerDesc[1].MinLOD = 0;
	d3dSamplerDesc[1].MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc[1].ShaderRegister = 1;
	d3dSamplerDesc[1].RegisterSpace = 0;
	d3dSamplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
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

void Scene::PrevPassRender()
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
	m_pCamera->SetViewportsAndScissorRects(m_pd3dCommandList);
	UpdatePassInfoAboutCamera();

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbPassInfo->GetGPUVirtualAddress();
	m_pd3dCommandList->SetGraphicsRootConstantBufferView(ROOTSIGNATURECAMERAIDX, d3dGpuVirtualAddress);

	XMFLOAT4X4 texture = {
	0.5f,		0,		0,		0,
	0,		-0.5f,		0,		0,
	0,			0,	 1.0f,		0,
	0.5f,	 0.5f,		0,	 1.0f };
	UINT passIdx = 0;
	XMStoreFloat4x4(&m_pcbMappedPassInfo->m_xmf4x4TextureTransform, XMMatrixTranspose(XMLoadFloat4x4(&texture)));
	::memcpy(&m_pcbMappedPassInfo->m_xmf3CameraPosition, &passIdx, sizeof(UINT));




	/*========================================================================
	* Pass 1. 메쉬 렌더 To Color, Normal, Depth
	*=======================================================================*/
	//DSV의 상태를 DEPTH_WRITE에서 GENERIC_READ로 변경해줬다가 다시 DEPTH_WRITE로 변경해줄 것.
	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	
	d3dResourceBarrier.Transition.pResource = m_pRTTClass->GetDepthTexture()->GetTexture();
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	m_pRTTClass->ReadyToPrevPassRender(m_pd3dCommandList);

	d3dResourceBarrier.Transition.pResource = m_pRTTClass->GetDepthTexture()->GetTexture();
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Default"]);
	m_pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURETEXTUREIDX, m_uomTextures["Test"]->GetGpuHandle());
	for (int i = 0; i < m_vecObject.size(); i++) m_vecObject[i]->Render(m_pd3dCommandList);


	/*========================================================================
	* Pass 1. 광원 렌더
	*=======================================================================*/
	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Shadow"]);
	vector<UINT> onShadowLightIdx = m_lightMng->GetOnShadowLightIndices();
	m_pCamera->SetViewport(0, 0, 256, 256, 0.0f, 1.0f);
	m_pCamera->SetViewportsAndScissorRects(m_pd3dCommandList);

	m_lightMng->UploadLightInfoToGpu(m_pd3dCommandList);
	for (int i = 0; i < onShadowLightIdx.size(); i++) {
		/* 
		* 카메라를 광원 공간으로 옮기고
		* 이전 뷰 변환 행렬과 투영 행렬은 저장해 둔 뒤에
		* 새로 뷰 변환 행렬과 투영 행렬을 만들고
		* (투영 행렬은 바뀌는게 없음? 아마도)
		* 그걸 조명에 전달해주고
		* 카메라는 업데이트
		* 조명에선 텍스처 행렬 곱해서 올리기
		* 
		* 위의 방법이 안되는걸 일단 현상은 확인함.
		* 언제까지고 안되는걸 붙잡고 있을 수는 없으니까.
		* 광원별 뷰변환 행렬과 투영변환 행렬을 광원 초기화 시점에 생성하고(그림자를 쓴다면)
		* 그럼 광원 정보를 올려둬야 함.
		* m_lightMng->UploadLightInfoToGpu(m_pd3dCommandList);
		* 이게 업로드를 해주는거니까 이걸 여기서 해주면 그만일 듯.
		* 
		* 아니지 렌더할 때 이게 몇 번째 광원인지 알 수 있어야 하거나
		* 아니면 렌더할 때 idx번째 광원의 행렬을 미리 업로드 해둬야 쓰지.
		* 
		* 카메라의 뷰포트와 시저렉트만 다시 설정하자.
		* Zf랑 Zn 같은건 괜찮은데
		* 
		*/

		// PassInfo.m_uIdx에 지금 몇 번째 그림자 맵에 접근해야 하는지 알려줌.
		UINT shadowIdx = onShadowLightIdx[i];
		::memcpy(&m_pcbMappedPassInfo->m_uIdx, &shadowIdx, sizeof(UINT));

		m_pShadowMapRenderer->ReadyToPrevPassRender(m_pd3dCommandList, i);
		for (int i = 0; i < m_vecObject.size(); i++) m_vecObject[i]->Render(m_pd3dCommandList);
	}


}
void Scene::Render()
{
	/*========================================================================
	* 루트 시그니처, 디스크립터 힙, 메쉬 토폴로지 설정
	* 
	* 근데 PrevRender에서 했던 설정값이 저장되진 않을까?
	*=======================================================================*/
	m_pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature);				
	m_pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);			
	m_pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/*========================================================================
	* PassInfo 설정
	*=======================================================================*/
	m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	m_pCamera->SetViewportsAndScissorRects(m_pd3dCommandList);
	UpdatePassInfoAboutCamera();


	/*========================================================================
	* Pass 2. 스크린 렌더
	*=======================================================================*/
	m_lightMng->UploadLightInfoToGpu(m_pd3dCommandList);
	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Pass2"]);
	m_pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_pRTTClass->GetSRVGpuHandle(0));
	m_pd3dCommandList->SetGraphicsRootDescriptorTable(3, m_pRTTClass->GetSRVGpuHandle(1));
	m_pd3dCommandList->SetGraphicsRootDescriptorTable(4, m_pRTTClass->GetDSVGpuHandle());
	m_pd3dCommandList->SetGraphicsRootDescriptorTable(6, m_pShadowMapRenderer->GetDSVGpuHandle());
	m_vecDebugWindow[3]->Render(m_pd3dCommandList);

	/*========================================================================
	* Pass 2. Debug Window 렌더
	*=======================================================================*/
	if (test) {
		m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Debug"]);
		m_pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURETEXTUREIDX, m_pRTTClass->GetSRVGpuHandle(0));
		m_vecDebugWindow[0]->Render(m_pd3dCommandList);
		m_pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURETEXTUREIDX, m_pRTTClass->GetSRVGpuHandle(1));
		m_vecDebugWindow[1]->Render(m_pd3dCommandList);

		m_pd3dCommandList->SetGraphicsRootDescriptorTable(4, m_pRTTClass->GetDSVGpuHandle());
		m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Depth"]);
		m_vecDebugWindow[2]->Render(m_pd3dCommandList);
	}
}
void Scene::Update(float fTimeElapsed)
{
	m_pCamera->Update(fTimeElapsed);

	m_fCurrentTime += fTimeElapsed;
	::memcpy(&m_pcbMappedPassInfo->m_xmfCurrentTime, &m_fCurrentTime, sizeof(float));
}
void Scene::Input(UCHAR* pKeyBuffer, float fTimeElapsed)
{
	if (pKeyBuffer[KeyCode::_W] & 0xF0) { m_pCamera->MoveForward(200); }
	if (pKeyBuffer[KeyCode::_A] & 0xF0) { m_pCamera->MoveLeft(200); }
	if (pKeyBuffer[KeyCode::_S] & 0xF0) { m_pCamera->MoveBackward(200); }
	if (pKeyBuffer[KeyCode::_D] & 0xF0) { m_pCamera->MoveRight(200); }
	if (pKeyBuffer[KeyCode::_R] & 0xF0) { m_pCamera->MoveUp(200); }
	if (pKeyBuffer[KeyCode::_F] & 0xF0) { m_pCamera->MoveDown(200); }
	if (pKeyBuffer[KeyCode::_Q] & 0xF0) { m_pCamera->Rotate(0, -50 * fTimeElapsed, 0); }
	if (pKeyBuffer[KeyCode::_E] & 0xF0) { m_pCamera->Rotate(0, 50 * fTimeElapsed, 0); }
	if (pKeyBuffer[KeyCode::_Z] & 0xF0) { m_pCamera->Rotate(50 * fTimeElapsed, 0, 0); }
	if (pKeyBuffer[KeyCode::_X] & 0xF0) { m_pCamera->Rotate(-50 * fTimeElapsed, 0, 0); }

	if (pKeyBuffer[KeyCode::_1] & 0xF0) {
		m_pCamera->SetPosition(XMFLOAT3(0, 0, -500));
		m_pCamera->SetLookAt(XMFLOAT3(0, 0, 0));

	}
	if (pKeyBuffer[KeyCode::_2] & 0xF0) {
		m_pCamera->SetPosition(XMFLOAT3(0, 300, -500));
		m_pCamera->SetLookAt(XMFLOAT3(0, 300, 0));

	}
	if (pKeyBuffer[KeyCode::_3] & 0xF0) {
		m_pCamera->SetPosition(XMFLOAT3(0, 0, 500));
		m_pCamera->SetLookAt(XMFLOAT3(0, 0, 0));
	}
	if (pKeyBuffer[KeyCode::_4] & 0xF0) {
		m_pCamera->SetPosition(XMFLOAT3(500, 0, 0));
		m_pCamera->SetLookAt(XMFLOAT3(0, 0, 0));
	}

	if (pKeyBuffer[KeyCode::_N] & 0xF0) { test = true; }
	if (pKeyBuffer[KeyCode::_M] & 0xF0) { test = false; }



}

void Scene::CreatePSO()
{
	PipelineStateObject tempPSO			= PipelineStateObject(m_pd3dDevice, m_pd3dRootSignature);
	DebugWindowPSO tempDebugWindowPSO	= DebugWindowPSO(m_pd3dDevice, m_pd3dRootSignature);
	DepthDebugPSO tempDepthWindowPSO	= DepthDebugPSO(m_pd3dDevice, m_pd3dRootSignature);
	Pass2PSO tempPass2PSO				= Pass2PSO(m_pd3dDevice, m_pd3dRootSignature);
	ShadowPSO shadowPso					= ShadowPSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["Default"]	= tempPSO.GetPipelineState();
	m_uomPipelineStates["Debug"]	= tempDebugWindowPSO.GetPipelineState();
	m_uomPipelineStates["Depth"]	= tempDepthWindowPSO.GetPipelineState();
	m_uomPipelineStates["Pass2"]	= tempPass2PSO.GetPipelineState();
	m_uomPipelineStates["Shadow"]	= shadowPso.GetPipelineState();
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
	m_pd3dcbPassInfo->Map(0, NULL, (void**)&m_pcbMappedPassInfo);
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
