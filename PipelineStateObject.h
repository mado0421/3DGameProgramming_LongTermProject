#pragma once
class PipelineStateObject
{
public:
	PipelineStateObject() = default;
	PipelineStateObject(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
	ID3D12PipelineState* GetPipelineState() { return m_pd3dPipelineState; }

protected:
	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();
	virtual D3D12_BLEND_DESC			CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreateComputeShader(ID3DBlob** ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE				CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);

protected:
	ID3D12PipelineState* m_pd3dPipelineState = nullptr;
};

class DebugWindowPSO : public PipelineStateObject
{
public:
	DebugWindowPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
protected:
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();
};

class DepthDebugPSO : public PipelineStateObject
{
public:
	DepthDebugPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
protected:
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();

};

class Pass2PSO : public PipelineStateObject
{
public:
	Pass2PSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
protected:
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();


};

class ShadowPSO : public PipelineStateObject
{
public:
	ShadowPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
protected:
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();


};