

#include "MeshGrid.h"


using namespace Render;


MeshGrid::MeshGrid(std::string name, UINT xdim, UINT zdim, float texfactor) : 
	Mesh( name ), mXdim(xdim), mZdim(zdim)
{
	mTexFactor = texfactor;
}


MeshGrid::~MeshGrid(void)
{
}


void MeshGrid::buildGeometry()
{
	float dx = 2.0f / (float)(mXdim - 1); //num intervals = num vertices - 1
	float dz = 2.0f / (float)(mZdim - 1);
	float du = 1.0f / (mXdim-1);
	float dv = 1.0f / (mZdim-1);

	for( UINT i=0; i<mZdim; i++ ) {
		for( UINT j=0; j<mXdim; j++ ) {
			Vertex v;

			v.Pos = XMFLOAT3( -1.0f + j*dx, 0.0f, -1.0f + i*dz );
			v.Normal = XMFLOAT3(0,1,0);
			v.Tex = XMFLOAT2( j*du * mTexFactor, i*dv * mTexFactor );

			mVertices.push_back(v);
		}
	}
	for( UINT i=0; i<mZdim-1; i++ ) {
		for( UINT j=0; j<mXdim-1; j++ ) {
			UINT v1 = i*mXdim + j;
			UINT v2 = i*mXdim + j + 1;
			UINT v3 = (i+1)*mXdim + j;
			UINT v4 = (i+1)*mXdim + j + 1;

			mIndices.push_back( v1 );
			mIndices.push_back( v3 );
			mIndices.push_back( v2 );

			mIndices.push_back( v2 );
			mIndices.push_back( v3 );
			mIndices.push_back( v4 );
		}
	}

	computeTangent();
}


bool MeshGrid::setHeightMap( std::wstring name )
{
	if( mVertices.size() == 0 )
		buildGeometry();

	unsigned char *heightmap=NULL;
	UINT nBytes=0;

	nBytes = D3DUtility::loadRawFile( name, (char**)&heightmap );

	if( heightmap && nBytes>0 ) {
		UINT dim = static_cast<UINT>(sqrtf( (float)nBytes ));

		for( UINT i=0; i<mZdim; i++ ) {
			for( UINT j=0; j<mXdim; j++ ) {
				UINT hx = static_cast<UINT>((mVertices[i*mXdim + j].Pos.x + 1.0)*dim/2.0f);
				UINT hz = static_cast<UINT>((mVertices[i*mXdim + j].Pos.z + 1.0)*dim/2.0f);

				//hz = dim - hz;

				mVertices[i*mXdim + j].Pos.y = heightmap[hz*dim + hx]/255.0f;
			}
		}
		return true;
	}
	return false;
}


float MeshGrid::getY( float x, float z)
{
	float cx = min( max( -1.0f, x ), 1.0f );
	float cz = min( max( -1.0f, z ), 1.0f );
	float idxX = (mXdim-1.0f)*(cx+1.0f)/2.0f;
	float idxZ = (mZdim-1.0f)*(cz+1.0f)/2.0f;

	int xf = (int)floor(idxX);
	int zf = (int)floor(idxZ);
	int xc = (int)ceil(idxX);
	int zc = (int)ceil(idxZ);

	XMFLOAT3 v0 = mVertices[zf*mXdim + xf].Pos;
	XMFLOAT3 v1 = mVertices[zf*mXdim + xc].Pos;
	XMFLOAT3 v2 = mVertices[zc*mXdim + xf].Pos;
	XMFLOAT3 v3 = mVertices[zc*mXdim + xc].Pos;

	float y0 = D3DUtility::lerp( v0.x, v0.y, v1.x, v1.y, x );
	float y1 = D3DUtility::lerp( v2.x, v2.y, v3.x, v3.y, x );

	return D3DUtility::lerp( v0.z, y0, v2.z, y1, z);
}
