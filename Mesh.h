#pragma once
class Mesh
{
protected:
	ID3D12Resource*				m_pd3dVertexBuffer;
	ID3D12Resource*				m_pd3dVertexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW	m_d3dVertexBufferView;

	//ID3D12Resource*				m_pd3dIndexBuffer;
	//ID3D12Resource*				m_pd3dIndexUploadBuffer;
	//D3D12_INDEX_BUFFER_VIEW		m_d3dIndexBufferView;

	UINT m_nStride;
	UINT m_nVertices;

public:
	Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
};

class DebugWindowMesh
{
protected:
	ID3D12Resource*				m_pd3dVertexBuffer;
	ID3D12Resource*				m_pd3dVertexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW	m_d3dVertexBufferView;

	UINT m_nStride;
	UINT m_nVertices;

public:
	DebugWindowMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, bool bIsPass2Screen = false);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);
};