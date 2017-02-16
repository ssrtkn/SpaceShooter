#include "MeshCube.h"


using namespace Render;

MeshCube::MeshCube( std::string name ) : Mesh(name)
{
}


MeshCube::~MeshCube(void)
{
}


void MeshCube::buildGeometry()
{
    Vertex vertices[] =
    {	
        { XMFLOAT3(-1.0f, +1.0f, -1.0f) },
        { XMFLOAT3(+1.0f, +1.0f, -1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f) },
        { XMFLOAT3(+1.0f, -1.0f, -1.0f) },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f) },
        { XMFLOAT3(+1.0f, +1.0f, +1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, +1.0f) },
        { XMFLOAT3(+1.0f, -1.0f, +1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, +1.0f) },
        { XMFLOAT3(-1.0f, +1.0f, +1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f) },
        { XMFLOAT3(-1.0f, +1.0f, -1.0f) },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f) },
        { XMFLOAT3(+1.0f, +1.0f, +1.0f) },
        { XMFLOAT3(+1.0f, -1.0f, -1.0f) },
        { XMFLOAT3(+1.0f, +1.0f, -1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, +1.0f) },
        { XMFLOAT3(+1.0f, -1.0f, +1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f) },
        { XMFLOAT3(+1.0f, -1.0f, -1.0f) },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f) },
        { XMFLOAT3(+1.0f, +1.0f, +1.0f) },
        { XMFLOAT3(-1.0f, +1.0f, -1.0f) },
        { XMFLOAT3(+1.0f, +1.0f, -1.0f) }
	};

	 UINT indices[] = {
        // front face
        0, 1, 2,
        1, 3, 2,
        4, 6, 5,
        5, 6, 7,

        8, 9, 10,
        9, 11, 10,
        12, 14, 13,
        13, 14, 15,

        16, 18, 17,
        17, 18, 19,
        20, 21, 22,
        21, 23, 22

    };

	for( int i=0; i<24; i++) {
		mVertices.push_back( vertices[i] );
		if( i%4==0 ) mVertices[i].Tex = XMFLOAT2(0,0);
		if( i%4==1 ) mVertices[i].Tex = XMFLOAT2(1,0);
		if( i%4==2 ) mVertices[i].Tex = XMFLOAT2(0,1);
		if( i%4==3 ) mVertices[i].Tex = XMFLOAT2(1,1);
	}
	for( int i=0; i<36; i++ ) {
		mIndices.push_back( indices[i] );
	}

	computeNormalTangent();
}

