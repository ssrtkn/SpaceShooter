#include "Material.h"
#include "AssetManager.h"

#include <assimp/scene.h>
#include <assimp/material.h>

using namespace Render;
using namespace Assimp;

void Material::setFromFile( std::wstring filename, std::wstring texturepath, UINT subset )
{
	// Import mesh from file using Open Asset Import Library
	std::string _filename(filename.begin(), filename.end());
	const aiScene* scene = AssetManager::getInstance()->readFile(_filename);
	if(!scene)
		return;

	// Check subset index
	if(subset >= scene->mNumMaterials)
	{
		char buf[256];
		sprintf_s(buf, "Subset index (%i) out of bounds. The file %s only contains %i materials.", subset, _filename.c_str(), scene->mNumMeshes);
		MessageBoxA(0, buf, "Error loading material from file.", MB_OK | MB_ICONERROR);
		return;
	}

	// Read properties
	aiMaterial* material = scene->mMaterials[subset];
	aiColor3D cprop(0.f,0.f,0.f);
	float fprop;
	aiString sprop;
	if(material->Get(AI_MATKEY_COLOR_AMBIENT, cprop) == AI_SUCCESS)
	{
		mMaterial.Ambient.x = cprop.r;
		mMaterial.Ambient.y = cprop.g;
		mMaterial.Ambient.z = cprop.b;
	}
	if(material->Get(AI_MATKEY_COLOR_DIFFUSE, cprop) == AI_SUCCESS)
	{
		mMaterial.Diffuse.x = cprop.r;
		mMaterial.Diffuse.y = cprop.g;
		mMaterial.Diffuse.z = cprop.b;
	}
	if(material->Get(AI_MATKEY_COLOR_SPECULAR, cprop) == AI_SUCCESS)
	{
		mMaterial.Specular.x = cprop.r;
		mMaterial.Specular.y = cprop.g;
		mMaterial.Specular.z = cprop.b;
	}
	if(material->Get(AI_MATKEY_OPACITY, fprop) == AI_SUCCESS)
		mMaterial.Diffuse.w = fprop;
	if(material->Get(AI_MATKEY_SHININESS, fprop) == AI_SUCCESS)
		mMaterial.Specular.w = fprop;
	if(material->Get( AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), sprop) == AI_SUCCESS)
	{
		std::string texfilename(sprop.C_Str());
		this->setTexture(texturepath + std::wstring(texfilename.begin(), texfilename.end()));
	}
}