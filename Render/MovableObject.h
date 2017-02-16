#pragma once

#include "D3DUtility.h"
#include "GameObject.h"
#include "SceneNode.h"

namespace Render {

//
//A movable object can be placed into the scene by attaching it to a scene node
//Represents anything in the scene, can be entity, light, terrain, camera, ...
//
class MovableObject : public GameObject
{
public:
    RENDERDLL_API MovableObject(std::string name);
    virtual RENDERDLL_API ~MovableObject(void);

	RENDERDLL_API void attachToSceneNode( shared_ptr<SceneNode> node ){ mSceneNode = node; };	//attach object to a SceneNode
	RENDERDLL_API void detachFromSceneNode(){ mSceneNode = shared_ptr<SceneNode>(); };							//detach from SceneNode
	RENDERDLL_API shared_ptr<SceneNode> getSceneNode(){ return mSceneNode; };					//get the SceneNode

	RENDERDLL_API XMVECTOR getPosition( SceneNode::TransformSpace ts = SceneNode::TS_PARENT ); //position of the object
	RENDERDLL_API XMMATRIX getOrientation( SceneNode::TransformSpace ts = SceneNode::TS_PARENT ); //orientation of the object
	virtual RENDERDLL_API void Draw( CXMMATRIX transform, SceneNode::DrawType dt ){};			//called fom SceneNode

protected:
	shared_ptr<SceneNode> mSceneNode;		//the SceneNode, this object is attached to
};

}

