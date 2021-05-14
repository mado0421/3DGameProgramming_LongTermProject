#include "Header.hlsli"
#include "Light.hlsli"

/*========================================================================
* PackGBufferPSO
*
* - position, normal, texCoord가 넘어옴.
*=======================================================================*/
float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
	// Uncompress each component from [0,1] to [-1,1].
	float3 normalT = 2.0f * normalMapSample - 1.0f;

	// Build orthonormal basis.
	float3 N = unitNormalW;
	float3 T = normalize(tangentW - dot(tangentW, N) * N);
	float3 B = cross(N, T);

	float3x3 TBN = float3x3(T, B, N);

	// Transform from tangent space to world space.
	float3 bumpedNormalW = mul(normalT, TBN);

	return bumpedNormalW;
}



GBuffer PS_PackGBuffer(VS_OUTPUT input)
{
	GBuffer output;
	input.normalW = normalize(input.normalW);
	output.cColor.rgb = gtxtColorMap.Sample(gSamplerState, input.uv).rgb;
	output.cColor.a = (1 - gtxtDepthMap.Sample(gSamplerState, input.uv).r);

	output.cNormal.rgb = NormalSampleToWorldSpace(gtxtNormalMap.Sample(gSamplerState, input.uv).rgb, input.normalW, input.tangentW) * 0.5 + 0.5;

	//output.cNormal.rgb = (gtxtNormalMap.Sample(gSamplerState, input.uv).rgb * 0.5 + 0.5).rgb;
	//output.cNormal = float4((input.normalW * 0.5 + 0.5), 1.0f);

	return output;
}

/*========================================================================
* RenderShadowPSO
*
* - Depth만 쓸 예정이므로 PS는 비워두면 됨. 알파 계산은 지금 안 할거니까.
*=======================================================================*/
void PS_RenderShadow(VS_OUTPUT input) {

}
void PS_RenderPointLightShadow(GS_OUTPUT input) {

}

/*========================================================================
* ColorFromGBufferPSO, DebugColorPSO
*
* - Color만 읽어올 예정. 그대로 뿌리면 된다.
*=======================================================================*/
float4 PS_ColorFromGBuffer(VS_OUTPUT input) : SV_TARGET{
	float3 color = gtxtColorMap.Sample(gSamplerState, input.uv).xyz;
	return float4(color, 1.0f);
}

float4 PS_ColorFromGBufferAmbient(VS_OUTPUT input) : SV_TARGET {
	float3 vWorldPosition = WorldPosFromLinearDepth(input.uv);
	float3 vColor = gtxtColorMap.Sample(gSamplerState, input.uv).xyz;
	float3 vNormal = gtxtNormalMap.Sample(gSamplerState, input.uv).xyz * 2.0f - 1.0f;
	float3 vToEye = normalize(gvCameraPosition - vWorldPosition);

	float3 color = gtxtColorMap.Sample(gSamplerState, input.uv).xyz;
	float3 vAmbientLight = float3(0.1f, 0.1f, 0.1f);
	return float4(color * vAmbientLight, 1.0f);
	 
	//float d = distance(gvCameraPosition ,vWorldPosition);
	//if (1000.0f <= d < 1020.0f) return float4(0.0f, 1.0f, 0.0f, 1.0f);
	//else if (0.006 <= d < 0.02) return float4(0.0f, 1.0f, 0.0f, 1.0f);
	//else if (0.02 <= d < 0.06) return float4(0.0f, 0.0f, 1.0f, 1.0f);
	//return float4(floor(d).xxx * 0.1f, 1.0f);

}

/*========================================================================
* DebugDepthPSO
*
* - Depth만 읽어올 예정. 그대로 뿌리면 된다.
*=======================================================================*/
float4 PS_DepthFromGBuffer(VS_OUTPUT input) : SV_TARGET{
	float depth = gtxtShadowArrayMap.Sample(gSamplerState, float3(input.uv, 0.0f)).r;
	depth = pow(depth, 5);

	return float4(depth.xxx, 1.0f);
}

/*========================================================================
* AddLightPSO
*
* - 조명값을 계산하기 위해 GBuffer의 Normal과 Depth를 읽어야 한다.
*=======================================================================*/
float4 PS_AddLight(VS_OUTPUT input) : SV_TARGET{
	float3 vWorldPosition	= WorldPosFromLinearDepth(input.uv);
	float3 vColor			= gtxtColorMap.Sample(gSamplerState, input.uv).rgb;
	float3 vNormal			= gtxtNormalMap.Sample(gSamplerState, input.uv).xyz * 2.0f - 1.0f;
	float3 vToEye			= normalize(gvCameraPosition - vWorldPosition);
	float fRoughness		= gtxtColorMap.Sample(gSamplerState, input.uv).a;


	float3 result = float3(0.0f, 0.0f, 0.0f);
	// 내용...
	switch (gLightType) {
	case 1: result += CalcPointLight(vWorldPosition, vNormal, vToEye, vColor, fRoughness); break;
	case 2: result += CalcSpotLight(vWorldPosition, vNormal, vToEye, vColor, fRoughness); break;
	case 3: result += CalcDirectionalLight(vWorldPosition, vNormal, vToEye, vColor, fRoughness);	break;
	default: break;
	}
	return float4(result, 1.0f);
}