#pragma once
#include "Texture.h"

class Material
{
public:
	Material();
	void Set(ID3D12GraphicsCommandList* pd3dCommandList);

public:
	string		matName;

	string		diffuseMap;
	string		normalMap;
	string		specularMap;
	XMFLOAT3	fresnelFactor;
};

class MaterialManager {
public:
	void Initialize() { 
		m_uomMaterial.clear(); 
		prevMat = ""; 
	
		Material defaultMat;
		AddMaterial(defaultMat);
	}
	void AddMaterial(Material m) { 
		assert(!m_uomMaterial.count(m.matName.c_str()) && "�̹� �����ϴ� ���׸����� �߰��Ϸ� ��\n");
		m_uomMaterial[m.matName.c_str()] = m;
	}
	void SetMaterial(const char* matName, ID3D12GraphicsCommandList* pd3dCommandList) {
		if (prevMat.compare(matName) == 0) return;
		m_uomMaterial[matName].Set(pd3dCommandList);
	}


private:
	unordered_map<string, Material> m_uomMaterial;
	string prevMat;
};

