#include "CameraOrtho.h"


using namespace Render;

//constructor
CameraOrtho::CameraOrtho( std::string name, float nearplane, float farplane,  float width  ) :
		Camera( name, nearplane, farplane ), mWidth(width)
{}

//destructor
CameraOrtho::~CameraOrtho(void)
{
}

//Inverse of the scene node's world transform
XMMATRIX CameraOrtho::getViewMatrix()
{
	if( mSceneNode==NULL ) {		//is there a scene node?
		return XMMatrixIdentity();
	}

	XMMATRIX world = mSceneNode->getTransform( SceneNode::TS_WORLD );

	XMVECTOR det = XMMatrixDeterminant(world);
	return XMMatrixInverse( &det, world );	//return inverse of world transform
}

//get orthographic projetion matrix from D3D
XMMATRIX CameraOrtho::getProjectionMatrix()
{
	return XMMatrixOrthographicLH( mWidth, mWidth/mAspect, mNear, mFar);
}

//get points defining the camera frustum which is a cuboid
std::vector<XMFLOAT3> CameraOrtho::getFrustum()
{
	std::vector<XMFLOAT3> res(8);

	XMMATRIX world = mSceneNode->getTransform( SceneNode::TS_WORLD );

	XMFLOAT3 point;

	XMStoreFloat3(&point, XMVector4Transform( XMVectorSet( -mWidth/2.0f, +mWidth/(2.0f*mAspect), mNear, 1.0f ), world ) );
	res.push_back( point );
	XMStoreFloat3(&point, XMVector4Transform( XMVectorSet( +mWidth/2.0f, +mWidth/(2.0f*mAspect), mNear, 1.0f ), world ) );
	res.push_back( point );
	XMStoreFloat3(&point, XMVector4Transform( XMVectorSet( -mWidth/2.0f, -mWidth/(2.0f*mAspect), mNear, 1.0f ), world ) );
	res.push_back( point );
	XMStoreFloat3(&point, XMVector4Transform( XMVectorSet( +mWidth/2.0f, -mWidth/(2.0f*mAspect), mNear, 1.0f ), world ) );
	res.push_back( point );
	XMStoreFloat3(&point, XMVector4Transform( XMVectorSet( -mWidth/2.0f, +mWidth/(2.0f*mAspect), mFar, 1.0f ), world ) );
	res.push_back( point );
	XMStoreFloat3(&point, XMVector4Transform( XMVectorSet( +mWidth/2.0f, +mWidth/(2.0f*mAspect), mFar, 1.0f ), world ) );
	res.push_back( point );
	XMStoreFloat3(&point, XMVector4Transform( XMVectorSet( -mWidth/2.0f, -mWidth/(2.0f*mAspect), mFar, 1.0f ), world ) );
	res.push_back( point );
	XMStoreFloat3(&point, XMVector4Transform( XMVectorSet( +mWidth/2.0f, -mWidth/(2.0f*mAspect), mFar, 1.0f ), world ) );
	res.push_back( point );
	return res;
}

//
//The shadow camera's frustum should cover the normal camera's frustum
//So set the shadow camera's frustum to the points defining the normal camera's frustum
//
void CameraOrtho::setFrustumToPoints( std::vector<XMFLOAT3> points )
{
	if( points.size() == 0 ) return;
	if( mSceneNode == NULL ) return;

	XMMATRIX view = getViewMatrix();

	XMFLOAT3 vmin, vmax, vec;

	XMStoreFloat3( &vec, XMVector4Transform( XMVectorSet( points[0].x, points[0].y, points[0].z, 1.0f ), view ) ); //tramsform to cam space

	vmin = vec;	//current min and max
	vmax = vec;

	for( UINT i=1; i<points.size(); i++ ) {		//go through all frustum points
		XMStoreFloat3( &vec, XMVector4Transform( XMVectorSet( points[i].x, points[i].y, points[i].z, 1.0f ), view ) ); //tramsform to cam space
		vmin = XMFLOAT3( min(vmin.x, vec.x), min(vmin.y, vec.y), min(vmin.z, vec.z)  );	//compute component wise min
		vmax = XMFLOAT3( max(vmax.x, vec.x), max(vmax.y, vec.y), max(vmax.z, vec.z)  );	//compute component wise max
	}
	mFar = mNear + (vmax.z - vmin.z);		//far plane parameter

	mSceneNode->setPosition( XMVectorSet( (vmax.x+vmin.x)/2.0f,			//set shadow camera position using heuristic
							(vmax.y+vmin.y)/2.0f, vmin.z-mNear, 1.0f ), 
							SceneNode::TS_LOCAL );
	mWidth = vmax.x - vmin.x;
	setAspectRatio(  mWidth / (vmax.y - vmin.y) );		//set shadow camera aspect ratio
}

