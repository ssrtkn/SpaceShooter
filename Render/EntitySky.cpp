

#include "EntitySky.h"
#include "Root.h"
#include "SceneManager.h"
#include "RenderSystem.h"
#include "AssetManager.h"
#include "Effect.h"

using namespace Render;

void EffectSky::buildFX()
{
	if( !mEffect ) return;

    mTechNoFog			 = mEffect->GetTechniqueByName("SkyTechNoFog");
    mTechFog			 = mEffect->GetTechniqueByName("SkyTechFog");
    mfxWorldViewProj     = mEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	mfxCubeMap			 = mEffect->GetVariableByName("gCubeMap")->AsShaderResource();

	mfxMaterial			 = mEffect->GetVariableByName("gMaterial");

	mfxFogColor			 = mEffect->GetVariableByName("gFogColor");
	mfxFogStart			 = mEffect->GetVariableByName("gFogStart");
	mfxFogRange			 = mEffect->GetVariableByName("gFogRange");

	D3DX11_PASS_DESC passDesc;
	mTechNoFog->GetPassByIndex(0)->GetDesc(&passDesc);
	mInputLayout = getInputLayout( &passDesc, Mesh::VB_POSITION );
}




EntitySky::EntitySky( std::string name ) : Entity(name), mEffectSky()
{
}


EntitySky::~EntitySky(void)
{
}


void EntitySky::buildFXSky()
{
	const std::string effectname("Sky.fx");

	mEffectSky = dynamic_pointer_cast<EffectSky>( getEffect( effectname ) );
	if( mEffectSky != NULL) return;

	mEffectSky = shared_ptr<EffectSky>( new EffectSky(effectname) );

	storeEffect( mEffectSky );
}


void EntitySky::Draw4Light( CXMMATRIX world )
{
	if( mEffectSky == NULL )
		buildFXSky();

	Root * root = Root::getInstance();
	RenderSystem *rs = RenderSystem::getInstance();
	SceneManager *sm = SceneManager::getInstance();

    ID3D11DeviceContext* d3dImmediateContext = rs->getDeviceContext();		//get immediate context

	ID3DX11EffectTechniquePtr techn;										//choose the technique
	if( sm->isFogEnabled() ) {
		techn = mEffectSky->mTechFog;
		setFogShaderConstants( mEffectSky->mfxFogColor, mEffectSky->mfxFogStart, mEffectSky->mfxFogRange );
	}
	else techn = mEffectSky->mTechNoFog;

	D3DX11_PASS_DESC passDesc;
	techn->GetPassByIndex(0)->GetDesc(&passDesc);
	d3dImmediateContext->IASetInputLayout( mEffectSky->mInputLayout );
    d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = 3*sizeof(float);
	UINT offset = 0;
	ID3D11Buffer* vertexBuffer = mMesh->getVertexBuffer( Mesh::VB_POSITION );
	ID3D11Buffer* indexBuffer  = mMesh->getIndexBuffer();
	d3dImmediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    d3dImmediateContext->IASetIndexBuffer( indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	    // Set constants
	XMMATRIX the_world = world;
	shared_ptr<Camera> cam = sm->getCurrentCamera();
	if( cam ) {
		XMVECTOR ep = cam->getPosition( SceneNode::TS_WORLD );		//get camera position in world

		mSceneNode->setPosition( ep );
		the_world = mSceneNode->getTransform( SceneNode::TS_WORLD );
	}

    XMMATRIX worldViewProj = the_world * cam->getViewMatrix() * cam->getProjectionMatrix();
	
    mEffectSky->mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	setMaterialShaderConstants( mEffectSky->mfxMaterial, mEffectSky->mfxCubeMap, NULL, NULL );	//set cube map

	D3DX11_TECHNIQUE_DESC techDesc;
    techn->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        techn->GetPassByIndex(p)->Apply(0, d3dImmediateContext);
        d3dImmediateContext->DrawIndexed(mMesh->getNumIndices(), 0, 0);
    }
}

