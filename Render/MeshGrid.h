#pragma once

#include "Mesh.h"


namespace Render {


class EntityTerrain;

class MeshGrid : public Mesh
{
	friend EntityTerrain;

public:
	MeshGrid(	std::string name, 
				UINT xdim,				//num subdivisions in x-direction 
				UINT zdim, 				//num subdivisions in z-direction 
				float texfactor=1.0f);	//multiplied to texture coordinates -> WRAP -> preserve texture details
	~MeshGrid(void);
	void buildGeometry();				//create a flat grid

	RENDERDLL_API bool setHeightMap( std::wstring name );	//must be quadratic raw file with floats

	UINT getXdim(){ return mXdim; };
	UINT getZdim(){ return mZdim; };

	RENDERDLL_API float getY( float x, float z); 
protected:
	UINT mXdim;				//num vertices in x-direction 
	UINT mZdim;				//num vertices in z-direction 
};

}

