
#include <WindowsX.h>

#include "FrameListener.h"
#include "Root.h"

using namespace Render;

//constructor
FrameListener::FrameListener( std::string name ) : GameObject(name)
{
}

//destructor
FrameListener::~FrameListener(void)
{
}

//Conveneince preprocessing for standard Windows events
LRESULT FrameListener::MsgProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
    {
		case WM_KEYDOWN:
			for( size_t i=0; i<mKeysPressed.size(); i++ ) {	//keep a pressed key only once
				if( mKeysPressed[i] == wParam )
					return 1;
			}
			mKeysPressed.push_back(wParam);
			return 0;

		case WM_KEYUP:
			for( std::vector<WPARAM>::iterator it = mKeysPressed.begin(); it!=mKeysPressed.end(); ++it ) {
				if( (*it)==wParam ) {
					mKeysPressed.erase( it);		//remove key from pressed list
					break;
				}
			}
			return 0;

	    case WM_LBUTTONDOWN:
	    case WM_MBUTTONDOWN:
	    case WM_RBUTTONDOWN:
		    OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));	//handle mouse button down 
			return 0;
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));		//handle mouse button up 
			return 0;
		case WM_MOUSEMOVE:
			OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));	//handle mouse move
		    return 0;
    }
	return 1;
}

