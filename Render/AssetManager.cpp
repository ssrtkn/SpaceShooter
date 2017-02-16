

#include "AssetManager.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshCube.h"
#include "MeshGrid.h"
#include "MeshSphere.h"
#include "MeshFromFile.h"
#include "Effect.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#pragma comment (lib, "assimp.lib")

using namespace Render;

AssetManager* AssetManager::theInstance = NULL;


AssetManager::AssetManager(void)
{
	importer = new ::Assimp::Importer();
}


AssetManager::~AssetManager(void)
{
	delete importer;
}


AssetManager * AssetManager::getInstance()
{
    if ( AssetManager::theInstance==NULL ) {
        AssetManager::theInstance = new AssetManager();
	}
    return AssetManager::theInstance;
}


void AssetManager::addAsset( shared_ptr<GameObject> gob ) 
{
	mAssetMap.insert( std::pair<std::string,shared_ptr<GameObject>>(gob->getName(),gob) );
}


shared_ptr<GameObject> AssetManager::getAsset(std::string name)
{
	std::map<std::string,shared_ptr<GameObject>>::iterator it = mAssetMap.find( name );

	if( it == mAssetMap.end() )
		return shared_ptr<GameObject>();

	return it->second;
}


void AssetManager::destroyAsset(std::string name)
{
	mAssetMap.erase( name );
}


shared_ptr<Material> AssetManager::createMaterial(std::string name)
{
	if( getAsset( name ) != NULL ) return shared_ptr<Material>();	//if asset exists return NULL

	shared_ptr<Material> mat( new Material(name) );
	addAsset( mat );
	return mat;
}


shared_ptr<Material> AssetManager::createMaterialFromFile(std::string name, std::wstring filename, std::wstring texturepath, UINT subset)
{
	if( getAsset( name ) != NULL ) return shared_ptr<Material>();	//if asset exists return NULL

	shared_ptr<Material> mat( new Material(name) );
	mat->setFromFile(filename, texturepath, subset);
	addAsset( mat );
	return mat;
}


shared_ptr<Mesh> AssetManager::createMesh(std::string name )
{
	if( getAsset( name ) != NULL ) return shared_ptr<Mesh>();	//if asset exists return NULL

	shared_ptr<Mesh> mesh( new Mesh(name) );
	addAsset( mesh );
	return mesh;
}


shared_ptr<MeshCube> AssetManager::createMeshCube( std::string name )
{
	if( getAsset( name ) != NULL ) return shared_ptr<MeshCube>();	//if asset exists return NULL

	shared_ptr<MeshCube> mesh( new MeshCube(name) );
	addAsset( mesh );
	return mesh;
}


shared_ptr<MeshGrid> AssetManager::createMeshGrid(	std::string name, 
													UINT xdim,				//num subdivisions in x-direction 
													UINT zdim, 				//num subdivisions in z-direction 
													float texfactor)
{
	if( getAsset( name ) != NULL ) return shared_ptr<MeshGrid>();	//if asset exists return NULL

	shared_ptr<MeshGrid> mesh( new MeshGrid(name, xdim, zdim, texfactor) );
	addAsset( mesh );
	return mesh;
}


shared_ptr<MeshSphere>	AssetManager::createMeshSphere(	std::string name,
														UINT nTheta,				//num subdivisions polar
														UINT nPhi,					//num subdivisions azimuth
														float texfactor)		//multiplied to tex coordinates to preserve details);
{
	if( getAsset( name ) != NULL ) return shared_ptr<MeshSphere>();	//if asset exists return NULL

	shared_ptr<MeshSphere> mesh( new MeshSphere(name, nTheta, nPhi, texfactor) );
	addAsset( mesh );
	return mesh;
}


shared_ptr<MeshFromFile> AssetManager::createMeshFromFile( std::string name, std::wstring filename, UINT subset )
{
	if( getAsset( name ) != NULL ) return shared_ptr<MeshFromFile>();	//if asset exists return NULL

	shared_ptr<MeshFromFile> mesh( new MeshFromFile(name, filename, subset) );
	addAsset( mesh );
	return mesh;
}


shared_ptr<Effect> AssetManager::getEffect(std::string name )
{
	shared_ptr<GameObject> gob = getAsset( name );
	if( gob != NULL ) return dynamic_pointer_cast<Effect>(gob);		//if asset exists return it

	return shared_ptr<Effect>();
}


void AssetManager::storeEffect( shared_ptr<Effect> effect )
{
	shared_ptr<GameObject> gob = getAsset( effect->getName() );
	if( gob != NULL ) return

	addAsset( effect );
}

const ::aiScene* AssetManager::readFile(std::string filename)
{
	if(filename == currentfile)
		return currentscene;

	// Import mesh from file using Open Asset Import Library
	const ::aiScene* scene = importer->ReadFile(filename,
												aiProcess_CalcTangentSpace |
												aiProcess_Triangulate |
												aiProcess_JoinIdenticalVertices |
												aiProcess_SortByPType | aiProcess_FlipUVs );
	if(!scene)
	{
		char buf[256];
		sprintf_s(buf, "Error loading file %s.", filename.c_str());
		MessageBoxA(0, importer->GetErrorString(), buf, MB_OK | MB_ICONERROR);
		return NULL;
	}

	// Cache scene to avoid loading the same file multiple times
	currentfile = filename;
	currentscene = scene;

	return scene;
}

UINT AssetManager::getNumMeshes(std::string filename)
{
	const ::aiScene* scene = readFile(filename);
	return scene ? scene->mNumMeshes : 0;
}

UINT AssetManager::getNumMaterials(std::string filename)
{
	const ::aiScene* scene = readFile(filename);
	return scene ? scene->mNumMaterials : 0;
}