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
	cout << xmf3Pos.x << ", " << xmf3Pos.y << ", " << xmf3Pos.z << "\n";

	return xmf3Pos;
}
XMFLOAT2 IImporter::GetFloat2(stringstream& ss)
{
	XMFLOAT2 xmf2;

	ss >> xmf2.x;
	ss >> xmf2.y;

	cout << xmf2.x << ", " << xmf2.y << "\n";

	return xmf2;
}
float IImporter::GetFloat(stringstream& ss)
{
	float f;

	ss >> f;

	cout << f << "\n";

	return f;
}
bool IImporter::GetBool(stringstream& ss) {
	bool b;

	ss >> boolalpha >> b;

	cout << b << "\n";

	return b;
}
string IImporter::GetPath(stringstream& ss)
{
	string path;

	ss >> path;
	cout << path << "\n";

	return path;
}

vector<OBJECT_DESC> ObjectDataImporter::Load(const char* filePath) {
	ifstream in(filePath);
	string s;
	string token;
	vector<OBJECT_DESC> vecObjDesc;

	if (!in.is_open()) {
		cout << "Error\n";
		assert(!"오브젝트 정보들을 읽어오는데 실패했습니다.");
		return vecObjDesc;
	}

	while (in) {
		getline(in, s);

		stringstream ss(s);
		getline(ss, token, ' ');
		if (token.compare("#Object") == 0) cout << "<Object Load Start>\n\n";
		else if (token.compare("#End") == 0) { cout << "<Object Load End>";	break; }

		if (token.compare("{") == 0) {
			OBJECT_DESC temp;
			cout << "<New Object>\n";
			while (token.compare("}")) {

				getline(ss, token, ' ');

				if (token.compare("position") == 0) { cout << "  - Position :";		temp.position = IImporter::GetFloat3(ss); }
				else if (token.compare("rotation") == 0) { cout << "  - Rotation :";temp.rotation = IImporter::GetFloat3(ss); }
				else if (token.compare("model") == 0) { 
					cout << "  - Model";		
					temp.model = IImporter::GetPath(ss);
					temp.material = IImporter::GetPath(ss);
					temp.isMaterial = true;
				}
				else if (token.compare("animModel") == 0) {
					cout << "  - animModel";
					temp.model = IImporter::GetPath(ss);
					temp.material = IImporter::GetPath(ss);
					temp.isMaterial = true;
					temp.isAnimated = true;
				}
				else if (token.compare("mesh") == 0) { cout << "  - Mesh";			temp.model = IImporter::GetPath(ss); }
			}
			vecObjDesc.push_back(temp);
			cout << "<New Object End>\n\n";
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
		cout << "Error\n";
		assert(!"라이트 정보들을 읽어오는데 실패했습니다.");
		return vecLightDesc;
	}

	while (in) {
		getline(in, s);

		stringstream ss(s);
		getline(ss, token, ' ');
		if (token.compare("#Light") == 0) cout << "<Light Load Start>\n\n";
		else if (token.compare("#End") == 0) { cout << "<Light Load End>";	break; }

		if (token.compare("{") == 0) {
			LIGHT_DESC temp;
			cout << "<New Light>\n";
			while (token.compare("}")) {

				getline(ss, token, ' ');

				if (token.compare("point") == 0) { cout << "  - PointLight\n";		temp.lightType = LIGHTTYPE::LIGHT_POINT; }
				else if (token.compare("dir") == 0) { cout << "  - DirectionalLight\n"; temp.lightType = LIGHTTYPE::LIGHT_DIRECTIONAL; }
				else if (token.compare("spot") == 0) { cout << "  - SpotLight\n";		temp.lightType = LIGHTTYPE::LIGHT_SPOT; }
				else if (token.compare("position") == 0) { cout << " Position is ";			temp.xmf3Position = IImporter::GetFloat3(ss); }
				else if (token.compare("color") == 0) { cout << " color is ";				temp.xmf3Color = IImporter::GetFloat3(ss); }
				else if (token.compare("direction") == 0) { cout << " direction is ";			temp.xmf3Direction = IImporter::GetFloat3(ss); }
				else if (token.compare("falloff") == 0) { cout << " falloff is ";			temp.xmf2Falloff = IImporter::GetFloat2(ss); }
				else if (token.compare("spotPower") == 0) { cout << " spotPower is ";			temp.fSpotPower = IImporter::GetFloat(ss); }
				else if (token.compare("shadow") == 0) { cout << " shadow is ";			temp.bIsShadow = IImporter::GetBool(ss); }
			}
			vecLightDesc.push_back(temp);
			cout << "<New Light End>\n\n";
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
		cout << "Error\n";
		assert(!"메쉬 파일 이상해!!\n");
	}

	/*
	처음 열었다!
	o가 나오면 MESH_DATA를 하나 만들고 앞으로 나오는 face는 거기에 추가한다.
	o가 또 나오면 MESH_DATA를 하나 만들고 idx로 관리하면 되겠는데?
	*/

	while (in) {
		getline(in, s);
		stringstream ss(s);

		getline(ss, token, ' ');
		if (token.compare("#") == 0) continue;
		else if (token.compare("o") == 0) { 
			cout << token << " "; 
			s.replace(0, 2, empty); 
			stringstream ss(s); 

			MESH_DATA temp;
			temp.name = GetPath(ss);
			vecMeshData.push_back(temp);
			nObject++;

			continue; 
		}
		else if (token.compare("v") == 0) { 
			cout << token << " ";
			s.replace(0, 2, empty); 
			stringstream ss(s); 
			vecControlPoint.push_back( GetFloat3(ss) );
			
			continue;
		}
		else if (token.compare("vt") == 0) { 
			cout << token << " "; 
			s.replace(0, 3, empty); 
			stringstream ss(s); 
			vecTexCoord.push_back( GetFloat2(ss) );

			continue;
		}
		else if (token.compare("vn") == 0) {
			cout << token << " "; 
			s.replace(0, 3, empty);
			stringstream ss(s); 
			vecNormal.push_back( GetFloat3(ss) );

			continue; 
		}
		else if (token.compare("s") == 0) continue;
		else if (token.compare("f") == 0) {
			cout << token << " ";
			s.replace(0, 2, empty);
			stringstream ss(s);
			VertexIdx verIdx;
			verIdx = GetIdx(ss);
			vecMeshData[nObject].shape.push_back(Vertex(vecControlPoint[verIdx.vid], vecNormal[verIdx.vnid], vecTexCoord[verIdx.vtid]));

			verIdx = GetIdx(ss);
			vecMeshData[nObject].shape.push_back(Vertex(vecControlPoint[verIdx.vid], vecNormal[verIdx.vnid], vecTexCoord[verIdx.vtid]));

			verIdx = GetIdx(ss);
			vecMeshData[nObject].shape.push_back(Vertex(vecControlPoint[verIdx.vid], vecNormal[verIdx.vnid], vecTexCoord[verIdx.vtid]));
			cout << "\n";
			continue;
		}

		cout << s << "\n";
	}

	return vecMeshData;
}

struct CtrlPoint {
	XMFLOAT3    position;
	int      boneIndices[4];
	float    weights[4];

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


			float tempyz;

			temp.m_xmf3Pos = vecCP[v.ctrlPointIndex].position;
			tempyz = temp.m_xmf3Pos.y;
			temp.m_xmf3Pos.y = temp.m_xmf3Pos.z;
			temp.m_xmf3Pos.z = tempyz;

			temp.m_xmf3Normal = v.normal;
			//temp.m_xmf3BiTangent = v.binormal;
			temp.m_xmf3Tangent = v.tangent;
			temp.m_xmf2UV = v.uv;
			temp.m_xmi4BoneIndices.x = vecCP[v.ctrlPointIndex].boneIndices[0];
			temp.m_xmi4BoneIndices.y = vecCP[v.ctrlPointIndex].boneIndices[1];
			temp.m_xmi4BoneIndices.z = vecCP[v.ctrlPointIndex].boneIndices[2];
			temp.m_xmi4BoneIndices.w = vecCP[v.ctrlPointIndex].boneIndices[3];
			temp.m_xmi4BoneWeights.x = vecCP[v.ctrlPointIndex].weights[0];
			temp.m_xmi4BoneWeights.y = vecCP[v.ctrlPointIndex].weights[1];
			temp.m_xmi4BoneWeights.z = vecCP[v.ctrlPointIndex].weights[2];
			temp.m_xmi4BoneWeights.w = vecCP[v.ctrlPointIndex].weights[3];
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
		cout << "Error\n";
		assert(!"마테리얼 정보들을 읽어오는데 실패했습니다.");
		return;
	}

	while (in) {
		getline(in, s);

		stringstream ss(s);
		getline(ss, token, ' ');
		if (token.compare("#Material") == 0) cout << "<Material Load Start>\n\n";
		else if (token.compare("#End") == 0) { cout << "<Material Load End>";	break; }

		if (token.compare("{") == 0) {
			Material temp;
			cout << "<New Material>\n";
			while (token.compare("}")) {

				getline(ss, token, ' ');

				if (token.compare("name") == 0) { cout << " Material Name is ";			temp.matName = IImporter::GetPath(ss); }
				else if (token.compare("d") == 0) { cout << " DiffuseMap is ";	temp.diffuseMap = IImporter::GetPath(ss); }
				else if (token.compare("n") == 0) { cout << " NormalMap is ";			temp.normalMap = IImporter::GetPath(ss); }
				else if (token.compare("s") == 0) { cout << " SpecularMap is ";			temp.specularMap = IImporter::GetPath(ss); }
				else if (token.compare("f") == 0) { cout << " FresnelFactor is ";				temp.fresnelFactor = IImporter::GetFloat3(ss); }
			}
			gMaterialMng.AddMaterial(temp);
			cout << "<New Material End>\n\n";
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
		assert(!"에셋 리스트 정보들을 읽어오는데 실패했습니다.");
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
			if (gTextureMng.IsAleadyExist(name.c_str())) continue;
			gTextureMng.LoadFromFile(name.c_str(), pd3dDevice, pd3dCommandList, srvCpuHandle, srvGpuHandle);
		}
		if (type.compare("obj") == 0) {
			if (gModelMng.IsAleadyExist(name.c_str())) continue;
			gModelMng.AddModel(name.c_str(), pd3dDevice, pd3dCommandList);
		}
		if (type.compare("mm") == 0) {
			if (gModelMng.IsAleadyExist(name.c_str())) continue;
			gModelMng.AddFBXModel(name.c_str(), pd3dDevice, pd3dCommandList);
		}
		if (type.compare("mac") == 0) {
			if (gAnimMng.IsAleadyExist(name.c_str())) continue;
			gAnimMng.AddAnimClip(name.c_str(), pd3dDevice, pd3dCommandList);
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

	int nBone;
	in.read((char*)&nBone, sizeof(int));
	animClip.vecBone.resize(nBone);

	int* nFrame = new int[nBone];
	in.read((char*)nFrame, sizeof(int) * nBone);

	for (int iBone = 0; iBone < nBone; ++iBone) {
		animClip.vecBone[iBone].keys.resize(nFrame[iBone]);
	}

	float clipLength = 0;
	for (int iBone = 0; iBone < nBone; iBone++) {
		int nFloat = 7 + 8 * animClip.vecBone[iBone].keys.size();
		float* fIn = new float[nFloat];
		in.read((char*)fIn, sizeof(float) * nFloat);

		int offset = 0;
		animClip.vecBone[iBone].globalMtx = IImporter::GetMatrix(fIn, offset);

		for (int iKeys = 0; iKeys < animClip.vecBone[iBone].keys.size(); iKeys++) {
			animClip.vecBone[iBone].keys[iKeys] = IImporter::GetKeyframe(fIn, offset);
			clipLength = max(animClip.vecBone[iBone].keys[iKeys].keyTime, clipLength);
		}
	}

	in.close();

	animClip.fClipLength = clipLength;

	return animClip;
}
