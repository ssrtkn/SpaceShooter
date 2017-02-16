#pragma once

#include "D3DUtility.h"
#include "GameObject.h"
//#include "SceneManager.h"
#include <deque>
#include <string>

namespace Render {

class MovableObject;
class SceneManager;

//Represents positions and orientations in the scene
class SceneNode : GameObject
{
	friend SceneManager;

public:
 	enum  TransformSpace { TS_LOCAL, TS_PARENT, TS_WORLD };		//reference frame used
 	enum  DrawType { DT_LIGHT, DT_SHADOW };						//draw for light or shadow pass

    RENDERDLL_API virtual ~SceneNode(void);									//destructor

    RENDERDLL_API void setPosition(	CXMVECTOR position,						//set the position in parent space
									TransformSpace ts = TS_PARENT);	
    RENDERDLL_API XMVECTOR getPosition( TransformSpace ts = TS_PARENT );		//get the position in desired space
	RENDERDLL_API void setOrientation(	CXMMATRIX orientation, 				//set orientation in parent space
										TransformSpace ts = TS_PARENT);	
    RENDERDLL_API XMMATRIX getOrientation( TransformSpace ts = TS_PARENT );	//get orientation in desired space
	RENDERDLL_API void setScaling(CXMVECTOR scaling );						//set scaling along the parent axes
	RENDERDLL_API XMMATRIX getScaling();										//get scaling matrix
	RENDERDLL_API XMMATRIX getTransform( TransformSpace ts = TS_PARENT );		//get the transform fom local space to desired space
	RENDERDLL_API XMVECTOR getDirectionZ( TransformSpace ts = TS_PARENT );	//return z-axis vector in desired space

	RENDERDLL_API void translate( CXMVECTOR dv, TransformSpace ts = TS_PARENT );	//translate the scene node
	RENDERDLL_API void rotateAxis( CXMVECTOR axis, FLOAT deg, TransformSpace ts = TS_PARENT );	//rotate the scene node
	RENDERDLL_API void roll( FLOAT deg );		//rotate via z-axis
	RENDERDLL_API void pitch( FLOAT deg);		//rotate via x-axis
	RENDERDLL_API void yaw( FLOAT deg );		//rotate via y-axis
	RENDERDLL_API void lookAt( CXMVECTOR target, TransformSpace ts = TS_WORLD );				//look at a point in space
	RENDERDLL_API XMVECTOR changeToParentSpace( CXMVECTOR vecOrPoint, TransformSpace ts );	//change vector coordinates from any space to parent space
	RENDERDLL_API XMVECTOR changeToLocalSpace( CXMVECTOR vecOrPoint, TransformSpace ts );		//change vector coordinates from any space to local space

    RENDERDLL_API void addChild(shared_ptr<SceneNode> node);			//add a child scene node
    RENDERDLL_API void removeChild(std::string nodeName);				//remove a child scene node
	RENDERDLL_API void attachObject(shared_ptr<MovableObject> node);	//attach a movable object to the scene node
    RENDERDLL_API void detachObject(std::string nodeName);				//detach a movable object from the scene node

	static RENDERDLL_API XMMATRIX computeInverseTranspose( CXMMATRIX matrix);	//

protected:
	void Draw( DrawType dt = DT_LIGHT);							//draw the movable objects attached to this scene node
	void Draw( CXMMATRIX transform, DrawType dt = DT_LIGHT );	//draw the movable objects attached to this scene node
	SceneNode(std::string name);								//constructor
	void setParent( shared_ptr<SceneNode> sn ){ mParent = sn; };//store the parent of this node

public:
	shared_ptr<SceneNode> mParent;	//the parent scene node
    XMFLOAT3 mPosition;				//position of node in PARENT space
    XMFLOAT3X3 mOrientation;		//orientation of node in PARENT space
	XMFLOAT3 mScaling;				//scaling along the axes

    std::deque<shared_ptr<SceneNode>> mAttachedNodes;			//list of all attached scene nodes (results in a scene graph)
    std::deque<shared_ptr<MovableObject>> mAttachedObjects;		//list of all attached objects, e.g. entities, cameras, lights...		
};

}

