#pragma once

#include "D3DUtility.h"
#include "Camera.h"

namespace Render {

class SceneManager;

//
//Camera representing an orthographic projection, neede for creating the shadow map
//
class CameraOrtho : public Camera
{
	friend SceneManager;

public:
	RENDERDLL_API ~CameraOrtho(void);

	virtual RENDERDLL_API XMMATRIX getViewMatrix();			//returns view matrix
	virtual RENDERDLL_API XMMATRIX getProjectionMatrix();		//returns camera projection matrix
	virtual RENDERDLL_API std::vector<XMFLOAT3> getFrustum();
	virtual RENDERDLL_API void setFrustumToPoints( std::vector<XMFLOAT3> points );

	RENDERDLL_API void setWidth( float width ){ mWidth = width;};
	RENDERDLL_API float getWidth(){ return mWidth; };

protected:
	CameraOrtho( std::string name, float near, float far, float width  );

	float mWidth;
};

}
