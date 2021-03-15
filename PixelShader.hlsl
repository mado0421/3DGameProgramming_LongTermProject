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

float4 DebugWindow(VS_OUTPUT input) : SV_TARGET {
	float4 cColor = gtxtTexture1.Sample(gSamplerState, input.uv);
	
	return cColor;
}
float4 DepthWindow(VS_OUTPUT input) : SV_TARGET {
	float depth = gtxtTexture3.Sample(gSamplerState, input.uv).r;
	depth = pow(depth, 5);

	return float4(depth.xxx, 1.0f);
}

void ShadowMap(VS_OUTPUT input) {
	
}

float4 DeferredShading(VS_OUTPUT input) : SV_TARGET {
	float3 worldPos			= WorldPosFromDepth(input.uv);
	float3 color			= gtxtTexture1.Sample(gSamplerState, input.uv).xyz;
	float3 vNormal			= gtxtTexture2.Sample(gSamplerState, input.uv).xyz * 2.0f - 1.0f;

	//float test				= gtxtDepthArray.Sample(gSamplerState, float3(input.uv, 0.0f)).x;

	//return float4(test, 0.0f, 0.0f, 1.0f);

	float3 vToCam			= normalize(gvCameraPosition - worldPos);

	//==============================================================================
	float3 result = float3(0.1f, 0.1f, 0.1f);
	for (int i = 0; i < 64; i++) {
		switch (gLightArray[i].type) {
		//case 1: result += CalcPointLight(gLightArray[i], color, worldPos, vNormal, vToCam); break;
		case 2: result += CalcSpotLight(gLightArray[i], color, worldPos, vNormal, vToCam); break;
		//case 3: result += CalcDirectionalLight(gLightArray[i], color, vNormal, vToCam);	break;
		default: break;
		}
	}
	return float4(result, 1.0f);
	//==============================================================================

}