#pragma once

#include "D3DUtility.h"
#include "GameObject.h"

namespace Render {

//
//Call back base class. FrameListeners are called when events are received
//
class FrameListener : GameObject
{
public:
	RENDERDLL_API FrameListener( std::string name );
	RENDERDLL_API ~FrameListener(void);

	virtual RENDERDLL_API void frameStarted( float dt )=0;		//called whenever a new frame is rendered
	virtual RENDERDLL_API LRESULT MsgProc(HWND hwnd, UINT msg,	//message procedure for Windows callbacks
							WPARAM wParam, LPARAM lParam);

protected:
		std::vector<WPARAM> mKeysPressed;			//message procedure fills in all pressed keys

protected:
	   // Convenience overrides for handling mouse input.
    virtual void OnMouseDown(WPARAM btnState, int x, int y){ };
    virtual void OnMouseUp(WPARAM btnState, int x, int y)  { };
    virtual void OnMouseMove(WPARAM btnState, int x, int y){ };
};

}

