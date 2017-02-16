#pragma once

#include "MovableObject.h"

namespace Render {

class SceneManager;

//
//Camera class represents cameras that take snapshots of a scene
//either for rendering or creating a shadow map
//Camera is a pure abstract class and must be either projection or orthographic
class Camera : public MovableObject
{
	friend SceneManager;

public:
	RENDERDLL_API ~Camera(void);

	RENDERDLL_API virtual XMMATRIX getViewMatrix()=0;			//returns view matrix, pure abstract
	RENDERDLL_API virtual XMMATRIX getProjectionMatrix()=0;	//returns camera projection matrix, pure abstract

	RENDERDLL_API void setAspectRatio( float aspect ){ mAspect = aspect; };	//set newaspect ratio of window changes
	RENDERDLL_API float getAspectRatio(){ return mAspect; };	//set newaspect ratio of window changes
	RENDERDLL_API void setNearPlane( float fnear ){ mNear = fnear; };	//set newaspect ratio of window changes
	RENDERDLL_API float getNearPlane(){ return mNear; };	//set newaspect ratio of window changes
	RENDERDLL_API void setFarPlane( float ffar ){ mFar = ffar; };	//set newaspect ratio of window changes
	RENDERDLL_API float getFarPlane(){ return mFar; };	//set newaspect ratio of window changes

	virtual RENDERDLL_API std::vector<XMFLOAT3> getFrustum()=0;							//Get frustum points
	virtual RENDERDLL_API void setFrustumToPoints( std::vector<XMFLOAT3> points )=0;		//

protected:
	//constructor only called from SceneManager
	Camera( std::string name, float near, float far );
	
	float mNear;	//near plane
	float mFar;		//far plane
	float mAspect;	//aspect ratio
};

}

