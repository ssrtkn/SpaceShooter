
#include "Mesh.h"
#include "Root.h"
#include "RenderSystem.h"


using namespace Render;

//constructor
Mesh::Mesh( std::string name ) : 
	GameObject( name ),
	mIndexBuffer(NULL),
	mTexFactor(1.0f)
{
}

//
//Destructor
//Buffers must be released with ReleaseCOM
//
Mesh::~Mesh(void)
{
	std::map<UINT,ID3D11Buffer*>::iterator iter;
	for (iter = mVBuffers.begin(); iter != mVBuffers.end(); iter++) {
		ReleaseCOM( iter->second );
		mVBuffers.erase( iter );
    }
	ReleaseCOM( mIndexBuffer );
}

//
//Get a pointer to a vertex buffer with a specified byte layout
//If the vertex buffer does not exist yet, then create it
//
ID3D11Buffer* Mesh::getVertexBuffer(  UINT bufferFlags )
{
	std::map<UINT,ID3D11Buffer*>::iterator it;
	it = mVBuffers.find(bufferFlags);	//does the name exist in the map?

	if( it != mVBuffers.end() )			//find im map
		return it->second;				//if exists return it

	//does not exist in the map -> create the vertex buffer
	if( mVertices.size() == 0 )			//if no vertices in the mesh try to create them
		buildGeometry();

	Root * root = Root::getInstance();
	RenderSystem *rs = RenderSystem::getInstance();

    ID3D11Device* md3dDevice = rs->getDevice();

	//determin the size of each buffer entry
	UINT numFloats=0;
	if( bufferFlags & VB_POSITION ) numFloats += 3;
	if( bufferFlags & VB_NORMAL ) numFloats += 3;
	if( bufferFlags & VB_SIZE ) numFloats += 2;
	if( bufferFlags & VB_TEXCOORD ) numFloats += 2;
	if( bufferFlags & VB_TANGENT ) numFloats += 3;
	if( bufferFlags & VB_COLOR ) numFloats += 4;
	float *buffer = new float[numFloats*mVertices.size()];	//allocate data the buffer

	//fill the vertex data into the data buffer
	for( UINT idxV=0, i=0; i<mVertices.size(); i++ ) {		
		if( bufferFlags & VB_POSITION ) { *(XMFLOAT3*)(&buffer[idxV]) = mVertices[i].Pos;      idxV += 3; }
		if( bufferFlags & VB_NORMAL )   { *(XMFLOAT3*)(&buffer[idxV]) = mVertices[i].Normal;   idxV += 3; }
		if( bufferFlags & VB_SIZE )		{ *(XMFLOAT2*)(&buffer[idxV]) = mVertices[i].Size;	   idxV += 2; }
		if( bufferFlags & VB_TEXCOORD ) { *(XMFLOAT2*)(&buffer[idxV]) = mVertices[i].Tex;      idxV += 2; }
		if( bufferFlags & VB_TANGENT )  { *(XMFLOAT3*)(&buffer[idxV]) = mVertices[i].TangentU; idxV += 3; }
		if( bufferFlags & VB_COLOR )    { *(XMFLOAT4*)(&buffer[idxV]) = mVertices[i].Color;    idxV += 4; }
	}

	ID3D11Buffer* vertexBuffer;										//pointer to D3D vertex buffer
	D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = numFloats*sizeof(float)*mVertices.size();		//size of data buffer
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = buffer;										//use this data buffer to fill the vertex buffer
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &vertexBuffer));	//create the D3D vertex buffer

	delete[] buffer;	//delete data buffer

	mVBuffers.insert( std::pair<UINT,ID3D11Buffer*>(bufferFlags,vertexBuffer) );	//insert D3D vertex buffer into the map

	return vertexBuffer;
}


//
//Get a pointer to the D3D index buffer
//
ID3D11Buffer* Mesh::getIndexBuffer()
{
	if( mIndexBuffer != NULL )		//if already exists, then return it
		return mIndexBuffer;

	if( mIndices.size() == 0 )		//if geometry does not exist yet, create the geometry
		buildGeometry();

	Root * root = Root::getInstance();
	RenderSystem *rs = RenderSystem::getInstance();

    ID3D11Device* md3dDevice = rs->getDevice();

	D3D11_BUFFER_DESC ibd;								//index buffer descirptor
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * mIndices.size();		//size of index buffer
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &mIndices[0];					//indices data

    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIndexBuffer));	//create the D3D index buffer
	return mIndexBuffer;
}


//Create the 3D geometry, should be done in a inherited class
void Mesh::buildGeometry()
{
	//computeNormalTangent();
}


//add a vertex to the vertex buffer
void Mesh::addVertex( Vertex v)
{
	mVertices.push_back( v );
}

//add vertices to the vertex buffer
void Mesh::addVertices( std::vector<Vertex> vlist )
{
	mVertices.insert( mVertices.end(), vlist.begin(), vlist.end() );
}

//add an index to the index buffer
void Mesh::addIndex( UINT idx )
{
	mIndices.push_back( idx );
}


void Mesh::addIndices( std::vector<UINT> idxlist )
{
	UINT base = mIndices.size();
	std::vector<UINT>::iterator it = idxlist.begin();
	while( it != idxlist.end() ) {
		mIndices.push_back( (*it) + base );
	}
}


void Mesh::computeTangent()
{
	computeNormalTangent ( false );
}

void Mesh::computeNormalTangent()
{
	return computeNormalTangent( true );
}

void Mesh::computeNormalTangent( bool normal )
{
	for( UINT i=0; i< mVertices.size(); i++ ) {
		if( normal )
			mVertices[i].Normal   = XMFLOAT3(0,0,0);
		mVertices[i].TangentU = XMFLOAT3(0,0,0);
	}
	//go through all triangles
	for( UINT i=0; i<mIndices.size(); i+=3) {
		UINT i0=mIndices[i], i1=mIndices[i+1], i2=mIndices[i+2];
		Vertex *v0=&mVertices[i0], *v1=&mVertices[i1], *v2=&mVertices[i2];

		XMVECTOR e0 = XMLoadFloat3(&v1->Pos) - XMLoadFloat3(&v0->Pos);
		XMVECTOR e1 = XMLoadFloat3(&v2->Pos) - XMLoadFloat3(&v0->Pos);

		if( normal ) {
			XMVECTOR N  = XMVector3Normalize( XMVector3Cross( e0, e1 ) );
			XMStoreFloat3( &v0->Normal, XMLoadFloat3( &v0->Normal ) + N );
			XMStoreFloat3( &v1->Normal, XMLoadFloat3( &v1->Normal ) + N );
			XMStoreFloat3( &v2->Normal, XMLoadFloat3( &v1->Normal ) + N );
		}

		float a = v1->Tex.x - v0->Tex.x;
		float b = v1->Tex.y - v0->Tex.y;
		float c = v2->Tex.x - v0->Tex.x;
		float d = v2->Tex.y - v0->Tex.y;
		XMFLOAT3X3 Ainv;
		XMStoreFloat3x3( &Ainv, XMMatrixIdentity() );
		float f = 1.0f / ( a*d - b*c );
		Ainv._11 = f*d; Ainv._12 = f*-b; Ainv._21 = f*-c; Ainv._22 = f*a;
		XMMATRIX MAinv = XMLoadFloat3x3( &Ainv );

		XMFLOAT3X3 e;
		XMStoreFloat3( (XMFLOAT3*)&e._11, e0 );
		XMStoreFloat3( (XMFLOAT3*)&e._21, e1 );
		e._31=0;e._32=0;e._33=1;
		XMMATRIX E = XMLoadFloat3x3( &e );

		XMFLOAT3X3 V;
		XMStoreFloat3x3( &V, XMMatrixMultiply( MAinv, E ) );
		XMVECTOR TU = XMVector3Normalize( XMVectorSet( V._11, V._12, V._13, 0 ) );
		XMStoreFloat3( &v0->TangentU, XMLoadFloat3( &v0->TangentU ) + TU  );
		XMStoreFloat3( &v1->TangentU, XMLoadFloat3( &v1->TangentU ) + TU );
		XMStoreFloat3( &v2->TangentU, XMLoadFloat3( &v2->TangentU ) + TU );
	}

	for( UINT i=0; i<mVertices.size(); i++) {
		XMStoreFloat3( &mVertices[i].Normal,   XMVector3Normalize( XMLoadFloat3( &mVertices[i].Normal ) ) );	//orthogonalize in shader
		XMStoreFloat3( &mVertices[i].TangentU, XMVector3Normalize( XMLoadFloat3( &mVertices[i].TangentU ) ) );
	}
}

