#pragma once

#include "Mesh.h"

namespace Render {

class MeshSphere : public Mesh
{
public:
	MeshSphere( std::string name, 
				UINT nTheta,				//num subdivisions polar
				UINT nPhi,					//num subdivisions azimuth
				float texfactor=1.0f );		//multiplied to tex coordinates to preserve details
	~MeshSphere(void);

	void buildGeometry();
protected:
	UINT mNumTheta;				//num subdivisions polar
	UINT mNumPhi;				//num subdivisions azimuth
};

}

