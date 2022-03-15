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
	virtual D3D12_SHADER_BYTECODE		CreateGeometryShader(ID3DBlob** ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE				CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);

protected:
	ID3D12PipelineState* m_pd3dPipelineState = nullptr;
};

/*========================================================================
* SRToRt PSO
*
* - 1 RTV
* - 1 DSV
* - DepthTest True
* - FrontCounterClockwise False
* - VS_PackGBuffer
* - PS_PackGBuffer
*=======================================================================*/
class SRToRtPSO : public PipelineStateObject
{
public:
	SRToRtPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
};


/*========================================================================
* PackGBuffer PSO
* 
* - 2 RTV
* - 1 DSV
* - DepthTest True
* - FrontCounterClockwise False
* - VS_PackGBuffer
* - PS_PackGBuffer
*=======================================================================*/
class PackGBufferPSO : public PipelineStateObject
{
public:
	PackGBufferPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();

	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
};

/*========================================================================
* AnimatedObject PSO
*
* - 2 RTV
* - 1 DSV
* - DepthTest True
* - FrontCounterClockwise True
* - VS_AnimatedWVP
* - PS_PackGBuffer
*=======================================================================*/
class AnimatedObjectPSO : public PipelineStateObject
{
public:
	AnimatedObjectPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();

	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
};

/*========================================================================
* RenderShadow PSO
*
* - 0 RTV
* - 1 DSV
* - DepthTest True
* - Set Bias
* - FrontCounterClockwise True
* - VS_RenderSpotLightShadowObject
* - VS_RenderSpotLightShadowAnimatedObject
* - PS_RenderShadow
*=======================================================================*/
class RenderShadowPSO : public PipelineStateObject
{
public:
	RenderShadowPSO() {}
	RenderShadowPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();

	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
};
class RenderSpotLightShadowAnimatedObjectPSO : public RenderShadowPSO
{
public:
	RenderSpotLightShadowAnimatedObjectPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
};

/*========================================================================
* RenderPointLightShadow PSO
*
* - 0 RTV
* - 1 DSV(TextureCube)
* - DepthTest True
* - Set Bias
* - FrontCounterClockwise True
* - VS_RenderPointLightShadow
* - GS_RenderPointLightShadow
* - PS_RenderPointLightShadow
*=======================================================================*/
class RenderPointLightShadowPSO : public PipelineStateObject
{
public:
	RenderPointLightShadowPSO() {}
	RenderPointLightShadowPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();

	virtual D3D12_SHADER_BYTECODE		CreateGeometryShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
};
class RenderPointLightShadowAnimatedObjectPSO : public RenderPointLightShadowPSO
{
public:
	RenderPointLightShadowAnimatedObjectPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
};

/*========================================================================
* RenderDirectionalLightShadow PSO
*
* - 0 RTV
* - 1 DSV(Texture2DArray)
* - DepthTest True
* - Set Bias
* - FrontCounterClockwise True
* - VS_RenderDirectionalLightShadow
* - GS_RenderDirectionalLightShadow
* - PS_RenderPointLightShadow
*=======================================================================*/
class RenderDirectionalShadowPSO : public PipelineStateObject
{
public:
	RenderDirectionalShadowPSO() {}
	RenderDirectionalShadowPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();

	virtual D3D12_SHADER_BYTECODE		CreateGeometryShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
};
class RenderDirectionalShadowAnimatedObjectPSO : public RenderDirectionalShadowPSO
{
public:
	RenderDirectionalShadowAnimatedObjectPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
};


/*========================================================================
* ColorFromGBuffer PSO
*
* - 1 RTV
* - 0 DSV
* - DepthTest False
* - FrontCounterClockwise True
* - VS_FlatScreen
* - PS_ColorFromGBuffer
*=======================================================================*/
class ColorFromGBufferPSO : public PipelineStateObject
{
public:
	ColorFromGBufferPSO() = default;
	ColorFromGBufferPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
};


/*========================================================================
* AddLight PSO
*
* - 1 RTV
* - 0 DSV
* - DepthTest False
* - Blend True
* - FrontCounterClockwise True
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
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
};

/*========================================================================
* DebugColor PSO
*
* - 1 RTV
* - 0 DSV
* - DepthTest False
* - FrontCounterClockwise True
* - VS_FlatScreen
* - PS_ColorFromGBuffer
*=======================================================================*/
class DebugColorPSO : public ColorFromGBufferPSO
{
public:
	DebugColorPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
};

/*========================================================================
* DebugDepth PSO
*
* - 1 RTV
* - 0 DSV
* - DepthTest False
* - FrontCounterClockwise True
* - VS_FlatScreen
* - PS_DepthFromGBuffer
*=======================================================================*/
class DebugDepthPSO : public ColorFromGBufferPSO
{
public:
	DebugDepthPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
};

/*========================================================================
* EffectPSO
*
* - 1 RTV
* - 1 DSV
* - DepthTest True(MASK ZERO, �׽�Ʈ�� �ϰ� ���� ����)
* - FrontCounterClockwise False
* ����� �׸���, ���İ��� ���� ó���Ѵٴ� �� �ܿ� �� �Ȱ��� ó��
*=======================================================================*/
class EffectPSO : public PipelineStateObject
{
public:
	EffectPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual D3D12_BLEND_DESC			CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();
};


/*========================================================================
* ParticlePSO
*
* - 1 RTV
* - 1 DSV
* - DepthTest True(MASK ZERO, �׽�Ʈ�� �ϰ� ���� ����)
* - FrontCounterClockwise TRUE
* - VS_Particle
* - GS_Partile
* - PS_Particle
*=======================================================================*/
class ParticlePSO : public PipelineStateObject
{
public:
	ParticlePSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual D3D12_BLEND_DESC			CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreateGeometryShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();
};


/*========================================================================
* TextPSO
*
* - 1 RTV
* - DepthTest False(������ ���� �׸�)
* - FrontCounterClockwise TRUE
* - VS_Text
* - GS_Text
* - PS_Text
*=======================================================================*/
class TextPSO : public PipelineStateObject
{
public:
	TextPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual D3D12_BLEND_DESC			CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreateGeometryShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();
};


/*========================================================================
* PostProcess PSOs
*=======================================================================*/
class ComputePipelineStateObject {
public:
	ComputePipelineStateObject() = default;
	ComputePipelineStateObject(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) { CreatePipelineState(pd3dDevice, pd3dRootSignature); }
	ID3D12PipelineState* GetPipelineState() { return m_pd3dPipelineState; }

protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
protected:
	virtual D3D12_SHADER_BYTECODE		CreateComputeShader(ID3DBlob** ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE				CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);

protected:
	ID3D12PipelineState* m_pd3dPipelineState = nullptr;
};

/*=============================================================================
* Blur
* - VerticalBlurCPSO
* - HorizontalBlurCPSO
*============================================================================*/
class VerticalBlurCPSO : public ComputePipelineStateObject
{
public:
	VerticalBlurCPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) 
	{ CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:	
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual D3D12_SHADER_BYTECODE		CreateComputeShader(ID3DBlob** ppd3dShaderBlob);
};
class HorizontalBlurCPSO : public ComputePipelineStateObject
{
public:
	HorizontalBlurCPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) 
	{ CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual D3D12_SHADER_BYTECODE		CreateComputeShader(ID3DBlob** ppd3dShaderBlob);
};

/*=============================================================================
* HDR ToneMapping
* - FirstPass CPSO
* - SecondPass CPSO
* - ToneMapping PSO
*============================================================================*/
class HDRFstPassCPSO : public ComputePipelineStateObject
{
public:
	HDRFstPassCPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) 
	{ CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual D3D12_SHADER_BYTECODE		CreateComputeShader(ID3DBlob** ppd3dShaderBlob);
};
class HDRScdPassCPSO : public ComputePipelineStateObject
{
public:
	HDRScdPassCPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
	{ CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual D3D12_SHADER_BYTECODE		CreateComputeShader(ID3DBlob** ppd3dShaderBlob);
};
class HDRToneMappingPSO : public PipelineStateObject
{
public:
	HDRToneMappingPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) 
	{ CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
};

/*=============================================================================
* Bloom
* - BloomCPSO
*============================================================================*/
class BloomCPSO : public ComputePipelineStateObject
{
public:
	BloomCPSO(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature) 
	{ CreatePipelineState(pd3dDevice, pd3dRootSignature); }
protected:
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

	virtual D3D12_SHADER_BYTECODE		CreateComputeShader(ID3DBlob** ppd3dShaderBlob);
};
