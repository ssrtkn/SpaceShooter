#pragma once

#include "Entity.h"


namespace Render {


class SceneManager;
class EntityTerrain;

class EffectTerrain : public Effect {
	friend EntityTerrain;
public:
	RENDERDLL_API ~EffectTerrain(){};

protected:
	EffectTerrain(std:: string name) : Effect(name){ buildFX(); };
	void buildFX();

	ID3D11InputLayoutPtr mInputLayout;			//vertex layout for light pass

			//FX Stuff
	ID3DX11EffectTechniquePtr mTechNoFog;		//effect technique
	ID3DX11EffectTechniquePtr mTechFog;			//effect technique
	ID3DX11EffectTechniquePtr mTechShadow;		//effect technique

	ID3DX11EffectMatrixVariablePtr mfxWorldViewProj;		//world view proj matri
	ID3DX11EffectMatrixVariablePtr mfxWorld;				//world matrix
	ID3DX11EffectMatrixVariablePtr mfxWorldInvTranspose;	//inv transpose for normals
	ID3DX11EffectMatrixVariablePtr mfxShadowTransform;		//world view proj matri
	ID3DX11EffectVariablePtr mfxEyePosW;			//camera eye position for light

	ID3DX11EffectVariablePtr mfxMaterial;					//material struct

	ID3DX11EffectVariablePtr mfxDirLight;					//directional light in shader
	ID3DX11EffectVariablePtr mfxPointLight;				//point light in shader
	ID3DX11EffectVariablePtr mfxSpotLight;				//spot light in shader

	ID3DX11EffectVariablePtr mfxFogColor;		//Fogging
	ID3DX11EffectVariablePtr mfxFogStart;		
	ID3DX11EffectVariablePtr mfxFogRange;	

	std::vector<ID3DX11EffectShaderResourceVariablePtr> mfxLayerMapArray;		//texture stuff
	ID3DX11EffectShaderResourceVariablePtr mfxBlendMap;	//for blendng in textures
	ID3DX11EffectShaderResourceVariablePtr mfxShadowMap;	//texture stuff
	ID3DX11EffectVariablePtr mfxTexFactor;				//spot light in shader
};


class EntityTerrain : public Entity
{
	friend SceneManager;

public:
	~EntityTerrain(void);

	XMVECTOR projectOnTerrain( CXMVECTOR pos );
	bool isAboveGround( CXMVECTOR pos );

protected:
	EntityTerrain( std::string name );
	void buildFXTerrain();								//load and compile the shaders
	virtual void Draw4Light( CXMMATRIX transform );	//draw using lights

protected:
	shared_ptr<EffectTerrain> mEffectTerrain;
};

}

