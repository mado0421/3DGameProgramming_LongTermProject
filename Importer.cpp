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