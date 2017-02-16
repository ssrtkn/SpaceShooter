#pragma once

#include "Entity.h"

namespace Render {

class SceneManager;
class EntitySky;

class EffectSky : public Effect
{
	friend EntitySky;
public:
	RENDERDLL_API ~EffectSky(){};

protected:
	EffectSky(std:: string name) : Effect(name){ buildFX(); };	//connect variables
	void buildFX();

	ID3D11InputLayoutPtr mInputLayout;			//vertex layout for light pass

	ID3DX11EffectTechniquePtr mTechNoFog;			//effect technique
	ID3DX11EffectTechniquePtr mTechFog;			//effect technique
	ID3DX11EffectTechniquePtr mTechShadow;		//effect technique

	ID3DX11EffectVariablePtr mfxMaterial;					//material struct

	ID3DX11EffectMatrixVariablePtr mfxWorldViewProj;			//world view projction matrix
	ID3DX11EffectShaderResourceVariablePtr mfxCubeMap;		//texture stuff

	ID3DX11EffectVariablePtr mfxFogColor;		//Fogging
	ID3DX11EffectVariablePtr mfxFogStart;		
	ID3DX11EffectVariablePtr mfxFogRange;	
};


class EntitySky : public Entity
{
	friend SceneManager;

public:
	~EntitySky(void);

protected:
	EntitySky( std::string name );				//called only by SceneManager
	void buildFXSky();								//load and compile the shaders
	virtual void Draw4Light( CXMMATRIX transform );	//draw the sky 

protected:
	shared_ptr<EffectSky>	mEffectSky;
};

}

