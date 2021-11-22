#include "stdafx.h"
#include "MeshRenderer.h"

MeshRenderer::MeshRenderer(
	Object* pObject,
	ID3D12Device* pd3dDevice, 
	ID3D12GraphicsCommandList* pd3dCommandList,
	D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, 
	D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle)
	:Component(pObject, "MeshRenderer")
{
	UINT ncbElementBytes = ((sizeof(XMFLOAT4X4) + 255) & ~255);

	m_pd3dCBResource = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	CreateConstantBufferView(pd3dDevice, d3dCbvCPUDescriptorStartHandle);
	SetCBVGpuHandle(d3dCbvGPUDescriptorStartHandle);
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURE_OBJECTS, m_d3dCbvGPUDescriptorHandle);
	UINT ncbElementBytes = ((sizeof(XMFLOAT4X4) + 255) & ~255);
	memset(m_pCBMappedWorldTransform, NULL, ncbElementBytes);

	//if(m_pObject)


	//if (m_pParent) {
	//	XMMATRIX xmmtxParentWorld = dynamic_cast<HumanoidObject*>(m_pParent)->GetWorldTransform();
	//	XMMATRIX xmmtxParentBoneInv = dynamic_cast<HumanoidObject*>(m_pParent)->GetBoneMatrix(28);	// 9: L Hand, 28: R Hand
	//	xmmtxParentBoneInv = XMMatrixMultiply(XMMatrixRotationRollPitchYaw(0, XMConvertToRadians(-90), XMConvertToRadians(-90)), xmmtxParentBoneInv);

	//	XMMATRIX result = XMMatrixMultiply(XMLoadFloat4x4(&m_xmf4x4Local), xmmtxParentBoneInv);
	//	result = XMMatrixMultiply(result, xmmtxParentWorld);

	//	XMStoreFloat4x4(&m_pCBMappedObjects->xmf4x4World, XMMatrixTranspose(result));
	//}
	//else			XMStoreFloat4x4(&m_pCBMappedObjects->xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Local)));

	//g_MaterialMng.SetMaterial(material.c_str(), pd3dCommandList);
	//g_ModelMng.Render(model.c_str(), pd3dCommandList);
}

void MeshRenderer::CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle)
{
	D3D12_GPU_VIRTUAL_ADDRESS		d3dGpuVirtualAddress;
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	UINT ncbElementBytes = ((sizeof(XMFLOAT4X4) + 255) & ~255);

	if (nullptr != m_pd3dCBResource) {
		m_pd3dCBResource->Map(0, NULL, (void**)&m_pCBMappedWorldTransform);
		d3dGpuVirtualAddress = m_pd3dCBResource->GetGPUVirtualAddress();
		d3dCBVDesc.SizeInBytes = ncbElementBytes;
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress;
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorStartHandle);

		d3dCbvCPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
	}
}

void MeshRenderer::SetCBVGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle)
{
	m_d3dCbvGPUDescriptorHandle			= d3dCbvGPUDescriptorStartHandle;
	d3dCbvGPUDescriptorStartHandle.ptr	+= gnCbvSrvDescriptorIncrementSize;
}
