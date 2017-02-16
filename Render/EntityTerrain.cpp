

#include "EntityTerrain.h"
#include "Root.h"
#include "RenderSystem.h"
#include "SceneManager.h"
#include "AssetManager.h"
#include "Effect.h"
#include "Light.h"
#include "MeshGrid.h"

using namespace Render;

void EffectTerrain::buildFX()
{
	if( !mEffect ) return;

    mTechNoFog			 = mEffect->GetTechniqueByName("TerrainTechNoFog");
    mTechFog			 = mEffect->GetTechniqueByName("TerrainTechFog");
    mfxWorldViewProj     = mEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	mfxWorld             = mEffect->GetVariableByName("gWorld")->AsMatrix();
	mfxWorldInvTranspose = mEffect->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	mfxShadowTransform		= mEffect->GetVariableByName("gShadowTransforms")->AsMatrix();
	mfxEyePosW           = mEffect->GetVariableByName("gEyePosW")->AsVector();

	mfxDirLight          = mEffect->GetVariableByName("gDirLight");
	mfxPointLight        = mEffect->GetVariableByName("gPointLight");
	mfxSpotLight         = mEffect->GetVariableByName("gSpotLight");

	mfxFogColor			 = mEffect->GetVariableByName("gFogColor");
	mfxFogStart			 = mEffect->GetVariableByName("gFogStart");
	mfxFogRange			 = mEffect->GetVariableByName("gFogRange");

	mfxMaterial			 = mEffect->GetVariableByName("gMaterial");

	mfxLayerMapArray.push_back( mEffect->GetVariableByName("gLayerMap0")->AsShaderResource() );
	mfxLayerMapArray.push_back( mEffect->GetVariableByName("gLayerMap1")->AsShaderResource() );
	mfxLayerMapArray.push_back( mEffect->GetVariableByName("gLayerMap2")->AsShaderResource() );
	mfxLayerMapArray.push_back( mEffect->GetVariableByName("gLayerMap3")->AsShaderResource() );
	mfxLayerMapArray.push_back( mEffect->GetVariableByName("gLayerMap4")->AsShaderResource() );

	mfxBlendMap			 = mEffect->GetVariableByName("gBlendMap")->AsShaderResource();
	mfxShadowMap		 = mEffect->GetVariableByName("gShadowMaps")->AsShaderResource();
	mfxTexFactor		 = mEffect->GetVariableByName("gTexFactor");

	D3DX11_PASS_DESC passDesc;
	mTechNoFog->GetPassByIndex(0)->GetDesc(&passDesc);
	mInputLayout = getInputLayout( &passDesc, Mesh::VB_POSITION|Mesh::VB_NORMAL|Mesh::VB_TEXCOORD );}
	


EntityTerrain::EntityTerrain( std::string name ) : Entity(name), mEffectTerrain()
{
}


EntityTerrain::~EntityTerrain(void)
{
}



void EntityTerrain::buildFXTerrain()
{
	const std::string effectname("Terrain.fx");

	mEffectTerrain = dynamic_pointer_cast<EffectTerrain>( getEffect( effectname ) );
	if( mEffectTerrain != NULL) return;

	mEffectTerrain = shared_ptr<EffectTerrain>( new EffectTerrain(effectname) );

	storeEffect( mEffectTerrain );
}
	


void EntityTerrain::Draw4Light( CXMMATRIX world )
{
	if( mEffectTerrain == NULL )
		buildFXTerrain();

    Root * root = Root::getInstance();
	RenderSystem *rs = RenderSystem::getInstance();
	SceneManager *sm = SceneManager::getInstance();

    ID3D11DeviceContext* d3dImmediateContext = rs->getDeviceContext();		//get immediate context

	ID3DX11EffectTechniquePtr techn;										//choose the technique
	if( sm->isFogEnabled() ) {
		techn = mEffectTerrain->mTechFog;
		setFogShaderConstants( mEffectTerrain->mfxFogColor, mEffectTerrain->mfxFogStart, mEffectTerrain->mfxFogRange );
	}
	else 
		techn = mEffectTerrain->mTechNoFog;

	D3DX11_PASS_DESC passDesc;
	techn->GetPassByIndex(0)->GetDesc(&passDesc);
    d3dImmediateContext->IASetInputLayout( mEffectTerrain->mInputLayout );
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
		mEffectTerrain->mfxEyePosW->SetRawValue(&eyePosW, 0, sizeof(eyePosW));
	}

	setLightShaderConstants( mEffectTerrain->mfxDirLight, mEffectTerrain->mfxPointLight, mEffectTerrain->mfxSpotLight );

    // Set constants
    XMMATRIX worldViewProj = world * cam->getViewMatrix() * cam->getProjectionMatrix();
	XMMATRIX worldInvTranspose = SceneNode::computeInverseTranspose( world );

	XMMATRIX shadowTransforms[4];
	for( UINT i=0; i<4; i++ ) {
		shadowTransforms[i] = world * sm->getShadowTransform(i);
	}

	XMFLOAT4X4 wit;
	XMStoreFloat4x4( &wit, worldInvTranspose );

    mEffectTerrain->mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
	mEffectTerrain->mfxWorld->SetMatrix(reinterpret_cast<const float*>(&world));
	mEffectTerrain->mfxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&worldInvTranspose));
	mEffectTerrain->mfxShadowTransform->SetMatrixArray(reinterpret_cast<const float*>(&shadowTransforms[0]),0,4);

	float texFactor = mMesh->getTexFactor();
	mEffectTerrain->mfxTexFactor->SetRawValue(&texFactor, 0, sizeof(float));

	setMaterialShaderConstants( mEffectTerrain->mfxMaterial, mEffectTerrain->mfxLayerMapArray, NULL, mEffectTerrain->mfxBlendMap );
	mEffectTerrain->mfxShadowMap->SetResource( sm->getShadow()->getDepthMapSRV() );

    D3DX11_TECHNIQUE_DESC techDesc;
    techn->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        techn->GetPassByIndex(p)->Apply(0, d3dImmediateContext);
        d3dImmediateContext->DrawIndexed(mMesh->getNumIndices(), 0, 0);
    }

}



XMVECTOR EntityTerrain::projectOnTerrain( CXMVECTOR pos )
{
	if( mMesh==NULL ) return pos;

	XMVECTOR vpos = pos;
	if( mSceneNode ) {
		vpos = mSceneNode->changeToLocalSpace( vpos, SceneNode::TS_WORLD );
	}

	shared_ptr<MeshGrid> mg = dynamic_pointer_cast<MeshGrid>(mMesh);
	
	XMFLOAT3 fpos;
	XMStoreFloat3( &fpos, vpos );
	fpos.y = mg->getY( fpos.x, fpos.z );
	vpos = XMLoadFloat3( &fpos );

	if( mSceneNode ) {
		vpos = XMVector4Transform( vpos, mSceneNode->getTransform( SceneNode::TS_WORLD ) );
	}
	return vpos;
}


bool EntityTerrain::isAboveGround( CXMVECTOR pos )
{
	if( mMesh==NULL ) return false;

	XMVECTOR vpos = pos;
	if( mSceneNode ) {
		vpos = mSceneNode->changeToLocalSpace( vpos, SceneNode::TS_WORLD );
	}

	shared_ptr<MeshGrid> mg = dynamic_pointer_cast<MeshGrid>(mMesh);
	
	XMFLOAT3 fpos;
	XMStoreFloat3( &fpos, vpos );
	return fpos.y > mg->getY( fpos.x, fpos.z );
}




