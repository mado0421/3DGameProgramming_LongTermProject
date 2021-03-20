#pragma once
class PipelineStateObject
{
public:
	PipelineStateObject() = default;
	PipelineStateObject(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
	ID3D12PipelineState* GetPipelineState() { return m_pd3dPipelineState; }

protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

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
	DebugWindowPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();
};
class DepthDebugPSO : public PipelineStateObject
{
public:
	DepthDebugPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();

};
class Pass2PSO : public PipelineStateObject
{
public:
	Pass2PSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();


};
class ShadowPSO : public PipelineStateObject
{
public:
	ShadowPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();
};


/*========================================================================
* PackGBuffer PSO
* 
* - 2 RTV
* - 1 DSV
* - DepthTest True
* - VS_PackGBuffer
* - PS_PackGBuffer
*=======================================================================*/
class PackGBufferPSO : public PipelineStateObject
{
public:
	PackGBufferPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
};

/*========================================================================
* RenderShadow PSO
*
* - 0 RTV
* - 1 DSV
* - DepthTest True
* - VS_RenderShadow
* - PS_RenderShadow
*=======================================================================*/
class RenderShadowPSO : public PipelineStateObject
{
public:
	RenderShadowPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
};

/*========================================================================
* ColorFromGBuffer PSO
*
* - 1 RTV
* - 0 DSV
* - DepthTest False
* - VS_FlatScreen
* - PS_ColorFromGBuffer
*=======================================================================*/
class ColorFromGBufferPSO : public PipelineStateObject
{
public:
	ColorFromGBufferPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
};

/*========================================================================
* AddLight PSO
*
* - 1 RTV
* - 0 DSV
* - DepthTest False
* - Blend True
* - VS_FlatScreen
* - PS_AddLight
*=======================================================================*/
class AddLightPSO : public PipelineStateObject
{
public:
	AddLightPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual D3D12_BLEND_DESC			CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
};





