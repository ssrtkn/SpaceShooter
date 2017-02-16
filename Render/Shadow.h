#pragma once


#include "D3DUtility.h"

namespace Render {

class SceneManager;

class Shadow
{
	friend SceneManager;
	static const UINT SMapSize = 2048;
	static const UINT numShadowmaps = 1;

public:
	~Shadow(void);
	RENDERDLL_API ID3D11ShaderResourceViewPtr getDepthMapSRV();
	RENDERDLL_API UINT getNumShadowMaps(){ return numShadowmaps; };

protected:
	void bindDsvAndSetNullRenderTarget( UINT idx );
	Shadow(void);

protected:
	ID3D11ShaderResourceViewPtr				mDepthMapSRV;
	std::vector<ID3D11DepthStencilViewPtr>  mDepthMapDSV;
	D3D11_VIEWPORT mViewport;
};

}

