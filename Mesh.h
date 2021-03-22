#pragma once
#include "Importer.h"

class Mesh
{
public:
	Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, MESH_DATA meshData);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
protected:
	ID3D12Resource*				m_pd3dVertexBuffer;
	ID3D12Resource*				m_pd3dVertexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW	m_d3dVertexBufferView;

	UINT	m_nStride;
	UINT	m_nVertices;
	string	m_strName;
};

class DebugWindowMesh
{
public:
	DebugWindowMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, bool bIsPass2Screen = false);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);
protected:
	ID3D12Resource*				m_pd3dVertexBuffer;
	ID3D12Resource*				m_pd3dVertexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW	m_d3dVertexBufferView;

	UINT m_nStride;
	UINT m_nVertices;

};