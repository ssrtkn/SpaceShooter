

#include "Render.h"
#include "myFrameListener.h"

using namespace Render;

//The program starts HERE
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	Root *myRoot = Root::getInstance();


	myFrameListener myListener("FL1");
	myRoot->addFrameListener(&myListener);

	myRoot->startRendering();

}


