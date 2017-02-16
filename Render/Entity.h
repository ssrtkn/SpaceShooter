#pragma once

#include "D3DUtility.h"
#include "MovableObject.h"
#include "Material.h"
#include "Mesh.h"
#include "Effect.h"

namespace Render {

class SceneManager;


//
//The shadow effect for all entities
//
class  EffectShadow : public Effect
{
	friend Entity;
public:
	RENDERDLL_API ~EffectShadow(){};

protected:
	EffectShadow(std:: string name) : Effect(name){
        // Will be implemented later.
    };
	void buildFX();

	ID3D11InputLayoutPtr					mInputLayout;		//vertex layout for shadow
	ID3DX11EffectTechniquePtr				mTech;				//effect technique
	ID3DX11EffectMatrixVariablePtr			mfxWorldViewProj;	//world view proj matrix
	ID3DX11EffectShaderResourceVariablePtr	mfxDiffuseMap;		//texture stuff
};



//
//Main 3D object class
//
class Entity : public MovableObject
{
	friend SceneManager;

public:
	enum  EntityType { ET_NONE=-1, ET_COLOR, ET_LIGHT, ET_LIGHTNORMAL, ET_SKY, ET_TERRAIN, ET_BILLBOARD };

	~Entity(void);

	void setVisible( bool visible ){ mVisible = visible; };
	bool getVisible(){ return mVisible; };
	bool getCastShadow(){ return mCastShadow; };
	void setCastShadow( bool cs ){ mCastShadow=cs; };

	virtual void setMaterial( shared_ptr<Material> mat ){ mMaterial = mat; };	//set the material
	virtual void setMesh( shared_ptr<Mesh> mesh ){ mMesh = mesh; };			//set the 3D mesh

protected:
	Entity( std::string name );												//constructor only called by Scenemanager
	shared_ptr<Effect>	getEffect(  std::string name );						//
	void				storeEffect( shared_ptr<Effect> effect );

	void buildFXShadow();								//load and compile the shaders

	void Draw( CXMMATRIX transform, SceneNode::DrawType dt );

	virtual void Draw4Light( CXMMATRIX transform ){};		//draw for light passes function
	virtual void Draw4Shadow( CXMMATRIX transform );		//draw shadow function

	void setMaterialShaderConstants(ID3DX11EffectVariablePtr material,			//send to shaders
									ID3DX11EffectShaderResourceVariablePtr texture,
									ID3DX11EffectShaderResourceVariablePtr normal,
									ID3DX11EffectShaderResourceVariablePtr blend );

	void setMaterialShaderConstants(ID3DX11EffectVariablePtr material,			//send to shaders
									std::vector<ID3DX11EffectShaderResourceVariablePtr> &textures,
									ID3DX11EffectShaderResourceVariablePtr normal,
									ID3DX11EffectShaderResourceVariablePtr blend );

	void setLightShaderConstants(	ID3DX11EffectVariablePtr directional,			//send to shaders
									ID3DX11EffectVariablePtr point, 
									ID3DX11EffectVariablePtr spot );

	void setFogShaderConstants(	ID3DX11EffectVariable* mfxFogColor, 
								ID3DX11EffectVariable* mfxFogStart, 
								ID3DX11EffectVariable* mfxFogRange );

protected:
	bool					mVisible;			//is the entity visible?
	bool					mCastShadow;		//does it cast a shadow?

	EntityType				mType;				//type of the entity

	shared_ptr<Material>	mMaterial;			//material stuff
	shared_ptr<Mesh>		mMesh;				//the mesh
	shared_ptr<Effect>		mEffectShadow;		//the shadow effect used to draw shadow map
};

}
