#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// COM interface
#include <comip.h>
#include <comdef.h>

//DirectX
#include <d3d11.h>
#include <d3dx11.h>
#include "d3dx11Effect.h"
#include <xnamath.h>

//std stuff
#include "dxerr.h"
#include <cassert>
#include <ctime>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <random>
#include <memory>



#ifdef RENDER_EXPORTS
#define RENDERDLL_API __declspec(dllexport) 
#else
#define RENDERDLL_API __declspec(dllimport) 
#endif


//define COM smart pointers
// Device
_COM_SMARTPTR_TYPEDEF(ID3D11Device, __uuidof(ID3D11Device));
_COM_SMARTPTR_TYPEDEF(ID3D11DeviceContext, __uuidof(ID3D11DeviceContext));
_COM_SMARTPTR_TYPEDEF(ID3D11InputLayout,  __uuidof(ID3D11InputLayout));
_COM_SMARTPTR_TYPEDEF(ID3D11Debug,   __uuidof(ID3D11Debug));

// DXGI
_COM_SMARTPTR_TYPEDEF(IDXGISwapChain,  __uuidof(IDXGISwapChain));

// States
_COM_SMARTPTR_TYPEDEF(ID3D11BlendState,   __uuidof(ID3D11BlendState));
_COM_SMARTPTR_TYPEDEF(ID3D11DepthStencilState,   __uuidof(ID3D11DepthStencilState));
_COM_SMARTPTR_TYPEDEF(ID3D11RasterizerState,  __uuidof(ID3D11RasterizerState));
_COM_SMARTPTR_TYPEDEF(ID3D11SamplerState,   __uuidof(ID3D11SamplerState));

// Resources
_COM_SMARTPTR_TYPEDEF(ID3D11Resource,     __uuidof(ID3D11Resource));
_COM_SMARTPTR_TYPEDEF(ID3D11Texture1D,   __uuidof(ID3D11Texture1D));
_COM_SMARTPTR_TYPEDEF(ID3D11Texture2D,  __uuidof(ID3D11Texture2D));
_COM_SMARTPTR_TYPEDEF(ID3D11Texture3D, __uuidof(ID3D11Texture3D));
_COM_SMARTPTR_TYPEDEF(ID3D11Buffer,   __uuidof(ID3D11Buffer));

// Views
_COM_SMARTPTR_TYPEDEF(ID3D11View,   __uuidof(ID3D11View));
_COM_SMARTPTR_TYPEDEF(ID3D11RenderTargetView,  __uuidof(ID3D11RenderTargetView));
_COM_SMARTPTR_TYPEDEF(ID3D11ShaderResourceView, __uuidof(ID3D11ShaderResourceView));
_COM_SMARTPTR_TYPEDEF(ID3D11DepthStencilView,   __uuidof(ID3D11DepthStencilView));
_COM_SMARTPTR_TYPEDEF(ID3D11UnorderedAccessView,   __uuidof(ID3D11UnorderedAccessView));

// Shaders
_COM_SMARTPTR_TYPEDEF(ID3D11ComputeShader,  __uuidof(ID3D11ComputeShader));
_COM_SMARTPTR_TYPEDEF(ID3D11DomainShader,   __uuidof(ID3D11DomainShader));
_COM_SMARTPTR_TYPEDEF(ID3D11GeometryShader,   __uuidof(ID3D11GeometryShader));
_COM_SMARTPTR_TYPEDEF(ID3D11HullShader,  __uuidof(ID3D11HullShader));
_COM_SMARTPTR_TYPEDEF(ID3D11PixelShader,  __uuidof(ID3D11PixelShader));
_COM_SMARTPTR_TYPEDEF(ID3D11VertexShader,  __uuidof(ID3D11VertexShader));
_COM_SMARTPTR_TYPEDEF(ID3D11ClassInstance,   __uuidof(ID3D11ClassInstance));
_COM_SMARTPTR_TYPEDEF(ID3D11ClassLinkage,  __uuidof(ID3D11ClassLinkage));
_COM_SMARTPTR_TYPEDEF(ID3D11ShaderReflection,  IID_ID3D11ShaderReflection);
_COM_SMARTPTR_TYPEDEF(ID3D11ShaderReflectionType,  IID_ID3D11ShaderReflectionType);
_COM_SMARTPTR_TYPEDEF(ID3D11ShaderReflectionVariable,  IID_ID3D11ShaderReflectionVariable);
_COM_SMARTPTR_TYPEDEF(ID3D11ShaderReflectionConstantBuffer, IID_ID3D11ShaderReflectionConstantBuffer);

// D3D10
_COM_SMARTPTR_TYPEDEF(ID3D10Blob, IID_ID3D10Blob);

// Effect framework
//typedef _com_ptr_t <_com_IIID< ID3DX11EffectTechnique, &IID_ID3DX11EffectTechnique >> ID3DX11EffectTechniquePtr;
//typedef _com_ptr_t <_com_IIID< ID3DX11EffectVariable,  &IID_ID3DX11EffectVariable >>  ID3DX11EffectVariablePtr;
//typedef _com_ptr_t <_com_IIID< ID3DX11EffectMatrixVariable,  &IID_ID3DX11EffectMatrixVariable >>  ID3DX11EffectMatrixVariablePtr;
//typedef _com_ptr_t <_com_IIID< ID3DX11EffectShaderResourceVariable,  &IID_ID3DX11EffectShaderResourceVariable >>  ID3DX11EffectShaderResourceVariablePtr;

//_COM_SMARTPTR_TYPEDEF( ID3DX11Effect, IID_ID3DX11Effect );
//_COM_SMARTPTR_TYPEDEF( ID3DX11EffectTechnique, IID_ID3DX11EffectTechnique );
//_COM_SMARTPTR_TYPEDEF( ID3DX11EffectVariable,  IID_ID3DX11EffectVariable );
//_COM_SMARTPTR_TYPEDEF( ID3DX11EffectScalarVariable, IID_ID3DX11EffectScalarVariable );
//_COM_SMARTPTR_TYPEDEF( ID3DX11EffectVectorVariable, IID_ID3DX11EffectVectorVariable );
//_COM_SMARTPTR_TYPEDEF( ID3DX11EffectMatrixVariable, IID_ID3DX11EffectMatrixVariable );
//_COM_SMARTPTR_TYPEDEF( ID3DX11EffectShaderResourceVariable, IID_ID3DX11EffectShaderResourceVariable);

typedef ID3DX11Effect* ID3DX11EffectPtr;
typedef ID3DX11EffectTechnique* ID3DX11EffectTechniquePtr;
typedef ID3DX11EffectVariable* ID3DX11EffectVariablePtr;
typedef ID3DX11EffectScalarVariable* ID3DX11EffectScalarVariablePtr;
typedef ID3DX11EffectVectorVariable* ID3DX11EffectVectorVariablePtr;
typedef ID3DX11EffectMatrixVariable* ID3DX11EffectMatrixVariablePtr;
typedef ID3DX11EffectShaderResourceVariable* ID3DX11EffectShaderResourceVariablePtr;


using namespace std::tr1;	//use smart pointers from TR1/boost

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)

//---------------------------------------------------------------------------------------
// Simple d3d error checker.
//---------------------------------------------------------------------------------------
#if defined(DEBUG) | defined(_DEBUG)
    #ifndef HR
    #define HR(x)                                              \
    {                                                          \
        HRESULT hr = (x);                                      \
        if(FAILED(hr))                                         \
        {                                                      \
            DXTrace(__WFILE__, (DWORD)__LINE__, hr, L#x, true); \
        }                                                      \
    }
    #endif

#else
    #ifndef HR
    #define HR(x) (x)
    #endif
#endif 


//---------------------------------------------------------------------------------------
// Convenience macro for releasing COM objects.
//---------------------------------------------------------------------------------------
#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }

//---------------------------------------------------------------------------------------
// Convenience macro for deleting objects.
//---------------------------------------------------------------------------------------
#define SafeDelete(x) { delete x; x = 0; }

class D3DUtility
{
public:
    D3DUtility(void);
    ~D3DUtility(void);

	static UINT loadRawFile( std::wstring name, char** buffer );
	static ID3D11ShaderResourceView* CreateTexture2DArraySRV(
		ID3D11Device* device, ID3D11DeviceContext* context,
		std::vector<std::wstring>& filenames,
		DXGI_FORMAT format = DXGI_FORMAT_FROM_FILE,
		UINT filter = D3DX11_FILTER_NONE, 
		UINT mipFilter = D3DX11_FILTER_LINEAR);

	static ID3D11ShaderResourceView* CreateRandomTexture1DSRV(ID3D11Device* device);

	static float lerp( float x, float y, float s ){ return x + s*(y-x); };
	static float lerp( float x0, float y0, float x1, float y1, float x );

};

// Some colors.
namespace Colors
{
    XMGLOBALCONST XMVECTORF32 White     = {1.0f, 1.0f, 1.0f, 1.0f};
    XMGLOBALCONST XMVECTORF32 Black     = {0.0f, 0.0f, 0.0f, 1.0f};
    XMGLOBALCONST XMVECTORF32 Red       = {1.0f, 0.0f, 0.0f, 1.0f};
    XMGLOBALCONST XMVECTORF32 Green     = {0.0f, 1.0f, 0.0f, 1.0f};
    XMGLOBALCONST XMVECTORF32 Blue      = {0.0f, 0.0f, 1.0f, 1.0f};
    XMGLOBALCONST XMVECTORF32 Yellow    = {1.0f, 1.0f, 0.0f, 1.0f};
    XMGLOBALCONST XMVECTORF32 Cyan      = {0.0f, 1.0f, 1.0f, 1.0f};
    XMGLOBALCONST XMVECTORF32 Magenta   = {1.0f, 0.0f, 1.0f, 1.0f};

    XMGLOBALCONST XMVECTORF32 Silver    = {0.75f, 0.75f, 0.75f, 1.0f};
    XMGLOBALCONST XMVECTORF32 LightSteelBlue = {0.69f, 0.77f, 0.87f, 1.0f};
}