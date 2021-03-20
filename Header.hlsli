SamplerState gSamplerState				: register(s0);
SamplerState gShadowSamplerState		: register(s1);
cbuffer cbPassInfo						: register(b0)
{
	matrix		gmtxView				: packoffset(c0);
	matrix		gmtxProjection			: packoffset(c4);
	matrix		gmtxViewInv				: packoffset(c8);
	matrix		gmtxProjectionInv		: packoffset(c12);
	matrix		gmtxTexture				: packoffset(c16);
	float3		gvCameraPosition		: packoffset(c20);
	float		gfTime					: packoffset(c20.w);
	int			gPassIdx				: packoffset(c21);
};
cbuffer cbGameObjectInfo				: register(b1)
{
	matrix		gmtxGameObject			: packoffset(c0);
};
cbuffer cbLightArray					: register(b2) 
{
	matrix	gmtxLightSpaceVP	: packoffset(c0);
	float3	gvLightColor		: packoffset(c4);
	float	gfFalloffStart		: packoffset(c4.w);
	float3	gvLightDirection	: packoffset(c5);
	float	gfFalloffEnd		: packoffset(c5.w);
	float3	gvLightPosition		: packoffset(c6);
	float	gfSpotPower			: packoffset(c6.w);
	int		gLightType			: packoffset(c7);
	bool	bIsShadow			: packoffset(c7.y);
}
Texture2D gtxtColorMap					: register(t3);
Texture2D gtxtNormalMap					: register(t4);
Texture2D gtxtDepthMap					: register(t5);
Texture2DArray gtxtShadowMap			: register(t6);


struct VS_INPUT {
	float3 position : POSITION;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	float3 bitangent: BITANGENT;
	float2 uv		: TEXCOORD;
};
struct VS_OUTPUT {
	float4 position : SV_POSITION;
	float3 positionW: POSITION;
	float3 normalW	: NORMAL;
	float2 uv		: TEXCOORD0;
	float4 projTex	: TEXCOORD1;
};
struct GBuffer {
	float4 cColor	: SV_TARGET0;
	float4 cNormal	: SV_TARGET1;
};

float CalcWFromDepth(float depth) {
	float w = gmtxProjection._m32 / (depth - gmtxProjection._m22);
	return w;
}
float3 WorldPosFromLinearDepth(float2 uv) {
	float depth = gtxtDepthMap.Sample(gSamplerState, uv).r;
	float w = CalcWFromDepth(depth);
	float4 result = float4(uv * 2.0f - 1.0f, depth, 1);
	result.y *= -1;
	result *= w;
	result = mul(result, gmtxProjectionInv);
	result = mul(result, gmtxViewInv);
	return result.xyz;
}