

#include "Shadow.h"
#include "RenderSystem.h"
#include "SceneManager.h"

using namespace Render;

Shadow::~Shadow(void)
{
}

Shadow::Shadow()
{
	ID3D11DevicePtr device = RenderSystem::getInstance()->getDevice();

    mViewport.TopLeftX = 0.0f;
    mViewport.TopLeftY = 0.0f;
    mViewport.Width    = static_cast<float>(SMapSize);
    mViewport.Height   = static_cast<float>(SMapSize);
    mViewport.MinDepth = 0.0f;
    mViewport.MaxDepth = 1.0f;

	// Use typeless format because the DSV is going to interpret
	// the bits as DXGI_FORMAT_D24_UNORM_S8_UINT, whereas the SRV is going to interpret
	// the bits as DXGI_FORMAT_R24_UNORM_X8_TYPELESS.
	D3D11_TEXTURE2D_DESC texDesc;
    texDesc.Width     = SMapSize;
    texDesc.Height    = SMapSize;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = numShadowmaps;
    texDesc.Format    = DXGI_FORMAT_R24G8_TYPELESS;
    texDesc.SampleDesc.Count   = 1;  
    texDesc.SampleDesc.Quality = 0;  
    texDesc.Usage          = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0; 
    texDesc.MiscFlags      = 0;

    ID3D11Texture2D* depthMap = 0;
    HR(device->CreateTexture2D(&texDesc, 0, &depthMap));

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Texture2DArray.ArraySize = 1;
	for( UINT i=0; i<numShadowmaps; i++ ) {
		ID3D11DepthStencilViewPtr depthMapDSV;

		dsvDesc.Texture2DArray.FirstArraySlice = i;
	    HR(device->CreateDepthStencilView(depthMap, &dsvDesc, &depthMapDSV));
		mDepthMapDSV.push_back( depthMapDSV );
	}

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    HR(device->CreateShaderResourceView(depthMap, &srvDesc, &mDepthMapSRV));

    // View saves a reference to the texture so we can release our reference.
    ReleaseCOM(depthMap);
}


ID3D11ShaderResourceViewPtr Shadow::getDepthMapSRV()
{
    return mDepthMapSRV;
}


void Shadow::bindDsvAndSetNullRenderTarget( UINT idx )
{
	ID3D11DeviceContextPtr dc = RenderSystem::getInstance()->getDeviceContext();

    dc->RSSetViewports(1, &mViewport);

	// Set null render target because we are only going to draw to depth buffer.
	// Setting a null render target will disable color writes.
    ID3D11RenderTargetView* renderTargets[1] = {0};
    dc->OMSetRenderTargets(1, renderTargets, mDepthMapDSV[idx]);
    
    dc->ClearDepthStencilView(mDepthMapDSV[idx], D3D11_CLEAR_DEPTH, 1.0f, 0);
}

