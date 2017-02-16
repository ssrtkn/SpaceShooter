

#include "Material.h"
#include "Root.h"
#include "RenderSystem.h"

using namespace Render;

//constructor
//set to white as default
Material::Material( std::string name ) : GameObject(name), mNormalMapSRV(NULL), mBlendMapSRV(NULL)
{
	mMaterial.Ambient = XMFLOAT4(1,1,1,1);
	mMaterial.Diffuse = XMFLOAT4(1,1,1,1);
	mMaterial.Specular = XMFLOAT4(1,1,1,1);
	mMaterial.Reflect = XMFLOAT4(1,1,1,1);
	mMaterial.NumTextures = 0;
}

//destructor
Material::~Material(void)
{
}

//set default color
void Material::setMaterial( XMFLOAT4 Ambient, XMFLOAT4 Diffuse, XMFLOAT4 Specular, XMFLOAT4 Reflect )
{
	mMaterial.Ambient = Ambient;
	mMaterial.Diffuse = Diffuse;
	mMaterial.Specular = Specular;
	mMaterial.Reflect = Reflect;
}

//load a texture from file
void Material::setTexture( std::wstring filename )
{
	RenderSystem *rs = RenderSystem::getInstance();
	ID3D11Device* d3dDevice = rs->getDevice();			//I need the D3D device
	
	ID3D11ShaderResourceView *texSRV;
	HR(D3DX11CreateShaderResourceViewFromFile(d3dDevice, filename.c_str(), 0, 0, &texSRV, 0 ));	//load from file
	mTextureMapSRVArray.push_back( texSRV );		//store in collection
	mMaterial.NumTextures = mTextureMapSRVArray.size();
}

//use an existing texture
void Material::setTexture( ID3D11ShaderResourceViewPtr texture )
{
	mTextureMapSRVArray.push_back( texture );
	mMaterial.NumTextures = mTextureMapSRVArray.size();
}

//create several textures
void Material::setTextures( std::vector<std::wstring> filenames ) 
{
	RenderSystem *rs = RenderSystem::getInstance();
	ID3D11Device* d3dDevice = rs->getDevice();

	UINT size = filenames.size();

	std::vector<ID3D11ShaderResourceView *> srcTex(size);
	for(UINT i = 0; i < size; ++i)
	{
		ID3D11ShaderResourceView *texSRV;

        HR(D3DX11CreateShaderResourceViewFromFile(d3dDevice, filenames[i].c_str(), 0, 0, &texSRV, 0));

		mTextureMapSRVArray.push_back( texSRV );
	}
	mMaterial.NumTextures = mTextureMapSRVArray.size();
	
	//mTextureMapSRV = D3DUtility::CreateTexture2DArraySRV( d3dDevice, d3dImmediateContext, filenames );
}

//create a normal map from a file
void Material::setNormalMap( std::wstring filename )
{
	RenderSystem *rs = RenderSystem::getInstance();
	ID3D11Device* d3dDevice = rs->getDevice();
	
	HR(D3DX11CreateShaderResourceViewFromFile(d3dDevice, filename.c_str(), 0, 0, &mNormalMapSRV, 0 ));
}

//load a blend map from file
void Material::setBlendMap( std::wstring filename )
{
	RenderSystem *rs = RenderSystem::getInstance();
	ID3D11Device* d3dDevice = rs->getDevice();

	HR(D3DX11CreateShaderResourceViewFromFile(d3dDevice, filename.c_str(), 0, 0, &mBlendMapSRV, 0 ));
}


//
//Set material values to shader variables
//In: shader variables that might be set
//
void Material::setShaderConstants(	ID3DX11EffectVariablePtr fxMaterial, 
									ID3DX11EffectShaderResourceVariablePtr fxTexture,
									ID3DX11EffectShaderResourceVariablePtr fxNormal,
									ID3DX11EffectShaderResourceVariablePtr fxBlend)
{
	if( fxMaterial ) {
		fxMaterial->SetRawValue(&mMaterial, 0, sizeof(mMaterial));	//set color values
	}

	if( fxTexture && mTextureMapSRVArray.size()>0 ) {
		fxTexture->SetResource(mTextureMapSRVArray[0]);		//set first texture
	}

	if( fxNormal && mNormalMapSRV ) {
		fxNormal->SetResource(mNormalMapSRV);	//set normal map
	}

	if( fxBlend && mBlendMapSRV ) {
		fxBlend->SetResource(mBlendMapSRV);		//set blend map
	}
}


void Material::setShaderConstants(	ID3DX11EffectVariablePtr fxMaterial,	//send to shaders
									std::vector<ID3DX11EffectShaderResourceVariablePtr> &fxTexture,
									ID3DX11EffectShaderResourceVariablePtr fxNormal,
									ID3DX11EffectShaderResourceVariablePtr fxBlend )
{
	if( fxMaterial ) {
		fxMaterial->SetRawValue(&mMaterial, 0, sizeof(mMaterial));
	}

	if( fxTexture.size()>0 && mTextureMapSRVArray.size()>0 ) {
		for( UINT i=0; i<fxTexture.size(); i++ ) {
			if( mTextureMapSRVArray.size()>=i ) {
				fxTexture[i]->SetResource(mTextureMapSRVArray[i]);
			}
		}
	}

	if( fxNormal && mNormalMapSRV ) {
		fxNormal->SetResource(mNormalMapSRV);
	}

	if( fxBlend && mBlendMapSRV ) {
		fxBlend->SetResource(mBlendMapSRV);
	}
}



