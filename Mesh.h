#pragma once
#include "Importer.h"

class Mesh
{
public:
	Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, MESH_DATA meshData);
	~Mesh() {
		if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
		if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	}
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

	bool IsDiffuseMap() { return m_bIsDiffuseMap; }

	void SetDiffMapName(const char* name) { if (IsDiffuseMap()) assert(!"이미 디퓨즈맵이 있습니다.\n"); m_strDiffMapName = name; m_bIsDiffuseMap = true; }
	string GetDiffMapName() { if (!IsDiffuseMap()) assert(!"디퓨즈맵이 없습니다.\n"); return m_strDiffMapName; }

protected:
	ID3D12Resource*				m_pd3dVertexBuffer;
	ID3D12Resource*				m_pd3dVertexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW	m_d3dVertexBufferView;

	UINT	m_nStride;
	UINT	m_nVertices;
	string	m_strName;

	bool	m_bIsDiffuseMap = false;

	string	m_strDiffMapName;
	string	m_strNormMapName;
};

class DebugWindowMesh
{
public:
	DebugWindowMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, bool bIsPass2Screen = false);
	~DebugWindowMesh() {
		if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
		if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	}

	void Render(ID3D12GraphicsCommandList* pd3dCommandList);
protected:
	ID3D12Resource*				m_pd3dVertexBuffer;
	ID3D12Resource*				m_pd3dVertexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW	m_d3dVertexBufferView;

	UINT m_nStride;
	UINT m_nVertices;

};