#pragma once

#include "D3DUtility.h"

#include <assimp/scene.h> // Output data structure

namespace Render {
	
class SceneImporter
{
public:
	SceneImporter(void);
	~SceneImporter(void);

	bool loadFile( std::string& name );

protected:
	void importMeshes( const aiScene* scene );
	void importTextures( const aiScene* scene );
	void importMaterials( const aiScene* scene );
	void importCameras( const aiScene* scene );
	void importLights( const aiScene* scene  );
	void importAnimations( const aiScene* scene );

};

}

