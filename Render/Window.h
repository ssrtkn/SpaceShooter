#pragma once

#include "D3DUtility.h"


namespace Render {

class Root;
class RenderSystem;
class Viewport;
class SceneManager;

//
//Class Window represents a Windows window that is managed by DirectX
//
class Window
{
	friend Root;
	friend RenderSystem;
	friend SceneManager;

public:
	RENDERDLL_API ~Window(void);
	RENDERDLL_API void setBGColor( XMFLOAT4 color ){ mBGColor = color; };
	RENDERDLL_API XMFLOAT4 getBGColor(){ return mBGColor; };

	RENDERDLL_API LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam); //public for callback
	RENDERDLL_API static Window *getInstance();					//singleton!
	RENDERDLL_API int    getWidth(){ return mClientWidth; };		//return window width
	RENDERDLL_API int    getHeight(){ return mClientHeight; };	//return window height
	RENDERDLL_API float  getAspectRatio(){ return static_cast<float>(mClientWidth) / mClientHeight; };	//return aspect ratio
	RENDERDLL_API std::wstring getWindowCaption(){ return mMainWndCaption; };	//return window caption

	RENDERDLL_API shared_ptr<Viewport> createViewPort(std::string name, float topleft, float topright, float width, float height ); //create a view port
	RENDERDLL_API shared_ptr<Viewport> getViewPort(std::string name );	//get a specific viewport using its name
	RENDERDLL_API void destroyViewport( std::string name);				//destroy a specified viewport

public:
	Window();		//constructor is protected
	void init();	//initialize the class

	HWND  getMainWnd(){ return mhMainWnd; };	// Window stuff.
	static Window *theInstance;	//singleton

protected:
	// Windowing variables.
    std::wstring mMainWndCaption;
    HWND      mhMainWnd;	//window handle
    bool      mMinimized;	//flag to remember that app is minimized
    bool      mMaximized;	//flag to remember that app is maximized
    bool      mResizing;	//flag to remember that window is currently resized with mouse
    int		  mClientWidth;	//window width
    int		  mClientHeight;//window height

	XMFLOAT4  mBGColor;		//background color, used for erasing the backbuffer

	std::vector<shared_ptr<Viewport>> mViewports;	//list of viewports
};

}

