#pragma once

#include "D3DUtility.h"
#include "Entity.h"
#include "Root.h"
#include "Material.h"
#include "Effect.h"

namespace Render {


class SceneManager;
class EntityLight;


//
//This is the effect for this entity type
//It uses lights, fog
//
class EffectLight : public Effect {
	friend EntityLight;
public:
	RENDERDLL_API ~EffectLight(){};

protected:
	EffectLight( std::string name ) : Effect(name){ buildFX(); };	//call your own version of buildFX()!
	void buildFX();								//virtual version of the buildFX function, called from base class!

	//layouts
	ID3D11InputLayoutPtr mInputLayout;			//vertex layout for light pass

	//Techniques
	ID3DX11EffectTechniquePtr mTechNoFog;		//effect technique no fog
	ID3DX11EffectTechniquePtr mTechFog;			//effect technique with fog
	ID3DX11EffectTechniquePtr mTechNoLighting;	//effect technique no lighting

	//variables for light pass
	ID3DX11EffectMatrixVariablePtr mfxWorldViewProj;		//world view proj matrix
	ID3DX11EffectMatrixVariablePtr mfxWorld;				//world matrix
	ID3DX11EffectMatrixVariablePtr mfxWorldInvTranspose;	//inv transpose for normals
	ID3DX11EffectMatrixVariablePtr mfxShadowTransform;		//world view proj matrix
	ID3DX11EffectVariablePtr mfxEyePosW;					//camera eye world position for light

	ID3DX11EffectVariablePtr mfxDirLight;					//directional light in shader
	ID3DX11EffectVariablePtr mfxPointLight;					//point light in shader
	ID3DX11EffectVariablePtr mfxSpotLight;					//spot light in shader

	ID3DX11EffectVariablePtr mfxMaterial;					//material struct

	ID3DX11EffectVariablePtr mfxFogColor;					//Fogging
	ID3DX11EffectVariablePtr mfxFogStart;		
	ID3DX11EffectVariablePtr mfxFogRange;	

	ID3DX11EffectShaderResourceVariablePtr mfxDiffuseMap;	//texture stuff
	ID3DX11EffectShaderResourceVariablePtr mfxShadowMap;	//texture stuff
};


//-------------------------------------------------------

//
//Main object class that uses lighting and fogs
//
class EntityLight : public Entity
{
	friend SceneManager;

public:
    virtual ~EntityLight(void);
	void enableLighting(){ mLightingEnabled=true; };	//switch on lighting
	void disableLighting(){ mLightingEnabled=false; };	//disable lighting

protected:
    EntityLight(std::string name);					//constructor only called by SceneManager
	void buildFXLight();							//load and compile the shaders
	virtual void Draw4Light( CXMMATRIX transform );	//draw using lights

protected:
	bool mLightingEnabled;							//is lighting enabled?
	shared_ptr<EffectLight>	mEffectLight;			//the light effect for this object
};



}

