
#include "D3DUtility.h"
#include "Window.h"
#include "Root.h"
#include "RenderSystem.h"
#include "Viewport.h"

using namespace Render;

Window* Window::theInstance = NULL;


//
//Wrapping function called by Windows OS to notify a window of an event
//The Wrapper forwards this event to the Window class message procedure
//This procedure either consumes the event (like closing the window)
//or forwards it to the Root class, the Root class then to the registered frame
//listeners
//
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Forward hwnd on because we can get messages (e.g., WM_CREATE)
    // before CreateWindow returns, and thus before mhMainWnd is valid.
    return Window::getInstance()->MsgProc(hwnd, msg, wParam, lParam);
}

//Class constructor
Window::Window() : 
    mMainWndCaption(L"REPAI Engine"),	//Main window caption
	mhMainWnd(0),						//window handle
    mMinimized(false),					//window params
    mMaximized(false),
    mResizing(false), 
    mClientWidth(800),					//default width
    mClientHeight(600)					//default height
{
    FLOAT color[4] = {0,0,0,0};			//black
    setBGColor( color );				//use black to erase back buffer
}

//destructor
Window::~Window(void)
{
}

//singleton pattern
Window *Window::getInstance()
{
    if ( Window::theInstance==NULL ) {		//if there is no window create one

		Root *root = Root::getInstance();
        Window::theInstance = new Window();
	}
    return Window::theInstance;
}

//
//initialize the Window
//
void Window::init()
{
	Root *root = Root::getInstance();		//we need Root

	WNDCLASS wc;							//fill out this struct
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = MainWndProc; 
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
	wc.hInstance     = root->getAppInst();	//need app instance
    wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    wc.lpszMenuName  = 0;
    wc.lpszClassName = L"D3DWndClassName";

    if( !RegisterClass(&wc) )				//register the Windows class
    {
        MessageBox(0, L"RegisterClass Failed.", 0, 0);
        return;
    }

    // Compute window rectangle dimensions based on requested client area dimensions.
    RECT R = { 0, 0, mClientWidth, mClientHeight };
    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
    int width  = R.right - R.left;
    int height = R.bottom - R.top;

    mhMainWnd = CreateWindow(L"D3DWndClassName",	//create window and store handle
					mMainWndCaption.c_str(), 
					WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, root->getAppInst(), 0); 
    if( !mhMainWnd )
    {
        MessageBox(0, L"CreateWindow Failed.", 0, 0);
        return;
    }

    ShowWindow(mhMainWnd, SW_SHOW);		//make window visible
    UpdateWindow(mhMainWnd);

    return;

}

//
//Internal message procedure called from Wrapper function above
//
LRESULT Window::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Root *root = Root::getInstance();
	RenderSystem * rs = RenderSystem::getInstance();

    switch( msg )
    {
    // WM_ACTIVATE is sent when the window is activated or deactivated.  
    // We pause the game when the window is deactivated and unpause it 
    // when it becomes active.  
    case WM_ACTIVATE:
        if( LOWORD(wParam) == WA_INACTIVE )
        {
            root->pauseApp();
        }
        else
        {
			root->startApp();
        }
        return 0;

    // WM_SIZE is sent when the user resizes the window.  
    case WM_SIZE:
        // Save the new client area dimensions.
        mClientWidth  = LOWORD(lParam);
        mClientHeight = HIWORD(lParam);
		if( rs->getDevice() ) {
	        if( wParam == SIZE_MINIMIZED )	//if minimzed then pause the app
		    {
			    root->pauseApp();
				mMinimized = true;
	            mMaximized = false;
		    }
			else if( wParam == SIZE_MAXIMIZED )	//if maximized then make sure app is running
			{
				root->startApp();
	            mMinimized = false;
		        mMaximized = true;
			    rs->OnResize();					//react to resize by reallocating back buffer
	        }
		    else if( wParam == SIZE_RESTORED )	//size is restored from minized state
			{
				// Restoring from minimized state?
				if( mMinimized )
				{
					root->startApp();
					mMinimized = false;
					rs->OnResize();				//react to resize by reallocating back buffer
				}
				// Restoring from maximized state?
				else if( mMaximized )
				{
					root->startApp();
					mMaximized = false;
					rs->OnResize();				//react to resize by reallocating back buffer
				}
				else if( mResizing )
				{
                    // If user is dragging the resize bars, we do not resize 
                    // the buffers here because as the user continuously 
                    // drags the resize bars, a stream of WM_SIZE messages are
                    // sent to the window, and it would be pointless (and slow)
                    // to resize for each WM_SIZE message received from dragging
                    // the resize bars.  So instead, we reset after the user is 
                    // done resizing the window and releases the resize bars, which 
                    // sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					rs->OnResize();
				}
			}
        }
        return 0;

    // WM_ENTERSIZEMOVE is sent when the user grabs the resize bars.
    case WM_ENTERSIZEMOVE:
		root->pauseApp();
        mResizing  = true;
        return 0;

    // WM_EXITSIZEMOVE is sent when the user releases the resize bars.
    // Here we reset everything based on the new window dimensions.
    case WM_EXITSIZEMOVE:
		root->startApp();
        mResizing  = false;
        rs->OnResize();
        return 0;
 
    case WM_KEYDOWN:	//was ESC pressed? Then close the app
        if( wParam == VK_ESCAPE ) {
            ::DestroyWindow(hwnd);
			return 0;
		}
        break;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		SetCapture(hwnd); // Capture mouse movement and mouse button up events outside the application window
		break;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		ReleaseCapture(); // Stop capturing mouse movement and mouse button up events outside the application window
		break;

    case WM_DESTROY: 		// In the case of a destroy message, then
        ::PostQuitMessage(0); 
        return 0;
    }

	if( !root->MsgProc( hwnd, msg, wParam, lParam ) )	//send to frame listeners!
		return 0;
 
    return DefWindowProc(hwnd, msg, wParam, lParam);	//if no listeners consumed the message, send to default
}


//
//Create a viewport for the window
//This is like a foto paper that receives snapshots from a camera
//The floats are between 0 and 1
//
shared_ptr<Viewport> Window::createViewPort( std::string name, float topleft, float topright, float width, float height )
{
	shared_ptr<Viewport> vp = getViewPort( name );	//check if the name exists already
	if( getViewPort( name ) != NULL )				//yes return typed NULL pointer
		return shared_ptr<Viewport>();

	vp = shared_ptr<Viewport>(new Viewport(name, topleft, topright, width, height )); //else create the new viewport
	mViewports.push_back( vp );						//add to list
	return vp;
}


//find a view port given its name
shared_ptr<Viewport> Window::getViewPort(std::string name )
{
	std::vector<shared_ptr<Viewport>>::iterator it = mViewports.begin();	//use iterators
	while( it != mViewports.end() ) {
		if( (*it)->getName() == name ) {
			return *it;
		}
		++it;
	}
	return shared_ptr<Viewport>();
}


//destroy a certain view port
void Window::destroyViewport( std::string name)
{
	std::vector<shared_ptr<Viewport>>::iterator it = mViewports.begin();
	while( it != mViewports.end() ) {
		if( (*it)->getName() == name ) {
			mViewports.erase( it );
			return;
		}
	}
}
