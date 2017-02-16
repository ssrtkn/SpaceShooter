#include "CameraProjective.h"

using namespace Render;

//constructor
CameraProjective::CameraProjective( std::string name, float nearplane, float farplane,  float fov  ) :
		Camera( name, nearplane, farplane ), mFov(fov)
{
}

//destructor
CameraProjective::~CameraProjective(void)
{
}

//return view matrix, which is the inverse of the camera's (=scene node's) world matrix
XMMATRIX CameraProjective::getViewMatrix()
{
	if( mSceneNode==NULL ) {		//is there a scene node?
		return XMMatrixIdentity();
	}

	XMMATRIX world = mSceneNode->getTransform( SceneNode::TS_WORLD );	//get scene node transform

	XMVECTOR det = XMMatrixDeterminant(world);
	return XMMatrixInverse( &det, world );		//return inverse
}


//create a projection matrix using D3D
XMMATRIX CameraProjective::getProjectionMatrix()
{
	return XMMatrixPerspectiveFovLH(mFov, mAspect, mNear, mFar);
}

//
//Return the origin and four points on the far plane
//
std::vector<XMFLOAT3> CameraProjective::getFrustum()
{
	std::vector<XMFLOAT3> res;

	float at = atanf( mFov/2.0f );
	XMMATRIX world = mSceneNode->getTransform( SceneNode::TS_WORLD );

	float fFar = mFar; //min( mFar, 1000.0f );
	float fNear = mNear;

	XMFLOAT3 point;
	/*XMStoreFloat3( &point, XMVector4Transform( XMVectorSet( -mAspect *  fNear * at, +fNear * at, fNear, 1.0f ), world ) );
	res.push_back( point );
	XMStoreFloat3( &point, XMVector4Transform( XMVectorSet( +mAspect *  fNear * at, +fNear * at, fNear, 1.0f ), world ) );
	res.push_back( point );
	XMStoreFloat3( &point, XMVector4Transform( XMVectorSet( -mAspect *  fNear * at, -fNear * at, fNear, 1.0f ), world ) );
	res.push_back( point );
	XMStoreFloat3( &point, XMVector4Transform( XMVectorSet( +mAspect *  fNear * at, -fNear * at, fNear, 1.0f ), world ) );
	res.push_back( point );*/

	XMStoreFloat3( &point, XMVector4Transform( XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f ), world ) );
	res.push_back( point );

	XMStoreFloat3( &point, XMVector4Transform( XMVectorSet( -mAspect *  fFar * at, +fFar * at, fFar, 1.0f ), world ) );
	res.push_back( point );
	XMStoreFloat3( &point, XMVector4Transform( XMVectorSet( +mAspect *  fFar * at, +fFar * at, fFar, 1.0f ), world ) );
	res.push_back( point );
	XMStoreFloat3( &point, XMVector4Transform( XMVectorSet( -mAspect *  fFar * at, -fFar * at, fFar, 1.0f ), world ) );
	res.push_back( point );
	XMStoreFloat3( &point, XMVector4Transform( XMVectorSet( +mAspect *  fFar * at, -fFar * at, fFar, 1.0f ), world ) );
	res.push_back( point );

	return res;
}

