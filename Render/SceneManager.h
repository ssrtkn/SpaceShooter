#pragma once

#include <map>
#include "D3DUtility.h"
#include "SceneNode.h"
#include "Entity.h"
#include "CameraProjective.h"
#include "CameraOrtho.h"
#include "Light.h"
#include "Shadow.h"


namespace Render {


class Root;
class Viewport;

//
//The scene manager holds all important objects of a scene
//it also initiates drawing the scene
//
class SceneManager
{
	friend Root;
	friend Entity;

public:
	RENDERDLL_API ~SceneManager(void);
	static RENDERDLL_API SceneManager *getInstance();	//singleton!

	RENDERDLL_API shared_ptr<SceneNode> getRootSceneNode();	//exactly one default root scene node in the scene
	
	//manage scene nodes
	RENDERDLL_API shared_ptr<SceneNode> createSceneNode( std::string name );
	RENDERDLL_API void destroySceneNode( std::string name );
	RENDERDLL_API shared_ptr<SceneNode> getSceneNode( std::string name );

	//manage entities
	RENDERDLL_API shared_ptr<Entity> createEntity(std::string name, Entity::EntityType type = Entity::ET_LIGHT );
	RENDERDLL_API void destroyEntity(std::string name);
	RENDERDLL_API shared_ptr<Entity> getEntity( std::string name );

	//manage cameras
	RENDERDLL_API shared_ptr<Camera> createCamera( std::string name );					
	RENDERDLL_API shared_ptr<Camera> createCamera( std::string name, shared_ptr<Viewport> );	
	RENDERDLL_API shared_ptr<Camera> createCamera( std::string name, float near, float far, float fov );
	RENDERDLL_API shared_ptr<Camera> createCamera( std::string name, float near, float far, float fov, shared_ptr<Viewport> vp );
	RENDERDLL_API void destroyCamera( std::string name  );
	RENDERDLL_API shared_ptr<Camera> getCamera( std::string name );
	RENDERDLL_API shared_ptr<Camera> getCurrentCamera(){ return mCamera; };		//get current camera

	//manage light
	RENDERDLL_API shared_ptr<Light> createLight( std::string name  );		
	RENDERDLL_API void destroyLight( std::string name );
	RENDERDLL_API std::vector<shared_ptr<Light>> getLights(){ return mLights; };

	//manage fog
	RENDERDLL_API void setFog( XMFLOAT4 fogColor, float fogStart, float fogRange ){ mFogColor=fogColor;mFogStart=fogStart;mFogRange=fogRange;};
	RENDERDLL_API void enableFog(){ mFogEnabled  = true; };
	RENDERDLL_API void disableFog(){ mFogEnabled = false; };
	RENDERDLL_API bool isFogEnabled(){ return mFogEnabled; };

	//manage shadow
	RENDERDLL_API shared_ptr<Shadow> getShadow(){ return mShadow; };
	RENDERDLL_API XMMATRIX getShadowTransform( UINT idx ){ return XMLoadFloat4x4( &mShadowTransforms[idx] ); };
	RENDERDLL_API shared_ptr<Camera> getShadowCamera(){ return mShadowCamera; };

protected:
	SceneManager(void);
	virtual void drawScene();								//draw the complete scene using the current camera
	static SceneManager *theInstance;						//singleton
	void addMovableObject(shared_ptr<MovableObject> mob);	//add a movable object to the collection
	void removeMovableObject( std::string name );			//remove
	void setCurrentCamera( shared_ptr<Camera> cam );		//set the current camera that captures the scene
	XMMATRIX getViewProjectionMatrix();						//return the current ViewProjection Matrix
	void setFogConstants( ID3DX11EffectVariable* mfxFogColor, ID3DX11EffectVariable* mfxFogStart, ID3DX11EffectVariable* mfxFogRange );
	void drawShadowMap();

protected:
	shared_ptr<SceneNode> mRootSceneNode;					//root of the scene node tree
	shared_ptr<Camera>	  mCamera;							//active camera

	XMFLOAT4 mFogColor;										//for the fog
	float mFogStart;
	float mFogRange;
	bool mFogEnabled;

	shared_ptr<Light>  mLight;							//the light for the shadow pass
	shared_ptr<Shadow> mShadow;							//the Shadow
	shared_ptr<Camera> mShadowCamera;					//active camera
	std::vector<XMFLOAT4X4> mShadowTransforms;			//WVP transform for shadow camera

	std::vector<shared_ptr<Light>> mLights;				//list of all light sources
	std::map<std::string,shared_ptr<MovableObject>> mMovableObjectCollection;	//all movable objects
	std::map<std::string,shared_ptr<SceneNode>>		mSceneNodeCollection;			//all scene nodes
};

}

