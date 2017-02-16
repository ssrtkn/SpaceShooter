#pragma once

#include "Viewport.h"


namespace Render {


class Root;
class Window;
class SceneManager;

//
//The class RenderSystem is an abstraction of DirectX
//Could also abstract other systems like OpenGL()
//
class RenderSystem
{
	friend Root;
	friend Window;
	friend SceneManager;

public:
	RENDERDLL_API ~RenderSystem(void);
	static RENDERDLL_API RenderSystem *getInstance();	//singleton!

	//D3D stuff
	RENDERDLL_API ID3D11DevicePtr getDevice() {return md3dDevice;};
	RENDERDLL_API ID3D11DeviceContextPtr getDeviceContext(){ return md3dImmediateContext; };

protected:
	RenderSystem(void);				//only called from self, singleton!
	void init();					//init the D3D
	void OnResize();				//called when the window size changes, recreate buffers
	void resetRenderTarget();
	void clearRenderTarget();		//clear backbuffer 
	void clearDepthBuffer();		//clear depth buffer
	void presentBackbuffer(){  HR(mSwapChain->Present(0, 0)); };	//present the backbuffer
	void setViewport( shared_ptr<Viewport> vp );	//define a new viewport transform

protected:

	static RenderSystem *theInstance;	//singleton

	    // D3D stuff.
    ID3D11DevicePtr			md3dDevice;						//the D3Ddevice, entry point into D3D!
    ID3D11DeviceContextPtr	md3dImmediateContext;			//immediate context for drawing
    IDXGISwapChainPtr		mSwapChain;						//double buffering
	ID3D11Texture2DPtr		mDepthStencilBuffer;			//depth/stencil buffer
	ID3D11DepthStencilStatePtr	mDepthStencilState;			//state of depth/stencil buffer
    ID3D11RenderTargetView*		mRenderTargetView;			//view for double buffers
	ID3D11DepthStencilViewPtr	mDepthStencilView;			//view for depth/stencil buffer
    D3D11_VIEWPORT				mScreenViewport;			//the current viewport transform
    D3D_DRIVER_TYPE				md3dDriverType;				//info about the device
};


}

