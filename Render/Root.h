#pragma once

#include "D3DUtility.h"
#include "GameTimer.h"
#include "FrameListener.h"
#include "Entity.h"
#include "Window.h"

#include <vector>

namespace Render {

//
//Root is the central class of the engine
//it controls the game loop and forwards events to all registered listeners
//
class Root
{
	friend Window;

public:

	~Root();										//Destructor
    static RENDERDLL_API Root* getInstance();	//singleton!
	RENDERDLL_API HINSTANCE getAppInst(){ return mhAppInst; };	//get the Win32 App instances handle

	//framework stuff
    RENDERDLL_API int startRendering();				//enter the render loop forever
	RENDERDLL_API bool Config();					//config D3D, e.g., from a file
	RENDERDLL_API void RenderOneFrame(float dt);	//render exactly one frame
	RENDERDLL_API void addFrameListener( FrameListener * listener );	//add a callback object 

	RENDERDLL_API void pauseApp();					//pause the app
	RENDERDLL_API void startApp();					//restart the app
	RENDERDLL_API bool isAppPaused(){ return mAppPaused; };		//is the app paused?

protected:
    Root(HINSTANCE hInstance);						//only called by self (singleton)
    bool init();									//create window and render system
    void CalculateFrameStats();						//compute frame statistics
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam); //public for callback to framelisteners

protected:
	static RENDERDLL_API Root *theInstance;			//singleton!
	HINSTANCE mhAppInst;							//the Win32 application handle
    GameTimer mTimer;								// Timing, calc. delay between two consecutive frames
    bool      mAppPaused;							//if true the app is currentlypaused

	std::vector<FrameListener *> mFrameListeners;	//the registered callbacks
};


}

