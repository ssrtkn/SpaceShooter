
#include <cstdio>
#include "D3DUtility.h"
#include "SceneManager.h"
#include "RenderSystem.h"
#include "EntityLight.h"
#include "EntityColor.h"
#include "EntityLightNormal.h"
#include "Window.h"
#include "Viewport.h"
#include "EntitySky.h"


using namespace Render;

SceneManager* SceneManager::theInstance = NULL;	//the singleton instance

//
//constructor
//Create the only instance of the root scene node
//
SceneManager::SceneManager(void) : 
	mRootSceneNode(new SceneNode("RootSceneNode")), 
	mCamera(),
	mFogColor(XMFLOAT4(0.5f, 0.7f, 0.5f, 0.0f)),
	mFogStart(500.0f),
	mFogRange(500.0f),
	mFogEnabled(false),
	mShadow(new Shadow()),
	mShadowCamera()
{
	for( UINT i=0; i<4; i++ ) {
		XMFLOAT4X4 trans;
		XMStoreFloat4x4( &trans, XMMatrixIdentity() );
		mShadowTransforms.push_back( trans );
	}
}

//destructor
SceneManager::~SceneManager(void)
{
}

//
//get singleton class instance
//
SceneManager *SceneManager::getInstance()
{
    if ( SceneManager::theInstance==NULL ) {		//if there is no singleton instance, create one
        SceneManager::theInstance = new SceneManager();
	}
    return SceneManager::theInstance;
}

//
//Draw the shadow map
//
//
void SceneManager::drawShadowMap()
{
	RenderSystem *rs = RenderSystem::getInstance();
	Window *win = Window::getInstance();

	for( UINT i=0; i<mLights.size(); i++ ) {
		if( mLights[i]->getType()== Light::LT_DIRECTIONAL ) {
			shared_ptr<SceneNode> sn = mLights[i]->getSceneNode();
			if( sn ) {
				if( !mShadowCamera ) {	//if no shadow camera yet then create one
					mShadowCamera = shared_ptr<CameraOrtho>( new CameraOrtho("ShadowCam", 1, 1000, 100) );
					mShadowCamera->setAspectRatio( 1.0 );
					addMovableObject( mShadowCamera );
				}
				shared_ptr<SceneNode> ssn = getSceneNode( "ShadowSceneNode");	//use a special scene node to hang camera onto
				if( !ssn ) {
					ssn = createSceneNode("ShadowSceneNode");	//create it if it does not exist
					getRootSceneNode()->addChild( ssn );		//make child of root scene node
					ssn->attachObject( mShadowCamera );			//attach shadow camera to it
				}
				for( UINT i=0; i<mShadow->getNumShadowMaps(); i++ ) {	//go through all shadow maps
					mShadow->bindDsvAndSetNullRenderTarget(i);			//bind shadow depth/stencil buffer and void render target
			
					ssn->setOrientation( sn->getOrientation() );		//let shadow camera look at shadow area
					ssn->setPosition( sn->getPosition() - 1.0f*sn->getDirectionZ() );

					mShadowCamera->setFrustumToPoints( getCurrentCamera()->getFrustum() );	//set camera frustum
	
					XMMATRIX strans = mShadowCamera->getViewMatrix() * mShadowCamera->getProjectionMatrix();	//
					XMStoreFloat4x4( &mShadowTransforms[i], strans );
					mRootSceneNode->Draw( SceneNode::DT_SHADOW );		//draw the scene
					rs->resetRenderTarget();							//set back render target to back buffer
					break;
				}
			}
		}
	}
}


//
//Main drawing scene
//Every viewport with an attached camera receives a camera snapshot
//
void SceneManager::drawScene()
{
	RenderSystem *rs = RenderSystem::getInstance();
	Window *win = Window::getInstance();

	std::vector<shared_ptr<Viewport>>::iterator it = win->mViewports.begin();	//iterate through all viewports
	while( it != win->mViewports.end() ) {
		shared_ptr<Camera> cam = (*it)->getCamera();
		if( cam ) {	
			setCurrentCamera( cam );	//if viewport has a camera, make this the current camera
			drawShadowMap();			//draw the shadow map

			rs->clearDepthBuffer();		//clear depthbuffer
			rs->setViewport( *it );		//use the current view port
			mRootSceneNode->Draw();		//draw the whole scene by calling draw on the root scene node
		}
		++it;
	}
};

//
//get the product of the view and the projection matrix
//of the current camera
//
XMMATRIX SceneManager::getViewProjectionMatrix()
{
	if( mCamera == NULL ) {
		return XMMatrixIdentity();
	}

	return mCamera->getViewMatrix() * mCamera->getProjectionMatrix();
}

//
//return the root scene node
//
shared_ptr<SceneNode> SceneManager::getRootSceneNode()
{ 
	return mRootSceneNode; 
}

//
//create a new scene node
//and insert into the scene node map
//
shared_ptr<SceneNode> SceneManager::createSceneNode( std::string name )
{ 
	std::map<std::string,shared_ptr<SceneNode>>::iterator it = mSceneNodeCollection.find( name );
	if( it != mSceneNodeCollection.end() ) return it->second;

	shared_ptr<SceneNode> sn( new SceneNode(name) ); 
	mSceneNodeCollection.insert( std::pair<std::string,shared_ptr<SceneNode>>(sn->getName(),sn) );
	return sn;
}

//
//destroy a scene node with a given name
//
void SceneManager::destroySceneNode( std::string name )
{
	mSceneNodeCollection.erase( name );
}

//
//find a scene node with a given name and return it
//return NULL if not found
//
shared_ptr<SceneNode> SceneManager::getSceneNode( std::string name )
{
	std::map<std::string,shared_ptr<SceneNode>>::iterator it = mSceneNodeCollection.find( name );
	if( it != mSceneNodeCollection.end() ) return it->second;

	return shared_ptr<SceneNode>();
}

//
//create an entity with a certain name
//if the name already exsists, try to cast the element to Entity
//
shared_ptr<Entity> SceneManager::createEntity( std::string name, Entity::EntityType type )
{
	std::map<std::string,shared_ptr<MovableObject>>::iterator it = mMovableObjectCollection.find( name );
	if( it != mMovableObjectCollection.end() ) {
		return dynamic_pointer_cast<Entity>( it->second );	//if name exists return the existing entity
	}

	shared_ptr<Entity> ent;
	switch( type ) {

	case Entity::ET_COLOR:
		ent = shared_ptr<EntityColor>(new EntityColor(name));	//create entity with mono color
		break;
	case Entity::ET_SKY:
		ent = shared_ptr<EntitySky>(new EntitySky(name));		//create sky
		break;
	case Entity::ET_LIGHTNORMAL:								//create entity with lights, normalmap, shadow
		ent = shared_ptr<EntityLightNormal>(new EntityLightNormal(name));
		break;

	default:
		ent = shared_ptr<EntityLight>(new EntityLight(name));	//create entity with light and shadow
	};

	addMovableObject( ent );
	return ent;
}

//
//remove a movable object from the map
//
void SceneManager::destroyEntity(std::string name)
{
	removeMovableObject( name );
}

//
//find an entity with a given name
//if found return it
//if not found, return NULL
//
shared_ptr<Entity> SceneManager::getEntity( std::string name )
{
	std::map<std::string,shared_ptr<MovableObject>>::iterator it = mMovableObjectCollection.find( name );
	
	if( it != mMovableObjectCollection.end() )
		return dynamic_pointer_cast<Entity>(it->second);

	return shared_ptr<Entity>();
}

//
//create a camera with a given name
//use camera default values
//use the aspect ratio of the current window
//
shared_ptr<Camera> SceneManager::createCamera( std::string name )
{
	Window *win = Window::getInstance();
	return createCamera( name, 1.0f, 1000.0f, 0.25f*XM_PI );
}

//
//create a camera with a given name
//use camera default values, and a given view port
//use the aspect ratio of the current window
//
shared_ptr<Camera> SceneManager::createCamera( std::string name, shared_ptr<Viewport> vp )
{
	Window *win = Window::getInstance();
	return createCamera( name, 1.0f, 1000.0f, 0.25f*XM_PI, vp );
}

//
//create a camera with parameters
//
shared_ptr<Camera> SceneManager::createCamera( std::string name, float nearplane, float farplane, float fov )
{
	Window *win = Window::getInstance();

	shared_ptr<Viewport> vp = win->createViewPort( "",0,0,1,1 );

	return createCamera( name, nearplane, farplane, fov, vp );
}

//
//create a camera with parameters and a given viewport
//
shared_ptr<Camera> SceneManager::createCamera( std::string name, float nearplane, float farplane, float fov, shared_ptr<Viewport> vp )
{	
	shared_ptr<Camera> cam( new CameraProjective( name, nearplane, farplane, fov  ) );
	addMovableObject( cam );
	vp->setCamera( cam );

	return cam;
}

//destroy a specified camera
void SceneManager::destroyCamera( std::string name  )
{
	removeMovableObject( name );
}

//set the current camera, that will be used to make a snapshop
void SceneManager::setCurrentCamera( shared_ptr<Camera> cam )
{ 
	mCamera = cam; 
}

//find a camera with a given name
shared_ptr<Camera> SceneManager::getCamera( std::string name )
{
	std::map<std::string,shared_ptr<MovableObject>>::iterator it = mMovableObjectCollection.find(name);

	if( it != mMovableObjectCollection.end() ) {
		return dynamic_pointer_cast<Camera>(it->second);
	}
	return shared_ptr<Camera>();
}

//create a light with a given name
shared_ptr<Light> SceneManager::createLight( std::string name  )
{
	shared_ptr<Light> light( new Light( name ) );
	addMovableObject( light );
	mLights.push_back( light );
	return light;
}

//destroy a light
void SceneManager::destroyLight( std::string name )
{
	std::vector<shared_ptr<Light>>::iterator it = mLights.begin();
	while( it != mLights.end() ) {
		if( (*it)->getName() == name ) {
			mLights.erase( it );
			break;
		}
		++it;
	}
	removeMovableObject( name );
}


//add a movable object to the scene, i.e. an object tha can be attached to a scene node
void SceneManager::addMovableObject( shared_ptr<MovableObject> mob)
{
	mMovableObjectCollection.insert( std::pair<std::string,shared_ptr<MovableObject>>(mob->getName(),mob) );
}

//remove a movable object from the scene
void SceneManager::removeMovableObject( std::string name )
{
	std::map<std::string,shared_ptr<MovableObject>>::iterator it = mMovableObjectCollection.find(name);

	if( it == mMovableObjectCollection.end() ) return;	//not found

	shared_ptr<MovableObject> mob = it->second;

	if( mob->getSceneNode() ) {							//if attached to a scene node, detach it
		mob->getSceneNode()->detachObject( name );
	}

	mMovableObjectCollection.erase( name );				//remove from collection

	if( mLight && name == mLight->getName() )			//if the removed item is one of the current important MOs
		mLight = shared_ptr<Light>();					//mLight, mCamera, mShadowCamera
	if( mCamera && name == mCamera->getName() )
		mCamera = shared_ptr<Camera>();
	if( mShadowCamera && name == mShadowCamera->getName() )
		mShadowCamera = shared_ptr<Camera>();
}

//set D3D fog constants for shaders
void SceneManager::setFogConstants( ID3DX11EffectVariable* mfxFogColor, ID3DX11EffectVariable* mfxFogStart, ID3DX11EffectVariable* mfxFogRange )
{
	mfxFogColor->SetRawValue(&mFogColor, 0, sizeof(mFogColor));
	mfxFogStart->SetRawValue(&mFogStart, 0, sizeof(mFogStart));
	mfxFogRange->SetRawValue(&mFogRange, 0, sizeof(mFogRange));
}



