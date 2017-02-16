#include "MeshFromFile.h"
#include "AssetManager.h"

#include <assimp/scene.h>

using namespace Render;
using namespace Assimp;

MeshFromFile::MeshFromFile( std::string name, std::wstring filename, UINT subset ) : Mesh(name), filename(filename), subset(subset)
{
}


MeshFromFile::~MeshFromFile(void)
{
}


void MeshFromFile::buildGeometry()
{
	// Import mesh from file using Open Asset Import Library
	std::string _filename(filename.begin(), filename.end());
	const aiScene* scene = AssetManager::getInstance()->readFile(_filename);
	if(!scene)
		return;

	// Check subset index
	if(subset >= scene->mNumMeshes)
	{
		char buf[256];
		sprintf_s(buf, "Subset index (%i) out of bounds. The file %s only contains %i meshes.", subset, _filename.c_str(), scene->mNumMeshes);
		MessageBoxA(0, buf, "Error loading mesh from file.", MB_OK | MB_ICONERROR);
		return;
	}

	// Store vertices
	aiMesh* mesh = scene->mMeshes[subset];
	Vertex newvertex;
	bool hasnormals = mesh->HasNormals();
	bool hastangents = mesh->HasTangentsAndBitangents();
	bool hastexcoords = mesh->HasTextureCoords(0);
	bool hascolor = mesh->HasVertexColors(0);
	for(UINT i = 0; i < mesh->mNumVertices; i++)
	{
		newvertex.Pos = XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		if(hasnormals)
			newvertex.Normal = XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		if(hastangents)
			newvertex.TangentU = XMFLOAT3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
		if(hastexcoords)
			newvertex.Tex = XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		if(hascolor)
			newvertex.Color = XMFLOAT4(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b, mesh->mColors[0][i].a);
		mVertices.push_back( newvertex );
	}

	// Store indices
	for(UINT i = 0; i < mesh->mNumFaces; i++)
		for(UINT j = 0; j < mesh->mFaces[i].mNumIndices; j++)
			mIndices.push_back(mesh->mFaces[i].mIndices[j]);

	// Compute tangents/normals if needed
	if(!hastangents)
		computeNormalTangent(!hasnormals);
}

