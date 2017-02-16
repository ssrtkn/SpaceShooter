#include "MeshSphere.h"

using namespace Render;


MeshSphere::MeshSphere( std::string name, UINT nTheta, UINT nPhi, float texfactor ) : 
	Mesh(name), mNumTheta(nTheta), mNumPhi(nPhi)
{
	mTexFactor=texfactor;
}


MeshSphere::~MeshSphere(void)
{
}


void MeshSphere::buildGeometry()
{
	float dPhi   = XM_2PI / (mNumPhi-1);
	float dTheta = XM_PI / (mNumTheta-1);

	for( UINT iTheta = 0; iTheta<mNumTheta; iTheta++) {
		for( UINT iPhi = 0; iPhi<mNumPhi; iPhi++ ) {
			Vertex v;

			float phi   = iPhi*dPhi;
			float theta = iTheta*dTheta;
			v.Pos    = XMFLOAT3( sinf(theta)*cosf(phi), cosf(theta), sinf(theta)*sinf(phi) );
			v.Normal = v.Pos;
			v.Tex = XMFLOAT2( mTexFactor* theta / XM_PI, mTexFactor * phi / XM_2PI );
			v.TangentU = XMFLOAT3(0,0,0);

			mVertices.push_back( v );
		}
	}
	for( UINT iTheta=0; iTheta<mNumTheta-1; iTheta++ ) {
		for( UINT iPhi=0; iPhi<mNumPhi-1; iPhi++ ) {

			UINT v1 = iTheta*mNumTheta + iPhi;
			UINT v2 = iTheta*mNumTheta + iPhi+1;
			UINT v3 = (iTheta+1)*mNumTheta + iPhi;
			UINT v4 = (iTheta+1)*mNumTheta + iPhi+1;

			mIndices.push_back( v1 );
			mIndices.push_back( v2 );
			mIndices.push_back( v3 );

			mIndices.push_back( v2 );
			mIndices.push_back( v4 );
			mIndices.push_back( v3 );
		}
	}

	computeTangent();
}

