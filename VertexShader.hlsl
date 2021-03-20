#include "Header.hlsli"

/*========================================================================
* PackGBufferPSO
*
* - 평범하게 월드 변환, 뷰 변환, 투영 변환 행렬곱
* - position, normal, texCoord
*=======================================================================*/
VS_OUTPUT VS_PackGBuffer(VS_INPUT input) {
	VS_OUTPUT output;

	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.uv = input.uv;

	return output;
}

/*========================================================================
* RenderShadowPSO
*
* - 월드 변환 후에 조명공간의 뷰 변환, 투영 변환 행렬을 곱
* - position 만 넘겨주면 됨. 깊이를 쓸거니까.
*=======================================================================*/
VS_OUTPUT VS_RenderShadow(VS_INPUT input)
{
	VS_OUTPUT output;

	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(float4(output.positionW, 1.0f), gmtxLightSpaceVP);

	return output;
}

/*========================================================================
* ColorFromGBufferPSO, AddLightPSO
*
* - 평면 스크린을 그릴 예정. 월드 변환만 진행. 값은 메쉬 생성때 미리 저장.
* - position, uv
*=======================================================================*/
VS_OUTPUT VS_FlatScreen(VS_INPUT input)
{
	VS_OUTPUT output;

	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = float4(output.positionW, 1.0f);
	output.uv = input.uv;

	return output;
}
