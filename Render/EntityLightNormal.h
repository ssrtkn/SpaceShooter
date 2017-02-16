#pragma once

#include "D3DUtility.h"
#include "Entity.h"
#include "Root.h"
#include "Material.h"


namespace Render {

class SceneManager;
class EntityLightNormal;


//
//This is the effect for this entity type
//It uses lights, fog, normal maps
//
class EffectLightNormal : public Effect {

	friend EntityLightNormal;
public:
	RENDERDLL_API ~EffectLightNormal(){};

protected:
	EffectLightNormal(std:: string name) : Effect(name){ buildFX(); };	//call buildFX for connecting the variables
	void buildFX();

	//layouts
	ID3D11InputLayoutPtr mInputLayout;			//vertex layout for light pass

	//FX Stuff
	ID3DX11EffectTechniquePtr mTechNoFog;			//effect technique
	ID3DX11EffectTechniquePtr mTechFog;				//effect technique

	ID3DX11EffectMatrixVariablePtr mfxWorldViewProj;		//world view proj matrix
	ID3DX11EffectMatrixVariablePtr mfxWorld;				//world matrix
	ID3DX11EffectMatrixVariablePtr mfxWorldInvTranspose;	//inv transpose for normals
	ID3DX11EffectVariablePtr mfxEyePosW;					//camera eye position for light

	ID3DX11EffectVariablePtr mfxDirLight;					//directional light in shader
	ID3DX11EffectVariablePtr mfxPointLight;					//point light in shader
	ID3DX11EffectVariablePtr mfxSpotLight;					//spot light in shader

	ID3DX11EffectVariablePtr mfxMaterial;					//material struct

	ID3DX11EffectVariablePtr mfxFogColor;					//Fogging
	ID3DX11EffectVariablePtr mfxFogStart;		
	ID3DX11EffectVariablePtr mfxFogRange;	

	ID3DX11EffectShaderResourceVariablePtr mfxDiffuseMap;		//texture stuff
	ID3DX11EffectShaderResourceVariablePtr mfxNormalMap;		//texture stuff
};


//------------------------------------------------------


//
//Main object class that uses lighting, fogs and normal maps
//
class EntityLightNormal : public Entity
{
	friend SceneManager;

public:
    virtual ~EntityLightNormal(void);

protected:
    EntityLightNormal(std::string name);				//constructor only called by SceneManager
	void buildFXLN();									//load and compile the shaders
	virtual void Draw4Light( CXMMATRIX transform );		//draw using lights

protected:
	shared_ptr<EffectLightNormal>	mEffectLN;
};

}

