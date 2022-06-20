#include "stdafx.h"
#include "Importer.h"
#include "Material.h"
#include "Model.h"
#include "Animation.h"


XMFLOAT3 IImporter::GetFloat3(stringstream& ss)
{
	XMFLOAT3 xmf3Pos;

	ss >> xmf3Pos.x;
	ss >> xmf3Pos.y;
	ss >> xmf3Pos.z;

	return xmf3Pos;
}
XMFLOAT2 IImporter::GetFloat2(stringstream& ss)
{
	XMFLOAT2 xmf2;

	ss >> xmf2.x;
	ss >> xmf2.y;


	return xmf2;
}
float IImporter::GetFloat(stringstream& ss)
{
	float f;

	ss >> f;


	return f;
}
bool IImporter::GetBool(stringstream& ss) {
	bool b;

	ss >> boolalpha >> b;


	return b;
}
string IImporter::GetPath(stringstream& ss)
{
	string path;

	ss >> path;

	return path;
}

VertexIdx IImporter::GetIdx(stringstream& ss)
{
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

XMFLOAT4X4 IImporter::GetMatrix(const float* fIn, int& offset)
{
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

Keyframe IImporter::GetKeyframe(const float* fIn, int& offset)
{
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

vector<OBJECT_DESC> ObjectDataImporter::Load(const char* filePath) {
	ifstream in(filePath);
	string s;
	string token;
	vector<OBJECT_DESC> vecObjDesc;

	if (!in.is_open()) {
		assert(!"������Ʈ �������� �о���µ� �����߽��ϴ�.");
		return vecObjDesc;
	}

	while (in) {
		getline(in, s);

		stringstream ss(s);
		getline(ss, token, ' ');
		if (token.compare("#End") == 0) break;

		if (token.compare("{") == 0) {
			OBJECT_DESC temp;
			while (token.compare("}")) {

				getline(ss, token, ' ');

				if (token.compare("position") == 0) temp.position = IImporter::GetFloat3(ss); 
				else if (token.compare("rotation") == 0) temp.rotation = IImporter::GetFloat3(ss); 
				else if (token.compare("model") == 0) { 
					temp.model = IImporter::GetPath(ss);
					temp.material = IImporter::GetPath(ss);
					temp.isMaterial = true;
				}
				else if (token.compare("animModel") == 0) {
					temp.model = IImporter::GetPath(ss);
					temp.material = IImporter::GetPath(ss);
					temp.isMaterial = true;
					temp.isAnimated = true;
				}
				else if (token.compare("mesh") == 0) temp.model = IImporter::GetPath(ss); 
			}
			vecObjDesc.push_back(temp);
		}
	}

	return vecObjDesc;
}

vector<LIGHT_DESC> LightDataImporter::Load(const char* filePath) {
	ifstream in(filePath);
	string s;
	string token;
	vector<LIGHT_DESC> vecLightDesc;

	if (!in.is_open()) {
		assert(!"����Ʈ �������� �о���µ� �����߽��ϴ�.");
		return vecLightDesc;
	}

	while (in) {
		getline(in, s);

		stringstream ss(s);
		getline(ss, token, ' ');
		if (token.compare("#End") == 0) break; 

		if (token.compare("{") == 0) {
			LIGHT_DESC temp;
			while (token.compare("}")) {

				getline(ss, token, ' ');

				if (token.compare("point") == 0) {			temp.lightType = LIGHTTYPE::LIGHT_POINT; }
				else if (token.compare("dir") == 0) {		temp.lightType = LIGHTTYPE::LIGHT_DIRECTIONAL; }
				else if (token.compare("spot") == 0) {		temp.lightType = LIGHTTYPE::LIGHT_SPOT; }
				else if (token.compare("position") == 0) {	temp.xmf3Position = IImporter::GetFloat3(ss); }
				else if (token.compare("color") == 0) {		temp.xmf3Color = IImporter::GetFloat3(ss); }
				else if (token.compare("direction") == 0) { temp.xmf3Direction = IImporter::GetFloat3(ss); }
				else if (token.compare("falloff") == 0) {	temp.xmf2Falloff = IImporter::GetFloat2(ss); }
				else if (token.compare("spotPower") == 0) { temp.fSpotPower = IImporter::GetFloat(ss); }
				else if (token.compare("shadow") == 0) {	temp.bIsShadow = IImporter::GetBool(ss); }
			}
			vecLightDesc.push_back(temp);
		}
	}

	return vecLightDesc;
}

vector<MESH_DATA> MeshDataImporter::Load(const char* filePath)
{
	vector<MESH_DATA> vecMeshData;
	vector<XMFLOAT3> vecControlPoint;
	vector<XMFLOAT3> vecNormal;
	vector<XMFLOAT2> vecTexCoord;

	string ultimateOfPerfactFilePath;
	string fileHead = "Assets/";
	string fileTail = ".obj";
	
	ultimateOfPerfactFilePath = fileHead + filePath;
	ultimateOfPerfactFilePath += fileTail;

	int nObject = -1;

	ifstream in(ultimateOfPerfactFilePath);
	string s;
	string token;
	string empty("");
	if (!in.is_open()) {
		assert(!"�޽� ���� �̻���!!\n");
	}

	/*
	ó�� ������!
	o�� ������ MESH_DATA�� �ϳ� ����� ������ ������ face�� �ű⿡ �߰��Ѵ�.
	o�� �� ������ MESH_DATA�� �ϳ� ����� idx�� �����ϸ� �ǰڴµ�?
	*/

	while (in) {
		getline(in, s);
		stringstream ss(s);

		getline(ss, token, ' ');
		if (token.compare("#") == 0) continue;
		else if (token.compare("o") == 0) { 
			s.replace(0, 2, empty); 
			stringstream ss(s); 

			MESH_DATA temp;
			temp.name = GetPath(ss);
			vecMeshData.push_back(temp);
			nObject++;

			continue; 
		}
		else if (token.compare("v") == 0) { 
			s.replace(0, 2, empty); 
			stringstream ss(s); 
			vecControlPoint.push_back( GetFloat3(ss) );
			
			continue;
		}
		else if (token.compare("vt") == 0) { 
			s.replace(0, 3, empty); 
			stringstream ss(s); 
			vecTexCoord.push_back( GetFloat2(ss) );

			continue;
		}
		else if (token.compare("vn") == 0) {
			s.replace(0, 3, empty);
			stringstream ss(s); 
			vecNormal.push_back( GetFloat3(ss) );

			continue; 
		}
		else if (token.compare("s") == 0) continue;
		else if (token.compare("f") == 0) {
			s.replace(0, 2, empty);
			stringstream ss(s);
			VertexIdx verIdx;
			verIdx = GetIdx(ss);
			vecMeshData[nObject].shape.push_back(Vertex(vecControlPoint[verIdx.vid], vecNormal[verIdx.vnid], vecTexCoord[verIdx.vtid]));

			verIdx = GetIdx(ss);
			vecMeshData[nObject].shape.push_back(Vertex(vecControlPoint[verIdx.vid], vecNormal[verIdx.vnid], vecTexCoord[verIdx.vtid]));

			verIdx = GetIdx(ss);
			vecMeshData[nObject].shape.push_back(Vertex(vecControlPoint[verIdx.vid], vecNormal[verIdx.vnid], vecTexCoord[verIdx.vtid]));
			continue;
		}

	}

	return vecMeshData;
}

struct CtrlPoint {
	XMFLOAT3		position;
	unsigned int    boneIndices[4];
	double			weights[4];

	CtrlPoint()
		: position(XMFLOAT3(0, 0, 0))
	{
		int i = 0;
		boneIndices[i++] = 0;
		boneIndices[i++] = 0;
		boneIndices[i++] = 0;
		boneIndices[i++] = 0;
		i = 0;
		weights[i++] = 0;
		weights[i++] = 0;
		weights[i++] = 0;
		weights[i++] = 0;
	}
};
struct VertexForImport {
	UINT ctrlPointIndex;
	XMFLOAT3 normal;
	XMFLOAT3 binormal;
	XMFLOAT3 tangent;
	XMFLOAT2 uv;

	VertexForImport()
		: ctrlPointIndex(0)
		, normal(XMFLOAT3(0, 0, 0))
		, binormal(XMFLOAT3(0, 0, 0))
		, tangent(XMFLOAT3(0, 0, 0))
		, uv(XMFLOAT2(0, 0))
	{}
};

vector<MESH_DATA> MeshDataImporter::FBXLoad(const char* filePath)
{
	vector<MESH_DATA> vecMeshData;

	string ultimateOfPerfectFilePath;

	string fileHead = "Assets/";
	string fileTail = ".mm";   // my mesh

	ultimateOfPerfectFilePath = fileHead + filePath;
	ultimateOfPerfectFilePath += fileTail;

	ifstream in;
	in.open(ultimateOfPerfectFilePath, ios::in | ios::binary);

	int nMesh;
	in.read((char*)&nMesh, sizeof(int));

	for (int iMesh = 0; iMesh < nMesh; iMesh++) {
		string name = "fbxMesh.";
		string num = to_string(iMesh);
		
		MESH_DATA tempMesh;
		tempMesh.name = name + num;
		
		int nCtrlPoint;
		in.read((char*)&nCtrlPoint, sizeof(int));

		CtrlPoint* pCtrlPoint = new CtrlPoint[nCtrlPoint];
		in.read((char*)pCtrlPoint, sizeof(CtrlPoint) * nCtrlPoint);
		vector<CtrlPoint> vecCP;

		for (int iCP = 0; iCP < nCtrlPoint; ++iCP) {
			CtrlPoint temp;
			temp = pCtrlPoint[iCP];
			vecCP.push_back(temp);
		}

		int nVertex;
		in.read((char*)&nVertex, sizeof(int));
		vector<Vertex> vecVertex;

		VertexForImport* pVertex = new VertexForImport[nVertex];
		in.read((char*)pVertex, sizeof(VertexForImport) * nVertex);

		for (int iV = 0; iV < nVertex; ++iV) {
			VertexForImport v;
			v = pVertex[iV];
			Vertex temp;

			temp.m_xmf3Pos				= vecCP[v.ctrlPointIndex].position;
			temp.m_xmf3Normal			= v.normal;
			temp.m_xmf3Tangent			= v.tangent;
			temp.m_xmf2UV				= v.uv;
			temp.m_xmi4BoneIndices.x	= vecCP[v.ctrlPointIndex].boneIndices[0];
			temp.m_xmi4BoneIndices.y	= vecCP[v.ctrlPointIndex].boneIndices[1];
			temp.m_xmi4BoneIndices.z	= vecCP[v.ctrlPointIndex].boneIndices[2];
			temp.m_xmi4BoneIndices.w	= vecCP[v.ctrlPointIndex].boneIndices[3];
			temp.m_xmi4BoneWeights.x	= vecCP[v.ctrlPointIndex].weights[0];
			temp.m_xmi4BoneWeights.y	= vecCP[v.ctrlPointIndex].weights[1];
			temp.m_xmi4BoneWeights.z	= vecCP[v.ctrlPointIndex].weights[2];
			temp.m_xmi4BoneWeights.w	= vecCP[v.ctrlPointIndex].weights[3];
			tempMesh.shape.push_back(temp);
		}

		vecMeshData.push_back(tempMesh);
	}

	in.close();

	return vecMeshData;
}

void MaterialDataImporter::Load(const char* filePath)
{
	ifstream in(filePath);
	string s;
	string token;

	if (!in.is_open()) {
		assert(!"���׸��� �������� �о���µ� �����߽��ϴ�.");
		return;
	}

	while (in) {
		getline(in, s);

		stringstream ss(s);
		getline(ss, token, ' ');
		if (token.compare("#End") == 0) break;

		if (token.compare("{") == 0) {
			Material temp;
			while (token.compare("}")) {

				getline(ss, token, ' ');

				if (token.compare("name") == 0) {	temp.matName = IImporter::GetPath(ss); }
				else if (token.compare("d") == 0) { temp.diffuseMap = IImporter::GetPath(ss); }
				else if (token.compare("n") == 0) { temp.normalMap = IImporter::GetPath(ss); }
				else if (token.compare("s") == 0) { temp.specularMap = IImporter::GetPath(ss); }
				else if (token.compare("f") == 0) { temp.fresnelFactor = IImporter::GetFloat3(ss); }
			}
			g_MaterialMng.AddMaterial(temp);
		}
	}


}

void AssetListDataImporter::Load(
	ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
	D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle)
{
	ifstream in("Data/AssetsData.txt");
	string s;

	string empty("");
	if (!in.is_open()) {
		assert(!"���� ����Ʈ �������� �о���µ� �����߽��ϴ�.");
		return;
	}

	while (in) {
		string name;
		string type;
		getline(in, s);
		stringstream ss(s);

		getline(ss, name, '.');
		getline(ss, type, '\n');
		if (type.compare("dds") == 0) {
			if (g_TextureMng.IsAleadyExist(name.c_str())) continue;
			g_TextureMng.LoadFromFile(name.c_str(), pd3dDevice, pd3dCommandList, srvCpuHandle, srvGpuHandle);
		}
		if (type.compare("obj") == 0) {
			if (g_ModelMng.IsAleadyExist(name.c_str())) continue;
			g_ModelMng.AddModel(name.c_str(), pd3dDevice, pd3dCommandList);
		}
		if (type.compare("mm") == 0) {
			if (g_ModelMng.IsAleadyExist(name.c_str())) continue;
			g_ModelMng.AddFBXModel(name.c_str(), pd3dDevice, pd3dCommandList);
		}
		if (type.compare("mac") == 0) {
			if (g_AnimMng.IsAleadyExist(name.c_str())) continue;
			g_AnimMng.AddAnimClip(name.c_str(), pd3dDevice, pd3dCommandList);
		}
	}
	return;
}
struct TransformForImport {
	float RotationTranslation[8];
};
AnimClip AnimClipDataImporter::Load(const char* filePath)
{
	AnimClip animClip;

	animClip.strClipName = filePath;

	string ultimateOfPerfectFilePath;

	string fileHead = "Assets/";
	string fileTail = ".mac";   // my animation clip

	ultimateOfPerfectFilePath = fileHead + filePath;
	ultimateOfPerfectFilePath += fileTail;

	ifstream in;
	in.open(ultimateOfPerfectFilePath, ios::in | ios::binary);

	int nBone, nKeys;
	double* pKeytimes;
	in.read((char*)&nBone, sizeof(int));
	in.read((char*)&nKeys, sizeof(int));

	pKeytimes = new double[nKeys];
	in.read((char*)pKeytimes, sizeof(double) * nKeys);

	for (int i = 0; i < nKeys; i++) animClip.vecTimes.push_back(pKeytimes[i]);
	animClip.fClipLength = animClip.vecTimes.back();

	animClip.vecBone.resize(nBone);

	for (int iBone = 0; iBone < nBone; iBone++) {
		// toDresspose + toWorld * nKey
		//int nFloat = 7 * (nKeys + 1);
		//float* fIn = new float[nFloat];
		//in.read((char*)fIn, sizeof(float) * nFloat);
		//int offset = 0;
		//animClip.vecBone[iBone].toDressposeInv = IImporter::GetMatrix(fIn, offset);
		//for (int iKeys = 0; iKeys < nKeys; iKeys++) {
		//	animClip.vecBone[iBone].keys.push_back(IImporter::GetKeyframe(fIn, offset));
		//}

		// toDresspose + toParent + local * nKey
		in.read((char*)&animClip.vecBone[iBone].parentIdx, sizeof(int));

		int nFloat = 7 * (nKeys + 2);
		float* fIn = new float[nFloat];
		in.read((char*)fIn, sizeof(float) * nFloat);
		int offset = 0;
		animClip.vecBone[iBone].toDressposeInv = IImporter::GetMatrix(fIn, offset);
		animClip.vecBone[iBone].toParent = IImporter::GetMatrix(fIn, offset);
		for (int iKeys = 0; iKeys < nKeys; iKeys++) {
			animClip.vecBone[iBone].keys.push_back(IImporter::GetKeyframe(fIn, offset));
		}
	}

	in.close();

	return animClip;
}
