#pragma once


#include "D3DUtility.h"
#include "GameObject.h"


namespace Render {


class AssetManager;

class Mesh : public GameObject
{
	friend AssetManager;

public:

	enum VB_LAYOUT {		//vertex buffer layout IDs
		VB_POSITION=1,
		VB_NORMAL=2,
		VB_TEXCOORD=4,
		VB_TANGENT=8,
		VB_COLOR=16,
		VB_SIZE	=32
	};

	struct Vertex
	{
		XMFLOAT3 Pos;		//local object position
		XMFLOAT3 Normal;	//surface normal vector
		XMFLOAT2 Size;		//2D size e.g. Billboards
		XMFLOAT2 Tex;		//texture coordinates
		XMFLOAT3 TangentU;	//tangent space u-axis
	    XMFLOAT4 Color;		//per vertex color
	};

	RENDERDLL_API ~Mesh(void);
	virtual RENDERDLL_API void buildGeometry();								//compute or load the geometry

	RENDERDLL_API void addVertex( Vertex v);									//add a vertex to the vertex buffer
	RENDERDLL_API void addIndex( UINT idx);									//add index to index buffer
	RENDERDLL_API void addVertices( std::vector<Vertex> vlist);				//add a vertex to the vertex buffer
	RENDERDLL_API void addIndices( std::vector<UINT> idxlist);				//add index to index buffer
	RENDERDLL_API std::vector<Vertex> getVertices(){ return mVertices; };		//return the vertices
	RENDERDLL_API std::vector<UINT> getIndices(){ return mIndices; };			//return indices

	RENDERDLL_API ID3D11Buffer* getVertexBuffer(UINT bufferFlags );			//return vertex buffer for D3D
	RENDERDLL_API ID3D11Buffer* getIndexBuffer();								//return index buffer for D3D

	RENDERDLL_API UINT getNumVertices(){ return mVertices.size(); };		//number of vertices
	RENDERDLL_API UINT getNumIndices(){ return mIndices.size(); };		//number of indices

	RENDERDLL_API float getTexFactor(){ return mTexFactor; };

protected:
	Mesh( std::string name );
	void computeTangent();							//compute the TangentU vector for all vertices
	void computeNormalTangent( bool normal );		//compute tangent and normals
	void computeNormalTangent();					//wrapper for computiing tangent and normals
	virtual UINT getVertexSize(){ return sizeof(Mesh::Vertex); };

	std::vector<Vertex> mVertices;		//vertices
	std::vector<UINT> mIndices;			//indices

	std::map<UINT,ID3D11Buffer*> mVBuffers;	//map for the various vertex buffers that are created
	ID3D11Buffer* mIndexBuffer;				//index buffer

	float mTexFactor;		//multiplied to texture coordinates -> WRAP -> preserve texture details
};

}

