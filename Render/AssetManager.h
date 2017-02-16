#pragma once

#include "D3DUtility.h"
#include "GameObject.h"

namespace Assimp
{
	class Importer;
}
struct aiScene;

namespace Render {

class Mesh;
class MeshCube;
class MeshGrid;
class MeshSphere;
class MeshFromFile;
class Material;
class MaterialFromFile;
class Effect;

class AssetManager
{
public:
	RENDERDLL_API ~AssetManager(void);
	static RENDERDLL_API AssetManager *getInstance();	//singleton!

	RENDERDLL_API shared_ptr<GameObject> getAsset( std::string name );
	RENDERDLL_API void destroyAsset( std::string name ); 

	RENDERDLL_API shared_ptr<Material>	createMaterial(std::string name);
	RENDERDLL_API shared_ptr<Material>	createMaterialFromFile(std::string name, std::wstring filename,
												   std::wstring texturepath, UINT subset = 0); // Helper function: Creates a material and calls Material::setFromFile()

	RENDERDLL_API shared_ptr<Mesh>			createMesh(std::string);
	RENDERDLL_API shared_ptr<MeshCube>		createMeshCube(std::string);
	RENDERDLL_API shared_ptr<MeshGrid>		createMeshGrid(std::string, 
											   UINT xdim,				//num subdivisions in x-direction 
											   UINT zdim, 				//num subdivisions in z-direction 
											   float texfactor = 1.0f);
	RENDERDLL_API shared_ptr<MeshSphere>		createMeshSphere(std::string,
												 UINT nTheta,				//num subdivisions polar
												 UINT nPhi,					//num subdivisions azimuth
												 float texfactor = 1.0f);		//multiplied to tex coordinates to preserve details);
	RENDERDLL_API shared_ptr<MeshFromFile>	createMeshFromFile(std::string name, std::wstring filename, UINT subset = 0);

	RENDERDLL_API shared_ptr<Effect>	getEffect(std::string);
	RENDERDLL_API void				storeEffect(shared_ptr<Effect> effect);

	RENDERDLL_API const ::aiScene* readFile(std::string filename);
	RENDERDLL_API UINT getNumMeshes(std::string filename);
	RENDERDLL_API UINT getNumMaterials(std::string filename);

protected:
	AssetManager(void);
	void addAsset( shared_ptr<GameObject> asset );


protected:
	static AssetManager *theInstance;					//singleton

	std::map<std::string,std::tr1::shared_ptr<GameObject>> mAssetMap;		//Materials

private:
	::Assimp::Importer* importer;
	const ::aiScene* currentscene;
	std::string currentfile;
};

}