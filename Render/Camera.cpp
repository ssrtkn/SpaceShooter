

#include "D3DUtility.h"
#include "Camera.h"
#include "SceneNode.h"
#include "Window.h"

using namespace Render;

//constructor
Camera::Camera( std::string name, float nearplane, float farplane ) : MovableObject(name),
		mNear(nearplane),
		mFar(farplane)
{
	Window *win = Window::getInstance();
	mAspect = win->getAspectRatio();
}

//destructor
Camera::~Camera(void)
{}

