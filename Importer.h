#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <DirectXMath.h>
#include "Light.h"
#include "Vertex.h"
#include "Animation.h"

using SHAPE = vector<Vertex>;

class Object;
class Material;
class MESH_DATA {
public:
	string name;
	SHAPE shape;
};
struct VertexIdx {
	int vid, vtid, vnid;
};
struct OBJECT_DESC {
	XMFLOAT3	position		= XMFLOAT3(0, 0, 0);
	XMFLOAT3	rotation		= XMFLOAT3(0, 0, 0);
	string		model			= "";
	string		material		= "defaultMaterial";
	bool		isMaterial = false;
	bool		isAnimated = false;
};

class IImporter {
protected:
	XMFLOAT3	GetFloat3(stringstream& ss);
	XMFLOAT2	GetFloat2(stringstream& ss);
	float		GetFloat(stringstream& ss);
	bool		GetBool(stringstream& ss);
	string		GetPath(stringstream& ss);
	VertexIdx GetIdx(stringstream& ss) {
		string token;
		VertexIdx output;
		char* str_buff = new char[256];
		char* tok;
		char* next_tok = NULL;
		char seps[] = " ,\t\n/";

		getline(ss, token, ' ');
		strcpy_s(str_buff, 256, token.c_str());
		tok = strtok_s(str_buff, seps, &next_tok);
		output.vid = atoi(tok) - 1;
		tok = strtok_s(next_tok, seps, &next_tok);
		output.vtid = atoi(tok) - 1;
		tok = strtok_s(next_tok, seps, &next_tok);
		output.vnid = atoi(tok) - 1;

		return output;
	}

	XMFLOAT4X4 GetMatrix(const float* fIn, int& offset) {
		int i = 0;
		XMFLOAT4X4 result;
		XMFLOAT4 r;
		XMFLOAT3 t;

		r.x = fIn[offset + i++];
		r.y = fIn[offset + i++];
		r.z = fIn[offset + i++];
		r.w = fIn[offset + i++];
		t.x = fIn[offset + i++];
		t.y = fIn[offset + i++];
		t.z = fIn[offset + i++];

		offset += i;

		XMStoreFloat4x4(&result,
				XMMatrixMultiply(
					XMMatrixRotationQuaternion(XMLoadFloat4(&r)),
					XMMatrixTranslationFromVector(XMLoadFloat3(&t))));

		return result;
	}
	Keyframe GetKeyframe(const float* fIn, int& offset) {
		Keyframe result;
		int i = 0;
		result.xmf4QuatRotation.x = fIn[offset + i++];
		result.xmf4QuatRotation.y = fIn[offset + i++];
		result.xmf4QuatRotation.z = fIn[offset + i++];
		result.xmf4QuatRotation.w = fIn[offset + i++];
		result.xmf3Translation.x = fIn[offset + i++];
		result.xmf3Translation.y = fIn[offset + i++];
		result.xmf3Translation.z = fIn[offset + i++];
		offset += i;
		return result;
	}
};


class ObjectDataImporter : public IImporter {
public:
	vector<OBJECT_DESC> Load(const char* filePath);
};
class LightDataImporter : public IImporter {
public:
	vector<LIGHT_DESC> Load(const char* filePath);
};
class MeshDataImporter : public IImporter {
public:
	vector<MESH_DATA> Load(const char* filePath);
	vector<MESH_DATA> FBXLoad(const char* filePath);
};
class MaterialDataImporter : public IImporter {
public:
	void Load(const char* filePath);
};
class AssetListDataImporter : public IImporter {
public:
	void Load(
		ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);
};
class AnimClipDataImporter : public IImporter {
public:
	AnimClip Load(const char* filePath);
};