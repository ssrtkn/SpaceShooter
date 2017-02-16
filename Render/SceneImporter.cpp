

#include "SceneImporter.h"
#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags

using namespace Render;


SceneImporter::SceneImporter(void)
{
}


SceneImporter::~SceneImporter(void)
{
}


bool SceneImporter::loadFile( std::string& pFile )
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile( pFile, aiProcess_CalcTangentSpace |
												aiProcess_Triangulate |
												aiProcess_JoinIdenticalVertices |
												aiProcess_ConvertToLeftHanded | 
												aiProcess_CalcTangentSpace |
												aiProcess_GenNormals );
	// If the import failed, report it
	if( !scene)
	{
		//DoTheErrorLogging( importer.GetErrorString());
		return false;
	}

	importMeshes( scene );
	return true;
}



void SceneImporter::importMeshes( const aiScene* scene )
{
}

void SceneImporter::importTextures( const aiScene* scene )
{
}

void SceneImporter::importMaterials( const aiScene* scene )
{
}

void SceneImporter::importCameras( const aiScene* scene )
{
}

void SceneImporter::importLights( const aiScene* scene  )
{
}

void SceneImporter::importAnimations( const aiScene* scene )
{
}


