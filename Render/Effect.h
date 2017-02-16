#pragma once

#include "D3DUtility.h"
#include "GameObject.h"


namespace Render {

class AssetManager;
class Entity;


struct EffectVariables {
	//
};


//
//Holds a smart pointer to a D3D effect
//
class Effect : public GameObject
{
	friend AssetManager;
	friend Entity;

public:
	~Effect(void);

protected:
	RENDERDLL_API Effect( std:: string name );
	virtual RENDERDLL_API void			buildFX();							//create the effect from FX file, virtual function!
	RENDERDLL_API ID3DX11EffectPtr		getEffect(){ return mEffect; };		//return the effect
	RENDERDLL_API ID3D11InputLayoutPtr	getInputLayout( D3DX11_PASS_DESC *passDesc, UINT vertexFlags ); //return vertex layout for D3D

protected:
	ID3DX11EffectPtr		mEffect;				//the D3D effect
};

}

