

#include "D3DUtility.h"
#include "GameObject.h"
#include "Viewport.h"
#include "Window.h"

using namespace Render;

//constructor
Viewport::Viewport(std::string name, float topleft, float topup, float width, float height) : GameObject( name),
	mTopleft(topleft),
	mTopup(topup),
	mWidth( width ),
	mHeight( height),
	mCamera()
{
}

//destructor
Viewport::~Viewport(void)
{
}

//
//fill out a D3D struct that represents this viewport
//is used to set the D3D viewport
//
D3D11_VIEWPORT Viewport::getD3DViewportStruct()
{
	Window *win = Window::getInstance();
	D3D11_VIEWPORT screenViewport;

	   // Set the viewport transform.
    screenViewport.TopLeftX = static_cast<float>(mTopleft*win->getWidth());
    screenViewport.TopLeftY = static_cast<float>(mTopup*win->getHeight());
    screenViewport.Width    = static_cast<float>(mWidth*win->getWidth());
    screenViewport.Height   = static_cast<float>(mHeight*win->getHeight());
    screenViewport.MinDepth = 0.0f;
    screenViewport.MaxDepth = 1.0f;

	if( mCamera ) {
		mCamera->setAspectRatio( (mWidth*win->getWidth()) / (mHeight*win->getHeight()) );
	}

	return screenViewport;
}




