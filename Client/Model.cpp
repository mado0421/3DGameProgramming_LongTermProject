#include "stdafx.h"
#include "Model.h"
#include "Importer.h"
#include "Mesh.h"

void ModelManager::Initialize()
{
	m_uomModel.clear();
}

void ModelManager::AddModel(const char* fileName, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	MeshDataImporter meshDataImporter;
	vector<MESH_DATA> vecMeshData = meshDataImporter.Load(fileName);
	Model model;

	for (int i = 0; i < vecMeshData.size(); i++) {
		Mesh* pMesh = new Mesh(pd3dDevice, pd3dCommandList, vecMeshData[i]);
		model.push_back(pMesh);
	}

	m_uomModel[fileName] = model;
}

void ModelManager::AddFBXModel(const char* fileName, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	MeshDataImporter meshDataImporter;
	vector<MESH_DATA> vecMeshData = meshDataImporter.FBXLoad(fileName);
	Model model;

	for (int i = 0; i < vecMeshData.size(); i++) {
		Mesh* pMesh = new Mesh(pd3dDevice, pd3dCommandList, vecMeshData[i]);
		model.push_back(pMesh);
	}

	m_uomModel[fileName] = model;
}

void ModelManager::Render(const char* modelName, ID3D12GraphicsCommandList* pd3dCommandList)
{
	assert(m_uomModel.count(modelName) && "modelName�� Ʋ�Ȱų�, ���� model�Դϴ�\n");
	for_each(m_uomModel[modelName].begin(), m_uomModel[modelName].end(), [&](Mesh* mesh) {mesh->Render(pd3dCommandList); });
}
