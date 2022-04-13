#include "stdafx.h"
#include "Object.h"
#include "Mesh.h"
#include "Model.h"
#include "Material.h"
#include "Animation.h"
#include "Mask.h"
#include "Components.h"

Object::Object()
	:m_strName("")
	,m_bEnable(true)
	,m_fTime(0.0f)
	,m_pParent(nullptr)
{
}

Object::Object(const char* strName)
	:m_strName(strName)
	, m_bEnable(true)
	, m_fTime(0.0f)
	, m_pParent(nullptr)
{
}

Object::~Object()
{
	for_each(m_vecComponents.begin(), m_vecComponents.end(), [](Component* c) { delete c; });
}

void Object::CheckCollision(Object* other)
{
	for_each(m_vecComponents.begin(), m_vecComponents.end(), [&](Component* c) {
		vector<ColliderComponent*> colliders = other->FindComponents<ColliderComponent>();
		for_each(colliders.begin(), colliders.end(), [&](ColliderComponent* collider) { c->CheckCollision(collider); });
		}
	);
}

void Object::SolveConstraint()
{
	for_each(m_vecComponents.begin(), m_vecComponents.end(), [](Component* c) { c->SolveConstraint(); });
}

void Object::Input(UCHAR* pKeyBuffer, XMFLOAT2& xmf2MouseMovement)
{
	// Input ���� �ָ� ó���ϰڴ�.
	InputManagerComponent* l_pInputMng = FindComponent<InputManagerComponent>();
	if (nullptr != l_pInputMng) l_pInputMng->InputEvent(pKeyBuffer, xmf2MouseMovement);
}

void Object::Update(float fTimeElapsed)
{
	m_fTime += fTimeElapsed;
	for_each(m_vecComponents.begin(), m_vecComponents.end(), [&](Component* c) { c->Update(fTimeElapsed); });
}

void Object::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for_each(m_vecComponents.begin(), m_vecComponents.end(), [&](Component* c) { c->Render(pd3dCommandList); });
}

void Object::SetActive(bool state)
{
	m_bEnable = state;
	for_each(m_vecComponents.begin(), m_vecComponents.end(), 
		[&](Component* c) {c->SetActive(state); });
}


void Object::AddComponent(Component* component)
{
	m_vecComponents.push_back(component);
}

Screen::Screen(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle, float width, float height)
{
	m_pScreenMesh = new Mesh(pd3dDevice, pd3dCommandList, width, height);

	UINT ncbElementBytes = ((sizeof(XMFLOAT4X4) + 255) & ~255);

	m_pd3dCBResource = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	D3D12_GPU_VIRTUAL_ADDRESS		d3dGpuVirtualAddress;
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;

	if (nullptr != m_pd3dCBResource) {
		m_pd3dCBResource->Map(0, NULL, (void**)&m_pCBMappedTransform);
		d3dGpuVirtualAddress = m_pd3dCBResource->GetGPUVirtualAddress();
		d3dCBVDesc.SizeInBytes = ncbElementBytes;
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress;
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorStartHandle);

		d3dCbvCPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
	}

	m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorStartHandle;
	d3dCbvGPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
}

Screen::~Screen()
{
	if (m_pScreenMesh) delete m_pScreenMesh;
	if (m_pd3dCBResource) {
		m_pd3dCBResource->Unmap(0, nullptr);
		m_pd3dCBResource->Release();
	}
	if (m_pCBMappedTransform) delete m_pCBMappedTransform;
}

void Screen::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURE_OBJECTS, m_d3dCbvGPUDescriptorHandle);
	UINT ncbElementBytes = ((sizeof(XMFLOAT4X4) + 255) & ~255);
	memset(m_pCBMappedTransform, NULL, ncbElementBytes);
	XMStoreFloat4x4(m_pCBMappedTransform, XMMatrixTranspose(XMMatrixIdentity()));

	m_pScreenMesh->Render(pd3dCommandList);
}
