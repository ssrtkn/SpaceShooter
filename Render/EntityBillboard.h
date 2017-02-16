#pragma once

#include "D3DUtility.h"
#include "Entity.h"
#include "Effect.h"


namespace Render {


class SceneManager;
class EntityBillboard;

class EffectBillboard : public Effect
{
	friend EntityBillboard;
public:
	RENDERDLL_API ~EffectBillboard(){};

protected:
	EffectBillboard(std:: string name) : Effect(name){ buildFX(); };
	void buildFX();

		//layouts
	ID3D11InputLayoutPtr mInputLayout;			//vertex layout for light pass

	ID3DX11EffectTechniquePtr mTechNoFog;					//effect technique
	ID3DX11EffectTechniquePtr mTechFog;						//effect technique
	ID3DX11EffectTechniquePtr mTechShadow;					//effect technique

	ID3DX11EffectMatrixVariablePtr	mfxWorld;				//world view proj matri
	ID3DX11EffectMatrixVariablePtr	mfxViewProj;				//world view proj matri
	ID3DX11EffectVariablePtr		mfxEyePosW;					//camera eye position for light

	ID3DX11EffectVariablePtr mfxMaterial;					//texture to set

	ID3DX11EffectVariablePtr mfxDirLight;					//directional light in shader
	ID3DX11EffectVariablePtr mfxPointLight;					//point light in shader
	ID3DX11EffectVariablePtr mfxSpotLight;					//spot light in shader

	ID3DX11EffectVariablePtr mfxFogColor;					//Fogging
	ID3DX11EffectVariablePtr mfxFogStart;		
	ID3DX11EffectVariablePtr mfxFogRange;	

	ID3DX11EffectShaderResourceVariablePtr mfxDiffuseMap;	//texture stuff
};


class EntityBillboard : public Entity 
{
	friend SceneManager;

public:
	~EntityBillboard( void);

protected:
	EntityBillboard( std::string name  );
	void buildFXBB();								//load and compile the shaders

	virtual void Draw4Light( CXMMATRIX transform );	//draw using lights 
	virtual void Draw4Light( CXMMATRIX transform, SceneNode::DrawType dt  );	//draw using both  

	virtual void Draw4Shadow( CXMMATRIX transform );

protected:
	shared_ptr<EffectBillboard>	mEffectBB;
};


}
