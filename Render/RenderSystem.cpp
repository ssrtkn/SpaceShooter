
#include "D3DUtility.h"
#include "RenderSystem.h"
#include "Window.h"
#include "Viewport.h"
#include "Root.h"

using namespace Render;

RenderSystem* RenderSystem::theInstance = NULL;	//singleton instance pointer

//Constructor
RenderSystem::RenderSystem(void):
    md3dDriverType(D3D_DRIVER_TYPE_HARDWARE),
    md3dDevice(0),
    md3dImmediateContext(0),
    mSwapChain(0),
    mRenderTargetView(0)
{
   ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));
}


//Destructor
RenderSystem::~RenderSystem(void)
{
    // Restore all default settings.
    if( md3dImmediateContext )
        md3dImmediateContext->ClearState();
}


//get the singleton instance
RenderSystem *RenderSystem::getInstance()
{
    if ( RenderSystem::theInstance==NULL ) {	//if there is none so far, create it
       RenderSystem::theInstance = new RenderSystem( );
	}
    return RenderSystem::theInstance;
}


//
//Initialize DirectX
//
void RenderSystem::init()
{
    // ToDo: Create a D3D11 Device using following variables:
    // mSwapChain
    // md3dDevice
    // md3dImmediateContext
    // ALSO:
    // Answer following questions here in the comments (keep it to a few sentences):
    //
    // Q: What will happen if you try to use a D3D_FEATURE_LEVEL that is higher than the one supported by your GPU?
    // A: Es wird nicht funktionieren
    //
    // Q: What is the difference between the D3D11Device and the D3D11DeviceContext? Describe when you need to use which one. 
    // A: Device ist Interface zum Adapter und DContext  zeichnet(rendering) die gegebene Befehle.
    //
    // Q: What is the difference between a D3D resource and a D3D view?
    // A: Resource ist die Materiell und View ist Bescheibung der Materiell

	Window *win = Window::getInstance();		//you need a window for this!

	UINT createDeviceFlags = 0;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC sd; 

	sd.BufferDesc.Width = win->getWidth();
	sd.BufferDesc.Height = win->getHeight();
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = win->getMainWnd();
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; 
	sd.Flags = 0;

    // ToDo: Create the device and device context.
    // Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.
    // Create the D3D device and the swap chain and assign result to hr.

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		0, md3dDriverType, 0, createDeviceFlags,0, 0,
		D3D11_SDK_VERSION,&sd,&mSwapChain,&md3dDevice,
		&featureLevel,&md3dImmediateContext);

    if( FAILED(hr) )
    {
        MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
        return;
    }
    
    OnResize();		//allocate the buffers

	return;
}


//
//Whenever a window changes its size we need to change the swap chain and depth/stencil buffer sizes
//Buffers are accessed via a view, so have to create the views as well
//
void RenderSystem::OnResize()
{
	Window *win = Window::getInstance();

    assert(md3dImmediateContext);
    assert(md3dDevice);
    assert(mSwapChain);

    // Release the old view, as it holds references to the buffer we
    // will be destroying.
    ReleaseCOM(mRenderTargetView);

    // Resize the swap chain and recreate the render target view.
	HR(mSwapChain->ResizeBuffers(1, win->getWidth(), win->getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, 0));
    ID3D11Texture2DPtr backBuffer;
    HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
    HR(md3dDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView)); //cannot use a smart pointer here :-(


    // ToDo: Create depth/stencil buffer texture.
    D3D11_TEXTURE2D_DESC depthBufferDesc; // <- ToDo: fill out with proper values (look at the MSDN docs) 
	
	depthBufferDesc.Width = win->getWidth();
	depthBufferDesc.Height = win->getHeight();
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	
	
	HR(md3dDevice->CreateTexture2D(&depthBufferDesc, 0, &mDepthStencilBuffer));	// create the depth/stencil 2D buffer and put it into mDepthStencilBuffer

    // ToDo: Create a depth/stencil view.
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc)); // Initailze the depth stencil view.
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Set up the depth stencil view description.
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    HR(md3dDevice->CreateDepthStencilView(mDepthStencilBuffer, &depthStencilViewDesc, &mDepthStencilView));

	//create a depth-stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;	// Set up the description of the stencil state.
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HR( md3dDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState) ); // Create the depth stencil state.
	md3dImmediateContext->OMSetDepthStencilState(mDepthStencilState, 1); //bind to output merger stage


    // Bind the render target view to the pipeline.
    md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
    
    // Set the default viewport transform, covering the whole window
    mScreenViewport.TopLeftX = 0;
    mScreenViewport.TopLeftY = 0;
    mScreenViewport.Width    = static_cast<float>(win->getWidth());
    mScreenViewport.Height   = static_cast<float>(win->getHeight());
    mScreenViewport.MinDepth = 0.0f;
    mScreenViewport.MaxDepth = 1.0f;
    md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
}
 

//
//Needed to switch back to normal rendering after rendering into the shadow map
//
void RenderSystem::resetRenderTarget()
{
	md3dImmediateContext->RSSetState(0);

	ID3D11RenderTargetView* renderTargets[1] = {mRenderTargetView};
	md3dImmediateContext->OMSetRenderTargets(1, renderTargets, mDepthStencilView);	//normal render target
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);						//default viewport
}

//
//Clear the backbuffer, also the depth map
//This is called from Root
//
void RenderSystem::clearRenderTarget()
{
	Window *win = Window::getInstance();

	FLOAT bg[4];
	bg[0] = win->getBGColor().x;
	bg[1] = win->getBGColor().y;
	bg[2] = win->getBGColor().z;
	bg[3] = win->getBGColor().w;

    // Clear the render target.
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, bg );
    md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
}

//only clear the depth buffer, needed before rendering into the shadow map
void RenderSystem::clearDepthBuffer()
{
	Window *win = Window::getInstance();
    md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

}

//
//set ouput viewport to a specific view port
//
void RenderSystem::setViewport( shared_ptr<Viewport> vp )
{
	mScreenViewport = vp->getD3DViewportStruct();				//get the D3D viewport struct describing the VP
    md3dImmediateContext->RSSetViewports(1, &mScreenViewport);	//set the D3D viewport
}

