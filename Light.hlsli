// Rf is Fresnel Factor.
// ex. water (0.02, 0.02, 0.02), gold (1.0, 0.71, 0.29)
float3 CalcSpecular(float3 Rf, float roughness, float3 cLight, float3 vToLight, float3 vNormal, float3 vToCam) {
	float3 vHalf = normalize(vToCam + vToLight);
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
float3 BlinnPhong(float3 lightColor, float3 vToLight, float3 vNormal, float3 vToCam, float3 matColor) {
	const float m = 0.1f * 256.0f;
	float3 vHalf = normalize(vToCam + vToLight);

	float roughnessFactor = (m + 8.0f) * pow(max(dot(vHalf, vNormal), 0.0f), m) / 8.0f;
	float3 fresnelFactor = SchlickFresnel(float3(0.02f, 0.02f, 0.02f), vHalf, vToLight);

	float3 specAlbedo = fresnelFactor * roughnessFactor;
	specAlbedo = specAlbedo / (specAlbedo + 1.0f);

	return (matColor + specAlbedo) * lightColor;
}

float CalcShadowFactor(Light l, float4 pos) {
	pos.xyz /= pos.w;
	float2 uv;

	uv.x = pos.x / 2.0f + 0.5f;
	uv.y = pos.y / 2.0f + 0.5f;

	if ((saturate(uv.x) == uv.x) && (saturate(uv.y) == uv.y)) {
		
		float shadowDepth = gtxtDepthArray.Sample(gShadowSamplerState, float3(uv, l.shadowIdx)).r;
		if (pos.z - 1.0f < shadowDepth) {
			return 1;
		}
	}
	return 0;
}

float3 CalcDirectionalLight(Light l, float3 matColor, float3 vNormal, float3 vToCam) {
	float3 vToLight = -l.direction;

	float ndotl = max(dot(vToLight, vNormal), 0.0f);
	float3 lightColor = l.color * ndotl;

	return BlinnPhong(lightColor, vToLight, vNormal, vToCam, matColor);
}
float3 CalcPointLight(Light l, float3 matColor, float3 pos, float3 vNormal, float3 vToCam) {
	float3 vToLight = l.position - pos;

	float d = length(vToLight);

	if (d > l.falloffEnd) return float3(0, 0, 0);

	vToLight /= d;

	float ndotl = max(dot(vToLight, vNormal), 0.0f);
	float3 lightColor = l.color * ndotl;

	float att = CalcAttenuation(d, l.falloffStart, l.falloffEnd);
	lightColor *= att;

	return BlinnPhong(lightColor, vToLight, vNormal, vToCam, matColor);
}
float3 CalcSpotLight(Light l, float3 matColor, float3 pos, float3 vNormal, float3 vToCam) {
	float3 vToLight = l.position - pos;
	float d = length(vToLight);
	if (d > l.falloffEnd) return float3(0, 0, 0);
	vToLight /= d;

	float ndotl = max(dot(vToLight, vNormal), 0.0f);
	float3 lightColor = l.color * ndotl;

	float att = CalcAttenuation(d, l.falloffStart, l.falloffEnd);
	lightColor *= att;

	float spotFactor = pow(max(dot(-vToLight, l.direction), 0.0f), l.spotPower);
	lightColor *= spotFactor;

	if (l.isShadow) {

	float shadowFactor = CalcShadowFactor(l, float4(pos, 1.0f));
	lightColor *= shadowFactor;
	}

	return BlinnPhong(lightColor, vToLight, vNormal, vToCam, matColor);
}
