

#include <windows.h>
#include "Root.h"
#include "SceneManager.h"
#include "Window.h"
#include "RenderSystem.h"
#include <sstream>
#include <vector>
#include "Camera.h"
#include "Viewport.h"

using namespace Render;

Root* Root::theInstance = NULL;


// Constructor setting default values and zeroing structures.
Root::Root( HINSTANCE hInstance) :	
	mhAppInst(hInstance),
    mAppPaused(false)

{
}

// Destructor 
Root::~Root()
{
}

//Singleton pattern
Root *Root::getInstance()
{
    if ( Root::theInstance==NULL ) {	//if there is no Root instance yet, create one
        Root::theInstance = new Root(GetModuleHandle(NULL));

    if( !Root::theInstance->init() )	//initialize it, i.e. create a window and a render system
        exit(1);
    }
    return Root::theInstance;
}


//
// The game loop.
//
int Root::startRendering()
{
    MSG msg = {0};
 
    mTimer.Reset();		//timer for getting frame duration time

    while(msg.message != WM_QUIT)
    {
        // If there are Window messages then process them.
        if(PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );	//this calls WinMsg of Root and FrameListeners
        }
        // Otherwise, do animation/game stuff.
        else
        {	
            mTimer.Tick();		//get frame time

            if( !mAppPaused )
            {
                CalculateFrameStats();
                RenderOneFrame(mTimer.DeltaTime());	 //render the next frame
            }
            else
            {
                Sleep(100);
            }
        }
    }

    return (int)msg.wParam;
}

//pause the render loop
void Root::pauseApp() 
{
    mAppPaused = true;
    mTimer.Stop();
}

//restart the render loop
void Root::startApp()
{
    mAppPaused = false;
    mTimer.Start();
}


//create a window and a render system (=DirectX) and initialize them
bool Root::init()
{
	Window::getInstance()->init();	//have to do this first since we need a window for DirectX!

	RenderSystem::getInstance()->init(); //init DirectX

    return true;
}

//read configuration information from disk
bool Root::Config()
{
    return true;
}

//
//Process a message received from Windows
//Forward it to potential consumers, i.e. the registered frame listeners
//Until one of them finally consumes it.
//
LRESULT Root::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   for( UINT i=0; i<mFrameListeners.size(); i++ ) {
        if( mFrameListeners[i] != NULL ) {
            if( !mFrameListeners[i]->MsgProc( hwnd, msg, wParam, lParam ) ) {
                return 0;
			}
		}
   }
   return 1;
}


//calculate frame statistics
void Root::CalculateFrameStats()
{
    // Code computes the average frames per second, and also the 
    // average time it takes to render one frame.  These stats 
    // are appended to the window caption bar.

    static int frameCnt = 0;
    static float timeElapsed = 0.0f;

    frameCnt++;

    // Compute averages over one second period.
    if( (mTimer.TotalTime() - timeElapsed) >= 1.0f )
    {
        float fps = (float)frameCnt; // fps = frameCnt / 1
        float mspf = 1000.0f / fps;

		Window *win = Window::getInstance();

        std::wostringstream outs;   
        outs.precision(6);
        outs << win->getWindowCaption() << L"    "
             << L"FPS: " << fps << L"    " 
             << L"Frame Time: " << mspf << L" (ms)";
        SetWindowText(win->getMainWnd(), outs.str().c_str()); //output in current window bar
        
        // Reset for next average.
        frameCnt = 0;
        timeElapsed += 1.0f;
    }
}


//add a new frame listener to the list
void Root::addFrameListener( FrameListener * listener )
{
    mFrameListeners.push_back( listener );
}

//render the next frame
void Root::RenderOneFrame(float dt)
{
    for( UINT i=0; i<mFrameListeners.size(); i++ )	//notify all listeners that frame is started
        if( mFrameListeners[i] != NULL ) 
            mFrameListeners[i]->frameStarted( dt );

	SceneManager *sm = SceneManager::getInstance();
	RenderSystem *rs = RenderSystem::getInstance();

	rs->clearRenderTarget();	//clear the back buffer

	sm->drawScene();			//ask scene manager to draw the scene into the back buffer

	rs->presentBackbuffer();    //make the back buffer visible
}


