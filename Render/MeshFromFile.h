#pragma once


#include "Mesh.h"

namespace Render {

class MeshFromFile : public Mesh
{
public:
	MeshFromFile( std::string name, std::wstring filename, UINT subset = 0 );
	~MeshFromFile(void);

	void buildGeometry();		//load the geometry

protected:
	std::wstring filename;
	UINT subset;
};

}

