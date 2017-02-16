#pragma once

#include "Camera.h"

namespace Render {

class Viewport : public GameObject
{
public:
	RENDERDLL_API Viewport( std::string name, float topleft, float topright, float width, float height );
	RENDERDLL_API ~Viewport(void);
	RENDERDLL_API void setCamera( shared_ptr<Camera> cam ){ mCamera = cam; };	//set camera of this viewport
	RENDERDLL_API shared_ptr<Camera> getCamera(){ return mCamera; };			//get current camera
    RENDERDLL_API D3D11_VIEWPORT getD3DViewportStruct();						//return D3D11_VIEWPORT with viewport data

protected:
	shared_ptr<Camera> mCamera;	//current camera

	float mTopleft;	//normalized, between 0 and 1
	float mTopup;	//normalized, between 0 and 1
	float mWidth;	//normalized, between 0 and 1
	float mHeight;	//normalized, between 0 and 1
};

}

