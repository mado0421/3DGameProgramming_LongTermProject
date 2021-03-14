#include "Header.hlsli"
#include "Light.hlsli"

TEST main(VS_OUTPUT input)
{
	TEST output;
	input.normalW	= normalize(input.normalW);
	output.cColor	= gtxtTexture1.Sample(gSamplerState, input.uv);
	output.cNormal	= float4((input.normalW * 0.5 + 0.5), 1.0f);

	return output;
}

/*
* PSO에서 깊이값 기록만 하고 실제로 쓰진 못하게 해야 할 듯.
* 
* 
*/
//void Depth(VS_OUTPUT input) : SV_TARGET {
//
//}

float4 DebugWindow(VS_OUTPUT input) : SV_TARGET {
	float4 cColor = gtxtTexture1.Sample(gSamplerState, input.uv);
	
	return cColor;
}
float4 DepthWindow(VS_OUTPUT input) : SV_TARGET {
	float depth = gtxtTexture3.Sample(gSamplerState, input.uv).r;
	depth = pow(depth, 5);

	return float4(depth.xxx, 1.0f);
}

float4 DeferredShading(VS_OUTPUT input) : SV_TARGET {
	float3 worldPos			= WorldPosFromDepth(input.uv);
	float3 color			= gtxtTexture1.Sample(gSamplerState, input.uv).xyz;
	float3 vNormal			= gtxtTexture2.Sample(gSamplerState, input.uv).xyz * 2.0f - 1.0f;

	float3 vToCam	= normalize(gvCameraPosition - worldPos);

	//==============================================================================
	//float3 cLight = float3(0.5f, 0.5f, 0.1f);
	//float3 cAmbi = float3(0.1f, 0.1f, 0.1f);
	//float3 vToLight = float3(0, 1.0f, 0);
	//float3 Rf = float3(0.02f, 0.02f, 0.02f);
	//float roughness = 64;
	//float3 diff = CalcDiffuse(cLight, color, vToLight, normal);
	//float3 ambi = CalcAmbient(cAmbi, color);
	//float3 spec = CalcSpecular(Rf, roughness, color, vToLight, normal, vToCam);
	//return float4(diff + ambi + spec, 1.0f);
	//return float4(spec, 1.0f);
	//==============================================================================

	//==============================================================================
	float3 result = float3(0.1f,0.1f,0.1f);
	for (int i = 0; i < 64; i++) {
		switch (gLightArray[i].type) {
		case 1: result += CalcPointLight(gLightArray[i], color, worldPos, vNormal, vToCam); break;
		case 2: result += CalcSpotLight(gLightArray[i], color, worldPos, vNormal, vToCam); break;
		case 3: result += CalcDirectionalLight(gLightArray[i], color, vNormal, vToCam);	break;
		default: break;
		}
	}
	return float4(result, 1.0f);
	//==============================================================================

}