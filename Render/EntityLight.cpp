

#include "EntityLight.h"
#include "Root.h"
#include "RenderSystem.h"
#include "SceneManager.h"
#include "AssetManager.h"
#include "Light.h"
#include "Effect.h"


using namespace Render;


//
//Connect all FX variables for lighting and fogging with member variables
//This is called from the constructor of EffectLight()
//Base class has been constructed and already loaded the FX file from disk and created the effect
//only need to connect variables now
//
void EffectLight::buildFX()
{
	if( !mEffect ) return;	//effect already created, so return

    mTechNoFog				= mEffect->GetTechniqueByName("LightTechNoFog");	//techniques
    mTechFog				= mEffect->GetTechniqueByName("LightTechFog");
    mTechNoLighting			= mEffect->GetTechniqueByName("NoLighting");

    mfxWorldViewProj		= mEffect->GetVariableByName("gWorldViewProj")->AsMatrix();		//transforms
	mfxWorld				= mEffect->GetVariableByName("gWorld")->AsMatrix();
	mfxWorldInvTranspose	= mEffect->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	mfxShadowTransform		= mEffect->GetVariableByName("gShadowTransforms")->AsMatrix();
	mfxEyePosW				= mEffect->GetVariableByName("gEyePosW")->AsVector();

	mfxDirLight				= mEffect->GetVariableByName("gDirLight");		//lights
	mfxPointLight			= mEffect->GetVariableByName("gPointLight");
	mfxSpotLight			= mEffect->GetVariableByName("gSpotLight");

	mfxMaterial				= mEffect->GetVariableByName("gMaterial");		//material

	mfxFogColor				= mEffect->GetVariableByName("gFogColor");		//fog
	mfxFogStart				= mEffect->GetVariableByName("gFogStart");
	mfxFogRange				= mEffect->GetVariableByName("gFogRange");

	mfxDiffuseMap			= mEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();	//
	mfxShadowMap			= mEffect->GetVariableByName("gShadowMaps")->AsShaderResource();

	D3DX11_PASS_DESC passDesc;
	mTechNoFog->GetPassByIndex(0)->GetDesc(&passDesc);
	mInputLayout = getInputLayout( &passDesc, Mesh::VB_POSITION|Mesh::VB_NORMAL|Mesh::VB_TEXCOORD );	//create fitting vertex layout
}


//-----------------------------------------------------------


//
//Constructor. Calls Entity constructor
//
EntityLight::EntityLight(std::string name) : Entity( name ), mLightingEnabled(true), mEffectLight()
{
}

//destructor
EntityLight::~EntityLight(void)
{
}


//
//Set the effect name and get it or create a new one
//This is called from Draw4Light
//
void EntityLight::buildFXLight()
{
	const std::string effectname("Lighting.fx");	//file name of FX file

	mEffectLight = dynamic_pointer_cast<EffectLight>( getEffect( effectname ) );	//set it from asset manager
	if( mEffectLight != NULL) return;												//found it so return

	//not found -> create it
	//Call sequence:
	//1. Effect::Effect() constructor
	//2. Effect::Effect() calls Effect::buildFX() which loads the FX file and constructs the effect
	//3. EffectLight::EffectLight() constructor
	//4. Calls EffectLight::buildFX() which connects the effect variables
	mEffectLight = shared_ptr<EffectLight>( new EffectLight(effectname) );	

	storeEffect( mEffectLight );													//store in asset manager
}


//
//Draw the entity for the light pass
//Called from Entity::Draw(), is a virtual function
//
void EntityLight::Draw4Light( CXMMATRIX world )
{
	if( mEffectLight == NULL )			//make sure you have the light effect
		buildFXLight();					//if not build it

    Root * root = Root::getInstance();
	RenderSystem *rs = RenderSystem::getInstance();
	SceneManager *sm = SceneManager::getInstance();

    ID3D11DeviceContext* d3dImmediateContext = rs->getDeviceContext();		//get immediate context

	ID3DX11EffectTechniquePtr techn;										//choose the technique
	if( mLightingEnabled ) {
		if( sm->isFogEnabled() ) {
			techn = mEffectLight->mTechFog;
			setFogShaderConstants( mEffectLight->mfxFogColor, mEffectLight->mfxFogStart, mEffectLight->mfxFogRange );
		}
		else 
			techn = mEffectLight->mTechNoFog;
	}
	else
		techn = mEffectLight->mTechNoLighting;

	d3dImmediateContext->IASetInputLayout( mEffectLight->mInputLayout );
    d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = 8*sizeof(float);
	UINT offset = 0;
	ID3D11Buffer* vertexBuffer = mMesh->getVertexBuffer( Mesh::VB_POSITION|Mesh::VB_NORMAL|Mesh::VB_TEXCOORD );
	ID3D11Buffer* indexBuffer  = mMesh->getIndexBuffer();
	d3dImmediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    d3dImmediateContext->IASetIndexBuffer( indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set per frame constants.
	shared_ptr<Camera> cam = sm->getCurrentCamera();
	if( cam ) {
		XMVECTOR ep = cam->getPosition( SceneNode::TS_WORLD );		//get camera position in world
		XMFLOAT3 eyePosW;
		XMStoreFloat3( &eyePosW, ep );
		mEffectLight->mfxEyePosW->SetRawValue(&eyePosW, 0, sizeof(eyePosW));
	}

	setLightShaderConstants( mEffectLight->mfxDirLight, mEffectLight->mfxPointLight, mEffectLight->mfxSpotLight );

    // Set constants
    XMMATRIX worldViewProj = world * cam->getViewMatrix() * cam->getProjectionMatrix();
	XMMATRIX worldInvTranspose = SceneNode::computeInverseTranspose( world );

	XMMATRIX shadowTransforms[4];
	for( UINT i=0; i<4; i++ ) {
		shadowTransforms[i] = world * sm->getShadowTransform(i);
	}

	XMFLOAT4X4 wit;
	XMStoreFloat4x4( &wit, worldInvTranspose );

    mEffectLight->mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
	mEffectLight->mfxWorld->SetMatrix(reinterpret_cast<const float*>(&world));
	mEffectLight->mfxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&worldInvTranspose));
	mEffectLight->mfxShadowTransform->SetMatrixArray(reinterpret_cast<const float*>(&shadowTransforms[0]),0,4);

	setMaterialShaderConstants( mEffectLight->mfxMaterial, mEffectLight->mfxDiffuseMap, NULL, NULL );
	mEffectLight->mfxShadowMap->SetResource( sm->getShadow()->getDepthMapSRV() );

    D3DX11_TECHNIQUE_DESC techDesc;
    techn->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        techn->GetPassByIndex(p)->Apply(0, d3dImmediateContext);
        d3dImmediateContext->DrawIndexed(mMesh->getNumIndices(), 0, 0);
    }
}

