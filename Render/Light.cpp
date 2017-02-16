

#include "Light.h"

using namespace Render;

//constructor
Light::Light( std::string name ) : 
	MovableObject(name),
	mType( LT_DIRECTIONAL )
{
	mDirectionalLight.Ambient  = XMFLOAT4(0.1f,0.1f,0.1f,1.0f);	//set colors to default values
	mDirectionalLight.Diffuse  = XMFLOAT4(0.3f,0.3f,0.3f,1.0f);
	mDirectionalLight.Specular = XMFLOAT4(0.1f,0.1f,0.1f,1.0f);
}

//destructor
Light::~Light(void)
{
}

//turn the light into a directional light
void Light::setDirectional( XMFLOAT4 Ambient, XMFLOAT4 Diffuse, XMFLOAT4 Specular )
{
	mDirectionalLight.Ambient = Ambient;
	mDirectionalLight.Diffuse = Diffuse;
	mDirectionalLight.Specular = Specular;
	mType = LT_DIRECTIONAL;
}

//turn the light into a point light
void Light::setPointLight( XMFLOAT4 Ambient, XMFLOAT4 Diffuse, XMFLOAT4 Specular, 
					float Range, XMFLOAT3 Att )
{
	mPointLight.Ambient = Ambient;
	mPointLight.Diffuse = Diffuse;
	mPointLight.Specular = Specular;
	mPointLight.Range = Range;
	mPointLight.Att = Att;
	mType = LT_POINT;

}

//turn the light into a spot light
void Light::setSpotLight( XMFLOAT4 Ambient, XMFLOAT4 Diffuse, XMFLOAT4 Specular,
							float Range, float Spot, XMFLOAT3 Att )
{
	mSpotLight.Ambient = Ambient;
	mSpotLight.Diffuse = Diffuse;
	mSpotLight.Specular = Specular;
	mSpotLight.Range = Range;
	mSpotLight.Spot = Spot;
	mSpotLight.Att = Att;
	mType = LT_SPOTLIGHT;
}

//set shader constants
void Light::setShaderConstants( ID3DX11EffectVariable *mfxDirLight, 
								ID3DX11EffectVariable *mfxPointLight, 
								ID3DX11EffectVariable *mfxSpotLight  )
{
	
	if( mType == LT_DIRECTIONAL && mfxDirLight!=NULL && mSceneNode != NULL ){								//for directional light
		XMStoreFloat3( &mDirectionalLight.Direction, mSceneNode->getDirectionZ( SceneNode::TS_WORLD ));

		mfxDirLight->SetRawValue(&mDirectionalLight, 0, sizeof(mDirectionalLight));							//load up struct values
	}

	if( mType == LT_POINT && mfxPointLight!=NULL && mSceneNode != NULL){								//for point light
		XMStoreFloat3( &mPointLight.Position, mSceneNode->getPosition( SceneNode::TS_WORLD ) );

		mfxPointLight->SetRawValue(&mPointLight, 0, sizeof(mPointLight));								//load up struct values
	}

	if( mType == LT_SPOTLIGHT && mfxSpotLight!=NULL && mSceneNode != NULL){							//for spot light
		XMStoreFloat3( &mSpotLight.Position, mSceneNode->getPosition( SceneNode::TS_WORLD ) );		//need position and orientation from scene node
		XMStoreFloat3( &mSpotLight.Direction, mSceneNode->getDirectionZ( SceneNode::TS_WORLD ) );

		mfxSpotLight->SetRawValue(&mSpotLight, 0, sizeof(mSpotLight));								//load up struct values
	}

}

