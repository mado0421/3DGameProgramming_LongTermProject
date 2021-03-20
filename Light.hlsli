// Rf is Fresnel Factor.
// ex. water (0.02, 0.02, 0.02), gold (1.0, 0.71, 0.29)
float3 CalcSpecular(float3 Rf, float roughness, float3 cLight, float3 vToLight, float3 vNormal, float3 vToEye) {
	float3 vHalf = normalize(vToEye + vToLight);
	float3 Specular = max(dot(vToLight, vNormal), 0) * cLight;
	// Fresnel 계산 공식에서 dot(vNormal, vHalf) 값이 음수가 나올 수 있기 때문에 max를 넣어줬음.
	float3 Fresnel = Rf * ((roughness + 8) / 8) * pow(max(dot(vNormal, vHalf), 0), roughness);
	return Specular * Fresnel;
}
float3 CalcDiffuse(float3 cLight, float3 cDiff, float3 vToLight, float3 vNormal) {
	return max(dot(vToLight, vNormal), 0) * cLight * cDiff;
}
float3 CalcAmbient(float3 cLight, float3 cDiff) {
	return cLight * cDiff;
}

float CalcAttenuation(float d, float falloffStart, float falloffEnd) {
	return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}
float3 SchlickFresnel(float3 R0, float3 vNormal, float3 vLight) {
	float cosIncidentAngle = saturate(dot(vNormal, vLight));
	float f0 = 1.0f - cosIncidentAngle;
	float3 reflectPercent = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0 * f0);

	return reflectPercent;
}
float3 BlinnPhong(float3 lightColor, float3 vToLight, float3 vNormal, float3 vToEye) {
	const float m = 0.1f * 256.0f;
	float3 vHalf = normalize(vToEye + vToLight);

	float roughnessFactor = (m + 8.0f) * pow(max(dot(vHalf, vNormal), 0.0f), m) / 8.0f;
	float3 fresnelFactor = SchlickFresnel(float3(0.02f, 0.02f, 0.02f), vHalf, vToLight);

	float3 specAlbedo = fresnelFactor * roughnessFactor;
	specAlbedo = specAlbedo / (specAlbedo + 1.0f);

	return specAlbedo * lightColor;
}

float CalcShadowFactor(float3 worldPos) {

	float4 temp = mul(float4(worldPos, 1.0f), gmtxLightSpaceVP);
	temp /= temp.w;
	temp = mul(temp, gmtxTexture);
	float depth = gtxtShadowMap.Sample(gShadowSamplerState, temp.xy).r;
	if (temp.z < depth) return 1.0f;
	return 0.0f;
}

float3 CalcDirectionalLight(float3 vNormal, float3 vToEye) {
	float3 vToLight = -gvLightDirection;

	float ndotl = max(dot(vToLight, vNormal), 0.0f);
	float3 lightColor = gvLightColor * ndotl;

	return BlinnPhong(lightColor, vToLight, vNormal, vToEye);
}
float3 CalcPointLight(float3 vPos, float3 vNormal, float3 vToEye) {
	float3 vToLight = gvLightPosition - vPos;

	float d = length(vToLight);

	if (d > gfFalloffEnd) return float3(0, 0, 0);

	vToLight /= d;

	float ndotl = max(dot(vToLight, vNormal), 0.0f);
	float3 lightColor = gvLightColor * ndotl;

	float att = CalcAttenuation(d, gfFalloffStart, gfFalloffEnd);
	lightColor *= att;

	return BlinnPhong(lightColor, vToLight, vNormal, vToEye);
}
float3 CalcSpotLight(float3 vPos, float3 vNormal, float3 vToEye) {
	float3 vToLight = gvLightPosition - vPos;
	float d = length(vToLight);
	if (d > gfFalloffEnd) return float3(0, 0, 0);
	vToLight /= d;

	float ndotl = max(dot(vToLight, vNormal), 0.0f);
	float3 lightColor = gvLightColor * ndotl;

	float att = CalcAttenuation(d, gfFalloffStart, gfFalloffEnd);
	lightColor *= att;

	float spotFactor = pow(max(dot(-vToLight, gvLightDirection), 0.0f), gfSpotPower);
	lightColor *= spotFactor;

	if (gbIsShadow) {
		float shadowFactor = CalcShadowFactor(vPos);
		lightColor *= shadowFactor;
	}

	return lightColor;
	//return BlinnPhong(lightColor, vToLight, vNormal, vToEye);
}
