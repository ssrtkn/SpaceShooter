#pragma once


#include "Mesh.h"

namespace Render {

class MeshCube : public Mesh
{
public:
	MeshCube( std::string name );
	~MeshCube(void);

	void buildGeometry();		//compute the geometry of a simple cube
protected:

};

}

