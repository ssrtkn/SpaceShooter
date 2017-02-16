#pragma once

#include "D3DUtility.h"
#include "Entity.h"
#include "Root.h"

namespace Render {

class SceneManager;
class EntityColor;

class EffectColor : public Effect
{
	friend EntityColor;
public:
	RENDERDLL_API ~EffectColor(){};

protected:
	EffectColor( std::string name ) : Effect(name){ buildFX(); };
	void buildFX();

	ID3DX11EffectTechniquePtr mTech;			//effect technique

	ID3DX11EffectMatrixVariablePtr mfxWorldViewProj;
	ID3D11InputLayoutPtr mInputLayout;

	ID3DX11EffectVariablePtr mfxMaterial;					//color material

	ID3DX11EffectVariablePtr mfxFogColor;		//Fogging
	ID3DX11EffectVariablePtr mfxFogStart;		
	ID3DX11EffectVariablePtr mfxFogRange;	
};


class EntityColor : public Entity
{
	friend SceneManager;

public:
    EntityColor(std::string name);
    virtual ~EntityColor(void);

protected:
	void buildFXColor();						//load and compile the shaders
	virtual void Draw4Light( CXMMATRIX transform );	//draw using lights

protected:
	shared_ptr<EffectColor>	mEffectColor;
};

}

