

#include "MovableObject.h"

using namespace Render;

//constructor
MovableObject::MovableObject(std::string name) : GameObject(name), mSceneNode()
{
}

//destructor
MovableObject::~MovableObject(void)
{
}

//return object position (=position of scene node)
XMVECTOR MovableObject::getPosition( SceneNode::TransformSpace ts )
{
	if( mSceneNode ) {
		return mSceneNode->getPosition( ts );
	}
	return XMVectorSet( 0,0,0,1);
}

//return object oritntation (=orientation of scene node)
XMMATRIX MovableObject::getOrientation( SceneNode::TransformSpace ts )
{
	if( mSceneNode ) {
		return mSceneNode->getOrientation( ts );
	}
	return XMMatrixIdentity();

}


