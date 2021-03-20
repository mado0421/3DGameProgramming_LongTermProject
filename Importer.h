#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <DirectXMath.h>
#include "Light.h"

class Object;

struct OBJECT_DESC {
	XMFLOAT3	position		= XMFLOAT3(0, 0, 0);
	XMFLOAT3	rotation		= XMFLOAT3(0, 0, 0);
	string		modelPath;
};

class IImporter {
protected:
	XMFLOAT3	GetFloat3(stringstream& ss);
	XMFLOAT2	GetFloat2(stringstream& ss);
	float		GetFloat(stringstream& ss);
	bool		GetBool(stringstream& ss);
	string		GetPath(stringstream& ss);
};

class ObjectDataImporter : public IImporter {
public:
	vector<OBJECT_DESC> Load(const char* filePath);
};
class LightDataImporter : public IImporter {
public:
	vector<LIGHT_DESC> Load(const char* filePath);
};

