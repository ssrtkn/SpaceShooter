#pragma once

#include "D3DUtility.h"
#include "GameObject.h"

namespace Render {

class Entity;
class AssetManager;

//Material manages all color structs, textures, normal maps etc.
class Material : public GameObject
{
	friend Entity;
	friend AssetManager;

public:
	struct Material_		//struct to be sent to the shader in one call
	{
		Material_() { ZeroMemory(this, sizeof(this)); }
		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT4 Specular;	// w = SpecPower
		XMFLOAT4 Reflect;
		UINT NumTextures;
	};

	RENDERDLL_API void setMaterial(	XMFLOAT4 Ambient, XMFLOAT4 Diffuse,									//set material parameters
						XMFLOAT4 Specular, XMFLOAT4 Reflect );
	RENDERDLL_API void setTexture( std::wstring filename );												//name of texture file
	RENDERDLL_API void setTexture( ID3D11ShaderResourceViewPtr texture );									//name of texture file
	RENDERDLL_API void setTextures( std::vector<std::wstring> filenames );								//name of texture file

	RENDERDLL_API void setNormalMap( std::wstring filename );												//name of normal map file
	RENDERDLL_API void setBlendMap( std::wstring filename );												//raw data, bytes!

	RENDERDLL_API void setFromFile( std::wstring filename, std::wstring texturepath, UINT subset = 0 );	//set material properties from file

	RENDERDLL_API ~Material(void);

protected:
	Material( std::string name );

	void setShaderConstants( ID3DX11EffectVariablePtr material,					//send to shaders
							ID3DX11EffectShaderResourceVariablePtr texture,		//only one texture
							ID3DX11EffectShaderResourceVariablePtr normal,
							ID3DX11EffectShaderResourceVariablePtr blend );

	void setShaderConstants( ID3DX11EffectVariablePtr material,					//send to shaders
							std::vector<ID3DX11EffectShaderResourceVariablePtr> &textures,	//use more textures
							ID3DX11EffectShaderResourceVariablePtr normal,
							ID3DX11EffectShaderResourceVariablePtr blend );

protected:
	Material_ mMaterial;											//struct holding all parameters
	std::vector<ID3D11ShaderResourceViewPtr> mTextureMapSRVArray;	//
	ID3D11ShaderResourceViewPtr mNormalMapSRV;						//resource view of the loaded normal map
	ID3D11ShaderResourceViewPtr mBlendMapSRV;						//resource view of the loaded displacement map

};

}

