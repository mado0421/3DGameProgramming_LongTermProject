/*==============================================================
* 여기서 선언을 먼저 해줘야 함.
* Light 구조체를 cbuffer 아래서 선언하면 안 됨.
* 
* ============================================================*/
struct Light {
	matrix	mtxLightSpaceVP;
	float3	color;
	float	falloffStart;
	float3	direction;
	float	falloffEnd;
	float3	position;
	float	spotPower;
	int		type;
	int		shadowIdx;
	bool	isEnable;
	bool	isShadow;
};

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
Texture2D gtxtTexture1					: register(t2);
Texture2D gtxtTexture2					: register(t3);
Texture2D gtxtTexture3					: register(t4);
cbuffer cbLightArray					: register(b5) 
{
	Light gLightArray[64] : packoffset(c0);
}
Texture2DArray gtxtDepthArray			: register(t6);



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
struct TEST {
	float4 cColor	: SV_TARGET0;
	float4 cNormal	: SV_TARGET1;
};

float CalcWFromDepth(float depth) {
	float w = gmtxProjection._m32 / (depth - gmtxProjection._m22);
	return w;
}
float3 WorldPosFromLinearDepth(float2 uv) {
	float depth = gtxtTexture3.Sample(gSamplerState, uv).r;
	float w = CalcWFromDepth(depth);
	float4 result = float4(uv * 2.0f - 1.0f, depth, 1);
	result.y *= -1;
	result *= w;
	result = mul(result, gmtxProjectionInv);
	result = mul(result, gmtxViewInv);
	return result.xyz;
}