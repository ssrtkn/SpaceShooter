
#include "SceneManager.h"
#include "SceneNode.h"
#include "MovableObject.h"

using namespace Render;

//constructor
SceneNode::SceneNode(std::string name) : GameObject(name), mParent()
{
	mPosition = XMFLOAT3(0,0,0);		//default position
	XMMATRIX I = XMMatrixIdentity();	//set default orientation
	XMStoreFloat3x3(&mOrientation, I);
	mScaling = XMFLOAT3(1,1,1);			//default scaling
}

//destructor
SceneNode::~SceneNode(void)
{
}

//
//set position to a fixed point (w=1)
//position is given in a certain space
//so first transform the position to parent space
//
void SceneNode::setPosition( CXMVECTOR position, TransformSpace ts )
{
	XMVECTOR pos = changeToParentSpace( position, ts);

	XMStoreFloat3( &mPosition, pos );
}

//
//return position position in the specified space
//TS_LOCAL: is always 0
//TS_PARENT: the position in parent space
//TS_WORLD: the position in world space = last row of the world matrix
//
XMVECTOR SceneNode::getPosition( TransformSpace ts )
{
	if( ts == TS_LOCAL ) {
		return XMVectorSet(0,0,0,1);
	}

	if( ts == TS_PARENT ) {
		return XMVectorSet( mPosition.x, mPosition.y, mPosition.z, 1.0f );
	}

	//world
   	XMMATRIX world = getTransform( TS_WORLD );
	XMFLOAT4X4 wm;
	XMStoreFloat4x4( &wm, world );

	return XMVectorSet( wm._41, wm._42, wm._43, 1);
}

//
//set orientation in parent space
//orientation is given in a certain space
//so first transform to parent space
//
void SceneNode::setOrientation( CXMMATRIX orientation,  TransformSpace ts )
{
	XMMATRIX or = orientation;									//do nothing if its already in parent space

	if( ts == TS_LOCAL ) {
		or = or * getTransform( TS_PARENT);						//transform from local to parent space
	}
	if( ts == TS_WORLD ) {										//transform from world into parent space
		XMMATRIX world = getTransform( TS_WORLD );				//get world matrix

		XMVECTOR det = XMMatrixDeterminant( world );			//necessary for computing the inverse

		or = or * XMMatrixInverse( &det, world ) * getTransform( TS_PARENT ); //transform first to local, then parent space
	}

	XMStoreFloat3x3( &mOrientation, or );
}

//
//get orientation in the selected space space
//TS_LOCAL: always identity matrix
//TS_PARENT: = the set orientation
//TS_WORLD: the world matrix, but set the translation to zero
//
XMMATRIX SceneNode::getOrientation( TransformSpace ts )
{
	if( ts == TS_LOCAL ) {							//orientation relative to own space
		return XMMatrixIdentity();
	}
	if( ts == TS_PARENT ) {							//orientation relative to parent space
		return XMLoadFloat3x3( &mOrientation );
	}

	//orientation relative to world space
	XMMATRIX world = getTransform( TS_WORLD );		//compute recursively up to root scene node
	XMFLOAT4X4 wm;
	XMStoreFloat4x4( &wm, world );
	wm._41 = 0;		//we do not need porition, only orientation
	wm._42 = 0;
	wm._43 = 0;

	return XMLoadFloat4x4( &wm );
}

//set the parameters as 3D scaling factors of the local reference frame
void SceneNode::setScaling(CXMVECTOR scaling )
{
	XMStoreFloat3( &mScaling, scaling );
}

//return the three scaling factors
XMMATRIX SceneNode::getScaling()
{
	return XMMatrixScaling( mScaling.x, mScaling.y, mScaling.z );
}


//
//get the direction of the z-axis of the object orientation in the given space
//TS_LOCAL: canonical vector e3
//TS_PARENT: direction of z-axis is parent space = 3. vector of orientation matrix
//TS_WORLD: 3rd vector or world matrix
//
XMVECTOR SceneNode::getDirectionZ( TransformSpace ts )
{
	if( ts == TS_LOCAL ) {				//direction of z in local space
		return XMVectorSet(0,0,1, 0);
	}

	if( ts == TS_PARENT ) {				//direction of z in parent space
		return XMVectorSet( mOrientation._31, mOrientation._32, mOrientation._33, 0 );
	}

	//direction of z in world space
	XMMATRIX world = getTransform( TS_WORLD );
	XMFLOAT4X4 wt;
	XMStoreFloat4x4( &wt, world );

	return XMVectorSet( wt._31, wt._32, wt._33, 0 );
}

//
//compute the inverse transpose of a matrix
//needed to transform normal vectors
//
XMMATRIX SceneNode::computeInverseTranspose( CXMMATRIX matrix)
{
	XMVECTOR det = XMMatrixDeterminant( matrix );		//determinant
	XMMATRIX inv =  XMMatrixInverse( &det, matrix );	//inverse
	return XMMatrixTranspose( inv );					//transpose
}

//
//look at target point (w=1)
//point is given in a certain space
//need to transform the point to parent space first
//TS_LOCAL: use parent info
//TS_PARENT: do not transform the point
//TS_WORLD: use inverse of world matrix, then parent info
//
void SceneNode::lookAt( CXMVECTOR target, TransformSpace ts )
{
    XMVECTOR up  = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);				//up vector to keep camera upside

	XMVECTOR tg = changeToParentSpace(target, ts);					//transform to parent space

	XMVECTOR pos = getPosition( TS_PARENT );						//get own position in parent space

	XMMATRIX V = XMMatrixLookAtLH( pos, tg, up);					//get the new VIEW matrix in parent space

	XMVECTOR det = XMMatrixDeterminant( V );						//new orientation is the INVERSE of the view matrix
	V = XMMatrixInverse( &det, V );

	XMStoreFloat3x3( &mOrientation, V );
}


//
//get a transform from local space to the requested space
//TS_LOCAL: identity matrix
//TS_PARENT: transform from local space to parent space
//TS_WORLD: transfrm from local space to world space
//
XMMATRIX SceneNode::getTransform( TransformSpace ts )
{
	if( ts == TS_LOCAL ) {				//from local to local
		return XMMatrixIdentity();
	}
	if( ts == TS_PARENT ) {				//from local to parent space
		return getScaling() * getOrientation() * XMMatrixTranslationFromVector( getPosition() );
	}

	//from local to world space
	XMMATRIX world = getTransform( TS_PARENT );				//call itself once to get parent transform
	if( mParent ) {
		world = world * mParent->getTransform( TS_WORLD );	//recursion up to root scene node
	}
	return world;
}

//add a new child scene node
void SceneNode::addChild(shared_ptr<SceneNode> node)
{
    mAttachedNodes.push_back(node);
	shared_ptr<SceneNode> p = SceneManager::getInstance()->getSceneNode( getName() );	//do not use "this"!
	node->setParent( p );
}

//remove a child scene node
//the child scene node remains stored in the scene manager!
void SceneNode::removeChild(std::string nodeName)
{
    std::deque<shared_ptr<SceneNode>>::iterator itNodes = mAttachedNodes.begin();	//go through all children
    while (mAttachedNodes.end() != itNodes)
    {   
        if( strcmp((*itNodes)->getName().c_str(), nodeName.c_str()) == 0)	//compare names
        {
			(*itNodes)->setParent( shared_ptr<SceneNode>());				//set parent to NULL
            mAttachedNodes.erase(itNodes);									//if found remove from collection			
            break;
        }
        ++itNodes;
    }
}

//attach a movable object to a scene node
void SceneNode::attachObject(shared_ptr<MovableObject> object)
{
	shared_ptr<SceneNode> old = object->getSceneNode();		//if already attached, detach it
	if( old ) old->detachObject( object->getName() );

    mAttachedObjects.push_back(object);

	object->attachToSceneNode( SceneManager::getInstance()->getSceneNode( getName() ) );
}

//detach a movable object from a scene node
void SceneNode::detachObject(std::string nodeName)
{
    std::deque<shared_ptr<MovableObject>>::iterator itObjects = mAttachedObjects.begin();	//find object
    while (mAttachedObjects.end() != itObjects)
    {   
        if( strcmp((*itObjects)->getName().c_str(), nodeName.c_str()) == 0)
        {
			(*itObjects)->detachFromSceneNode();	//detach object from this scene node
            mAttachedObjects.erase(itObjects);		//remove from collection
            break;
        }
        ++itObjects;
    }
}

//draw all attached movable objects but do not use any preset transformation
//This is probably the root scene node
void SceneNode::Draw( DrawType dt )
{
	Draw( XMMatrixIdentity(), dt );
}

//draw all attached movable objects
//transform is the world matrix of the parent scene node
//the world matrix of this scene node is 
//this world matrix times the own parent information
void SceneNode::Draw( CXMMATRIX transform, DrawType dt )
{
	XMMATRIX nextTransform = getTransform( TS_PARENT ) * transform;		//get own world matrix

    std::deque<shared_ptr<SceneNode>>::iterator itNodes = mAttachedNodes.begin();	//depth first drawing of subnodes
    while (mAttachedNodes.end() != itNodes)
    {   
        (*itNodes)->Draw( nextTransform, dt );		//call draw for all attached scene nodes
        ++itNodes;
    }

    std::deque<shared_ptr<MovableObject>>::iterator itObjects = mAttachedObjects.begin();	//draw movableobjects
    while (mAttachedObjects.end() != itObjects)
    {   
        (*itObjects)->Draw( nextTransform, dt );		//call draw for all attached movable objects
        ++itObjects;
    }
}

//
//translate the scene node by a vector (w=0) that is given in a certain space
//since the position and orientation are in parent space, the vector must be
//first transformed to parent space
//
//TS_LOCAL: vector is given in local space
//TS_PARENT: vector is given is parent space
//TS_WORLD: vector is given in world space
//
void SceneNode::translate( CXMVECTOR trans, TransformSpace ts )
{
	XMVECTOR vec = changeToParentSpace( trans, ts );	//change the translation vector to parent space

	XMVECTOR pos = XMLoadFloat3( &mPosition ) + vec;	//carry out translation
	XMStoreFloat3( &mPosition, pos );					//store new position
}


//rotate by an axis in parent space
void SceneNode::rotateAxis( CXMVECTOR axis, FLOAT deg, TransformSpace ts )
{
	XMVECTOR vec = changeToParentSpace( axis, ts );		//change axis vector to parent space

	XMMATRIX o =  XMLoadFloat3x3( &mOrientation ) * XMMatrixRotationAxis( vec, deg );
	XMStoreFloat3x3( &mOrientation, o );
}

//
//Roll the object in parent space through z-axis
//z-axis vector is stored in parent space already in the mOrientation matrix
//
void SceneNode::roll( FLOAT deg )
{
	XMMATRIX o = XMLoadFloat3x3( &mOrientation ) * XMMatrixRotationAxis( XMVectorSet(mOrientation._31,mOrientation._32,mOrientation._33, 0.0f), deg );
	XMStoreFloat3x3( &mOrientation, o );
}

//
//Change pitch in parent space through x-axis
//x-axis vector is stored in parent space already in the mOrientation matrix
//
void SceneNode::pitch( FLOAT deg)
{
	XMMATRIX o =   XMLoadFloat3x3( &mOrientation ) * XMMatrixRotationAxis( XMVectorSet(mOrientation._11,mOrientation._12,mOrientation._13, 0.0f), deg );
	XMStoreFloat3x3( &mOrientation, o );
}

//
//yaw the object in parent space using y-axis
//y-axis vector is stored in parent space already in the mOrientation matrix
//
void SceneNode::yaw( FLOAT deg )
{
	XMMATRIX o = XMLoadFloat3x3( &mOrientation ) * XMMatrixRotationAxis( XMVectorSet(mOrientation._21,mOrientation._22,mOrientation._23, 0.0f), deg );
	XMStoreFloat3x3( &mOrientation, o );
}


//
//transform a vector (w=0) or point (w=1) to parent space
//vector/point is given in a certain space
//
//TS_LOCAL: vector is given in local space
//TS_PARENT: vector is given is parent space
//TS_WORLD: vector is given in world space
//
XMVECTOR SceneNode::changeToParentSpace( CXMVECTOR vecOrPoint, TransformSpace ts )
{
	XMVECTOR vop = vecOrPoint;

	if( ts == TS_LOCAL ) {												//translation vector is given in local space
		XMMATRIX tf = getTransform( TS_PARENT );
		vop = XMVector4Transform( vop, tf );							//transform to parent space
		return vop;	
	}

	if( ts == TS_PARENT ) {												//translation vector is given in parent space
		//nothing to be done
		return vop;
	}

	//TS_WORLD
	XMMATRIX world = getTransform( TS_WORLD );					//get world transform

	XMVECTOR det = XMMatrixDeterminant( world );
	XMMATRIX worldinv = XMMatrixInverse( &det, world );			//inverse of world matrix

	vop = XMVector4Transform( vop, worldinv );					//transform to local space
	vop = XMVector4Transform( vop, getTransform( TS_PARENT ) );	//transform to parent space

	return vop;
}


//
//transform a vector (w=0) or point (w=1) to local space
//vector/point is given in a certain space
//
//TS_LOCAL: vector is given in local space
//TS_PARENT: vector is given is parent space
//TS_WORLD: vector is given in world space
//
XMVECTOR SceneNode::changeToLocalSpace( CXMVECTOR vecOrPoint, TransformSpace ts )
{
	XMVECTOR vop = vecOrPoint;

	if( ts == TS_LOCAL ) {												
		//nothing to be done
		return vop;
	}

	XMMATRIX transform;

	if( ts == TS_PARENT ) {												//vec is given in parent space
		transform = getTransform( TS_PARENT );
	} else {
		//TS_WORLD
		transform = getTransform( TS_WORLD );							//vec is given in world space
	}

	XMVECTOR det = XMMatrixDeterminant( transform );
	XMMATRIX transinv = XMMatrixInverse( &det, transform );				//inverse of world matrix

	return XMVector4Transform( vop, transinv );							//transform to local space
}


