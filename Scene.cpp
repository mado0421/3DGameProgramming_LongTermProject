#include "stdafx.h"
#include "Scene.h"
#include "Object.h"
#include "PipelineStateObject.h"
#include "Camera.h"
#include "RenderToTextureClass.h"
#include "Framework.h"
#include "Texture.h"
#include "Light.h"

void Scene::Init(Framework* pFramework, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//m_pFramework		= pFramework;
	m_pd3dDevice		= pd3dDevice;
	m_pd3dCommandList	= pd3dCommandList;
	m_pd3dRootSignature = CreateRootSignature();

	// 카메라 설정
	m_pCamera = new FollowCamera();
	m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pCamera->SetPosition(XMFLOAT3(0, 0, -200));
	m_pCamera->SetLookAt(XMFLOAT3(0, 0, 0));

	// heap 생성을 하려면 미리 서술자를 몇 개나 생성할 지 알아야 하기 때문에
	// obj 개수도 중요하지만 srv에 들어갈 texture 개수도 중요함..
	// 미리 몇 개 만들지 알 수 있으면 그걸 정해두고 힙 생성을 하겠는데
	// 만약 동적으로 리소스 로드&언로드를 한다면?
	// 그럴 경우에는 힙을 미리 한도를 걸어두고 만드는 것도 방법일 수 있음
	// (예를 들어, obj는 몇 개, 투사체는 몇 개, 텍스쳐는 몇 개 제한을 둔다던지)
	// 좋다, 제한을 두자. 오브젝트와 투사체, 레벨 등을 다 합쳐서 1,024개 정도면 될 듯.
	// 텍스쳐도 128개 정도면 충분할 것.
	CreateDescriptorHeap();

	int nObject = 27;
	for (int i = 0; i < nObject; i++) {
		Object* tempObj = new Object(m_pd3dDevice, m_pd3dCommandList);
		tempObj->CreateCBV(m_pd3dDevice, m_d3dCbvCPUDescriptorStartHandle);
		tempObj->SetCbvGpuHandle(m_d3dCbvGPUDescriptorStartHandle);
		tempObj->Move(XMFLOAT3(150 * (i % 3), 150 * ((i%9)/3), 150 * (i / 9)));
		m_vecObject.push_back(tempObj);
	}
	//{
	//	Object* tempObj = new Object(m_pd3dDevice, m_pd3dCommandList);
	//	tempObj->CreateCBV(m_pd3dDevice, m_d3dCbvCPUDescriptorStartHandle);
	//	tempObj->SetCbvGpuHandle(m_d3dCbvGPUDescriptorStartHandle);
	//	tempObj->Move(XMFLOAT3(0, 0, 0));
	//	m_vecObject.push_back(tempObj);
	//}
	//{
	//	Object* tempObj = new Object(m_pd3dDevice, m_pd3dCommandList);
	//	tempObj->CreateCBV(m_pd3dDevice, m_d3dCbvCPUDescriptorStartHandle);
	//	tempObj->SetCbvGpuHandle(m_d3dCbvGPUDescriptorStartHandle);
	//	tempObj->Move(XMFLOAT3(0, 100, 0));
	//	m_vecObject.push_back(tempObj);
	//}
	//{
	//	Object* tempObj = new Object(m_pd3dDevice, m_pd3dCommandList);
	//	tempObj->CreateCBV(m_pd3dDevice, m_d3dCbvCPUDescriptorStartHandle);
	//	tempObj->SetCbvGpuHandle(m_d3dCbvGPUDescriptorStartHandle);
	//	tempObj->Move(XMFLOAT3(100, 100, 0));
	//	m_vecObject.push_back(tempObj);
	//}
	//{
	//	Object* tempObj = new Object(m_pd3dDevice, m_pd3dCommandList);
	//	tempObj->CreateCBV(m_pd3dDevice, m_d3dCbvCPUDescriptorStartHandle);
	//	tempObj->SetCbvGpuHandle(m_d3dCbvGPUDescriptorStartHandle);
	//	tempObj->Move(XMFLOAT3(100, 0, 0));
	//	m_vecObject.push_back(tempObj);
	//}
	//{
	//	Object* tempObj = new Object(m_pd3dDevice, m_pd3dCommandList);
	//	tempObj->CreateCBV(m_pd3dDevice, m_d3dCbvCPUDescriptorStartHandle);
	//	tempObj->SetCbvGpuHandle(m_d3dCbvGPUDescriptorStartHandle);
	//	tempObj->Move(XMFLOAT3(300, 300, 300));
	//	m_vecObject.push_back(tempObj);
	//}

	for (int i = 0; i < 3; i++) {
		DebugWindowObject* temp = new DebugWindowObject(m_pd3dDevice, m_pd3dCommandList);
		temp->CreateCBV(m_pd3dDevice, m_d3dCbvCPUDescriptorStartHandle);
		temp->SetCbvGpuHandle(m_d3dCbvGPUDescriptorStartHandle);

		temp->Move(XMFLOAT3(-0.5f * i, 1, 0));

		m_vecDebugWindow.push_back(temp);
	}

	DebugWindowObject* tempScreen = new DebugWindowObject(m_pd3dDevice, m_pd3dCommandList, true);
	tempScreen->CreateCBV(m_pd3dDevice, m_d3dCbvCPUDescriptorStartHandle);
	tempScreen->SetCbvGpuHandle(m_d3dCbvGPUDescriptorStartHandle);
	m_vecDebugWindow.push_back(tempScreen);



	m_pRTTClass = new RenderToTextureClass();
	m_pRTTClass->Init(m_pd3dDevice, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle, 2);

	Texture* temp = new Texture();
	temp->LoadFromFile(m_pd3dDevice, m_pd3dCommandList, L"test.dds", m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
	m_uomTextures["Test"] = temp;

	m_lightMng = new LightManager();
	m_lightMng->Init(m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

	m_lightMng->AddDirectionalLight();
	m_lightMng->AddPointLight();
	camLightIdx = m_lightMng->AddSpotLight();
	m_lightMng->SetFalloff(camLightIdx, XMFLOAT2(500.0f, 510.0f));
	m_lightMng->SetColor(camLightIdx, XMFLOAT3(1.0f, 1.0f, 0.8f));

	CreatePSO();
}

ID3D12RootSignature* Scene::CreateRootSignature()
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;
	D3D12_DESCRIPTOR_RANGE d3dDescriptorRange[5];

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

	D3D12_ROOT_PARAMETER pd3dRootParameters[6];

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

	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc;
	::ZeroMemory(&d3dSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
	d3dSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.MipLODBias = 0;
	d3dSamplerDesc.MaxAnisotropy = 1;
	d3dSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc.ShaderRegister = 0;
	d3dSamplerDesc.RegisterSpace = 0;
	d3dSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 1;
	d3dRootSignatureDesc.pStaticSamplers = &d3dSamplerDesc;
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
	//DSV의 상태를 DEPTH_WRITE에서 GENERIC_READ로 변경해줬다가 다시 DEPTH_WRITE로 변경해줄 것.
	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = m_pRTTClass->m_pDepthStencilTexture->GetTexture();
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	m_pRTTClass->ReadyToPrevPassRender(m_pd3dCommandList);

	d3dResourceBarrier.Transition.pResource = m_pRTTClass->m_pDepthStencilTexture->GetTexture();
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);


	m_pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature);
	m_pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);
	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Default"]);
	m_pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// 원래 Mesh::Render()에 있던 함수를 빼옴

	m_pCamera->SetViewportsAndScissorRects(m_pd3dCommandList);
	m_pCamera->UpdateShaderVariables(m_pd3dCommandList);

	m_pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURETEXTUREIDX, m_uomTextures["Test"]->GetGpuHandle());
	for (int i = 0; i < m_vecObject.size(); i++) m_vecObject[i]->Render(m_pd3dCommandList);
}
void Scene::Render()
{
	m_pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature);				
	m_pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);			
	m_pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// 원래 Mesh::Render()에 있던 함수를 빼옴

	m_pCamera->SetViewportsAndScissorRects(m_pd3dCommandList);
	m_pCamera->UpdateShaderVariables(m_pd3dCommandList);

	//m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Default"]);
	//for (int i = 0; i < m_vecObject.size(); i++) m_vecObject[i]->Render(m_pd3dCommandList);

	//===========================================================================================================
	// 2Pass Screen Render
	m_lightMng->UploadLightInfoToGpu(m_pd3dCommandList);
	m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Pass2"]);
	m_pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_pRTTClass->GetSRVGpuHandle(0));
	m_pd3dCommandList->SetGraphicsRootDescriptorTable(3, m_pRTTClass->GetSRVGpuHandle(1));
	m_pd3dCommandList->SetGraphicsRootDescriptorTable(4, m_pRTTClass->GetDSVGpuHandle());
	m_vecDebugWindow[3]->Render(m_pd3dCommandList);





	//===========================================================================================================
	// DebugWindow Render
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
	m_lightMng->SetPos(camLightIdx, m_pCamera->GetPosition());
	m_lightMng->SetDir(camLightIdx, m_pCamera->GetLook());
	for (int i = 0; i < m_vecObject.size(); i++) m_vecObject[i]->Rotate(XMFLOAT3(30.0f * fTimeElapsed, 30.0f * fTimeElapsed, 30.0f * fTimeElapsed));
	//for (int i = 0; i < m_vecObject.size(); i++) m_vecObject[i]->Rotate(XMFLOAT3(30.0f * fTimeElapsed, 0, 0));
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
	PipelineStateObject tempPSO = PipelineStateObject(m_pd3dDevice, m_pd3dRootSignature);
	DebugWindowPSO tempDebugWindowPSO = DebugWindowPSO(m_pd3dDevice, m_pd3dRootSignature);
	DepthDebugPSO tempDepthWindowPSO = DepthDebugPSO(m_pd3dDevice, m_pd3dRootSignature);
	Pass2PSO tempPass2PSO = Pass2PSO(m_pd3dDevice, m_pd3dRootSignature);
	m_uomPipelineStates["Default"]	= tempPSO.GetPipelineState();
	m_uomPipelineStates["Debug"]	= tempDebugWindowPSO.GetPipelineState();
	m_uomPipelineStates["Depth"]	= tempDepthWindowPSO.GetPipelineState();
	m_uomPipelineStates["Pass2"]	= tempPass2PSO.GetPipelineState();
}
void Scene::CreateDescriptorHeap() 
{
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