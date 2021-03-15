#include "Header.hlsli"

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	output.positionW	= (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position		= mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW		= mul(input.normal, (float3x3)gmtxGameObject);
	output.uv			= input.uv;

	return output;
}
VS_OUTPUT DebugWindow(VS_INPUT input)
{
	VS_OUTPUT output;

	output.positionW	= (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position		= float4(output.positionW, 1.0f);
	output.uv			= input.uv;

	return output;
}
VS_OUTPUT Shadow(VS_INPUT input)
{
	VS_OUTPUT output;

	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.projTex = mul(output.position, gmtxTexture);

	return output;
}