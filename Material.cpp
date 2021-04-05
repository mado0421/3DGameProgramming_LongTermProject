#include "stdafx.h"
#include "Material.h"

Material::Material()
	: matName("defaultMaterial")
	, diffuseMap("defaultDiffuseMap")
	, normalMap("defaultNormalMap")
	, specularMap("defaultSpecularMap")
	, fresnelFactor(XMFLOAT3(0.1f,0.1f,0.1f))
{
}

void Material::Set(ID3D12GraphicsCommandList* pd3dCommandList)
{
	gTextureMng.UseForShaderResource(diffuseMap.c_str(), pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
	gTextureMng.UseForShaderResource(normalMap.c_str(), pd3dCommandList, ROOTSIGNATURE_NORMAL_TEXTURE);
	gTextureMng.UseForShaderResource(specularMap.c_str(), pd3dCommandList, ROOTSIGNATURE_DEPTH_TEXTURE);
}
