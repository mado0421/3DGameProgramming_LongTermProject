#include "stdafx.h"
#include "Importer.h"

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

				if (token.compare("position") == 0) { cout << "  - Position :";		 temp.position = IImporter::GetFloat3(ss); }
				else if (token.compare("rotation") == 0) { cout << "  - Rotation :"; temp.rotation = IImporter::GetFloat3(ss); }
				else if (token.compare("model") == 0) { cout << "  - Model";		 temp.modelPath = IImporter::GetPath(ss); }
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