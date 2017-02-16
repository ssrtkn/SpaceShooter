
#include "Camera.h"
#include "EntityLightNormal.h"
#include "Root.h"
#include "RenderSystem.h"
#include "SceneManager.h"
#include "AssetManager.h"
#include "Light.h"
#include "Effect.h"

using namespace Render;


//
//Connect all FX variables for lighting and fogging with member variables
//This is called from the constructor of EffectLightNormal()
//Base class has been constructed and already loaded the FX file from disk and created the effect
//only need to connect variables now
//
void EffectLightNormal::buildFX()
{
	if( !mEffect ) return;

    mTechNoFog				= mEffect->GetTechniqueByName("LightNormalTechNoFog");
    mTechFog				= mEffect->GetTechniqueByName("LightNormalTechFog");
    mfxWorldViewProj		= mEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	mfxWorld				= mEffect->GetVariableByName("gWorld")->AsMatrix();
	mfxWorldInvTranspose	= mEffect->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	mfxEyePosW				= mEffect->GetVariableByName("gEyePosW")->AsVector();

	mfxDirLight				= mEffect->GetVariableByName("gDirLight");
	mfxPointLight			= mEffect->GetVariableByName("gPointLight");
	mfxSpotLight			= mEffect->GetVariableByName("gSpotLight");

	mfxMaterial				= mEffect->GetVariableByName("gMaterial");

	mfxDiffuseMap			= mEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	mfxNormalMap			= mEffect->GetVariableByName("gNormalMap")->AsShaderResource();

	mfxFogColor				= mEffect->GetVariableByName("gFogColor");
	mfxFogStart				= mEffect->GetVariableByName("gFogStart");
	mfxFogRange				= mEffect->GetVariableByName("gFogRange");

	D3DX11_PASS_DESC passDesc;
	mTechNoFog->GetPassByIndex(0)->GetDesc(&passDesc);
	mInputLayout = getInputLayout( &passDesc, Mesh::VB_POSITION|Mesh::VB_NORMAL|Mesh::VB_TEXCOORD|Mesh::VB_TANGENT );
}


//------------------------------------------------------

//constructor
EntityLightNormal::EntityLightNormal(std::string name) : Entity( name ), mEffectLN()
{
}

//destructor
EntityLightNormal::~EntityLightNormal(void)
{
}


//
//Set the effect name and get it or create a new one
//This is called from Draw4Light
//
void EntityLightNormal::buildFXLN()
{
	const std::string effectname("LightingNormal.fx");		//set effect name

	mEffectLN = dynamic_pointer_cast<EffectLightNormal>( getEffect( effectname ) );		//if already in asset manager return it
	if( mEffectLN != NULL) return;

	//not found -> create it
	//Call sequence:
	//1. Effect::Effect() constructor
	//2. Effect::Effect() calls Effect::buildFX() which loads the FX file and constructs the effect
	//3. EffectLightNormal::EffectLightNormal() constructor
	//4. Calls EffectLightNormal::buildFX() which connects the effect variables
	mEffectLN = shared_ptr<EffectLightNormal>( new EffectLightNormal(effectname) );

	storeEffect( mEffectLN );	//store in asset manager
}



void EntityLightNormal::Draw4Light( CXMMATRIX world )
{
	if( mEffectLN == NULL )
		buildFXLN();

    Root * root = Root::getInstance();
	RenderSystem *rs = RenderSystem::getInstance();
	SceneManager *sm = SceneManager::getInstance();

    ID3D11DeviceContext* d3dImmediateContext = rs->getDeviceContext();		//get immediate context

	ID3DX11EffectTechniquePtr techn;										//choose the technique
	if( sm->isFogEnabled() ) {
		techn = mEffectLN->mTechFog;
		setFogShaderConstants( mEffectLN->mfxFogColor, mEffectLN->mfxFogStart, mEffectLN->mfxFogRange );
	}
	else 
		techn = mEffectLN->mTechNoFog;

    d3dImmediateContext->IASetInputLayout( mEffectLN->mInputLayout );
    d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = 11*sizeof(float);
	UINT offset = 0;
	ID3D11Buffer* vertexBuffer = mMesh->getVertexBuffer( Mesh::VB_POSITION|Mesh::VB_NORMAL|Mesh::VB_TEXCOORD|Mesh::VB_TANGENT );
	ID3D11Buffer* indexBuffer  = mMesh->getIndexBuffer();
	d3dImmediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    d3dImmediateContext->IASetIndexBuffer( indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set per frame constants.
	shared_ptr<Camera> cam = sm->getCurrentCamera();
	if( cam ) {
		XMVECTOR ep = cam->getPosition( SceneNode::TS_WORLD );		//get camera position in world
		XMFLOAT3 eyePosW;
		XMStoreFloat3( &eyePosW, ep );
		mEffectLN->mfxEyePosW->SetRawValue(&eyePosW, 0, sizeof(eyePosW));
	}

	setLightShaderConstants( mEffectLN->mfxDirLight, mEffectLN->mfxPointLight, mEffectLN->mfxSpotLight );

    // Set constants
    XMMATRIX worldViewProj = world * cam->getViewMatrix() * cam->getProjectionMatrix();
	XMMATRIX worldInvTranspose = SceneNode::computeInverseTranspose( world );

    mEffectLN->mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
	mEffectLN->mfxWorld->SetMatrix(reinterpret_cast<const float*>(&world));
	mEffectLN->mfxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&worldInvTranspose));

	setMaterialShaderConstants( mEffectLN->mfxMaterial, mEffectLN->mfxDiffuseMap, mEffectLN->mfxNormalMap, NULL );

    D3DX11_TECHNIQUE_DESC techDesc;
    techn->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        techn->GetPassByIndex(p)->Apply(0, d3dImmediateContext);
        d3dImmediateContext->DrawIndexed(mMesh->getNumIndices(), 0, 0);
    }

}
