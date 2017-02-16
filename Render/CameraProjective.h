#pragma once

#include "D3DUtility.h"
#include "Camera.h"

namespace Render {

class SceneManager;

//
//Camera using a projection from a central point
//This point is also the origin of the camera space
//
class CameraProjective : public Camera
{
	friend SceneManager;

public:
	RENDERDLL_API ~CameraProjective(void);

	virtual RENDERDLL_API XMMATRIX getViewMatrix();			//returns view matrix
	virtual RENDERDLL_API XMMATRIX getProjectionMatrix();		//returns camera projection matrix
	virtual RENDERDLL_API std::vector<XMFLOAT3> getFrustum(); //get points from the frustum
	virtual RENDERDLL_API void setFrustumToPoints(std::vector<XMFLOAT3> points){}; //only needed for shadow

	RENDERDLL_API void setFov( float fov ){ mFov = fov;};
	RENDERDLL_API float getFov(){ return mFov; };

protected:
	CameraProjective( std::string name, float nearplane, float farplane,  float fov  );

	float mFov;		//vertical field of view angle
};

}
