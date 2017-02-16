

#include "Entity.h"
#include "Root.h"
#include "RenderSystem.h"
#include "SceneManager.h"
#include "AssetManager.h"
#include "Effect.h"

using namespace Render;


//
//Connect effect variables for the shadow effect
//
void EffectShadow::buildFX()
{
    // Will be implemented later.
}

//---------------------------------------------------------


//constructor
Entity::Entity( std::string name ) : 
	MovableObject(name), mVisible(true),mType(ET_NONE), mMesh(NULL), 
		mMaterial(NULL), mEffectShadow(),mCastShadow(true)
{
}

//destructor
Entity::~Entity(void)
{
}

//get an effect with a specific name from AssetManager
shared_ptr<Effect> Entity::getEffect(  std::string effectname )
{
	AssetManager * am = AssetManager::getInstance();
	return am->getEffect( effectname );
}

//store effect in asset manager
void Entity::storeEffect( shared_ptr<Effect> effect )
{
	AssetManager * am = AssetManager::getInstance();
	am->storeEffect( effect );
}

//set fog constants in the scene manager
void Entity::setFogShaderConstants(	ID3DX11EffectVariable* mfxFogColor, 
									ID3DX11EffectVariable* mfxFogStart, 
									ID3DX11EffectVariable* mfxFogRange )
{
	SceneManager *sm = SceneManager::getInstance();

	sm->setFogConstants( mfxFogColor, mfxFogStart, mfxFogRange );
}

//build the shadow effect and store it in the asset manager
void Entity::buildFXShadow()
{
    // Will be implemented later.
}

//
//Main draw call for the entity
//Called from the scene node where this entity is attached to
//Can be called when drawing the shadow map, or when drawing for light
//This call simply calls the respective virtual member function of the inherited class
//
void Entity::Draw( CXMMATRIX transform, SceneNode::DrawType dt )
{
	if( dt == SceneNode::DT_LIGHT && mVisible) {	//draw for light
		return Draw4Light( transform );				//virtual function
	}
	
	if( mCastShadow )								//draw for shadow map
		Draw4Shadow( transform );					//virtual function
}


//
//Draw the entity for the shadow map
//Implemented here since can use for all inherited classes
//
void Entity::Draw4Shadow( CXMMATRIX world )
{
	// Will be implemented later.
}



void Entity::setMaterialShaderConstants( ID3DX11EffectVariablePtr material,	//send to shaders
										ID3DX11EffectShaderResourceVariablePtr texture,
										ID3DX11EffectShaderResourceVariablePtr normal,
										ID3DX11EffectShaderResourceVariablePtr blend )
{
	if( mMaterial ) {
		mMaterial->setShaderConstants( material, texture, normal, blend );
	}
}


void Entity::setMaterialShaderConstants( ID3DX11EffectVariablePtr material,	//send to shaders
										std::vector<ID3DX11EffectShaderResourceVariablePtr> &textures,
										ID3DX11EffectShaderResourceVariablePtr normal,
										ID3DX11EffectShaderResourceVariablePtr blend )
{
	if( mMaterial ) {
		mMaterial->setShaderConstants( material, textures, normal, blend );
	}

}

void Entity::setLightShaderConstants(	ID3DX11EffectVariablePtr directional, 
										ID3DX11EffectVariablePtr point, 
										ID3DX11EffectVariablePtr spot )
{
	SceneManager *sm = SceneManager::getInstance();

	std::vector<shared_ptr<Light>> lights = sm->getLights();
	std::vector<shared_ptr<Light>>::iterator it = lights.begin();
	while( it != lights.end() ) {
		(*it)->setShaderConstants( directional, point, spot );
		++it;
	}
}



