

#include "EntityBillboard.h"
#include "Root.h"
#include "RenderSystem.h"
#include "SceneManager.h"
#include "AssetManager.h"
#include "Light.h"
#include "Effect.h"


using namespace Render;

void EffectBillboard::buildFX()
{
	if( !mEffect ) return;

	const std::string effectname("Billboard.fx");

    mTechNoFog			= mEffect->GetTechniqueByName("BillboardTechNoFog");
    mTechFog			= mEffect->GetTechniqueByName("BillboardTechFog");
	mTechShadow			= mEffect->GetTechniqueByName("BuildShadowMapAlphaClipTech");

    mfxWorld			= mEffect->GetVariableByName("gWorld")->AsMatrix();
    mfxViewProj			= mEffect->GetVariableByName("gViewProj")->AsMatrix();
	mfxEyePosW          = mEffect->GetVariableByName("gEyePosW")->AsVector();
	
	mfxDirLight         = mEffect->GetVariableByName("gDirLight");
	mfxPointLight       = mEffect->GetVariableByName("gPointLight");
	mfxSpotLight        = mEffect->GetVariableByName("gSpotLight");
	mfxMaterial         = mEffect->GetVariableByName("gMaterial");

	mfxFogColor			= mEffect->GetVariableByName("gFogColor");
	mfxFogStart			= mEffect->GetVariableByName("gFogStart");
	mfxFogRange			= mEffect->GetVariableByName("gFogRange");

	mfxDiffuseMap       = mEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();

	D3DX11_PASS_DESC passDesc;
	mTechNoFog->GetPassByIndex(0)->GetDesc(&passDesc);
	mInputLayout = getInputLayout( &passDesc, Mesh::VB_POSITION|Mesh::VB_SIZE );
}




EntityBillboard::EntityBillboard( std::string name ) : 
	Entity(name), mEffectBB()
{
}


EntityBillboard::~EntityBillboard(void)
{
}


void EntityBillboard::buildFXBB()
{
	const std::string effectname("Billboard.fx");

	mEffectBB = dynamic_pointer_cast<EffectBillboard>( getEffect( effectname ) );
	if( mEffectBB != NULL) return;

	mEffectBB = shared_ptr<EffectBillboard>( new EffectBillboard(effectname) );

	storeEffect( mEffectBB );
}


void EntityBillboard::Draw4Light( CXMMATRIX world )
{
	Draw4Light( world, SceneNode::DT_LIGHT );
}

void EntityBillboard::Draw4Light( CXMMATRIX world, SceneNode::DrawType dt  )
{
	if( mEffectBB==NULL )
		buildFXBB();

    Root * root = Root::getInstance();
	RenderSystem *rs = RenderSystem::getInstance();
	SceneManager *sm = SceneManager::getInstance();

    ID3D11DeviceContext* d3dImmediateContext = rs->getDeviceContext();		//get immediate context

	ID3DX11EffectTechniquePtr techn;										//choose the technique

	if( dt == SceneNode::DT_LIGHT ) {
		if( sm->isFogEnabled() ) {
			techn = mEffectBB->mTechFog;
			setFogShaderConstants( mEffectBB->mfxFogColor, mEffectBB->mfxFogStart, mEffectBB->mfxFogRange );
		}
		else 
			techn = mEffectBB->mTechNoFog;
	}
	else
		techn = mEffectBB->mTechShadow;

	D3DX11_PASS_DESC passDesc;
	techn->GetPassByIndex(0)->GetDesc(&passDesc);
    d3dImmediateContext->IASetInputLayout( mEffectBB->mInputLayout );
    d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	UINT stride = 5*sizeof(float);
	UINT offset = 0;
	ID3D11Buffer* vertexBuffer = mMesh->getVertexBuffer( Mesh::VB_POSITION|Mesh::VB_SIZE );
	d3dImmediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// Set per frame constants.
	shared_ptr<Camera> cam = sm->getCurrentCamera();
	if( cam ) {
		XMVECTOR ep = cam->getPosition( SceneNode::TS_WORLD );		//get camera position in world
		XMFLOAT3 eyePosW;
		XMStoreFloat3( &eyePosW, ep );
		mEffectBB->mfxEyePosW->SetRawValue(&eyePosW, 0, sizeof(eyePosW));
	}

	setLightShaderConstants( mEffectBB->mfxDirLight, mEffectBB->mfxPointLight, mEffectBB->mfxSpotLight );

	if( dt == SceneNode::DT_SHADOW ) {
		cam = sm->getShadowCamera();
	}

    // Set constants
	XMMATRIX wld = world;
    XMMATRIX viewProj = cam->getViewMatrix() * cam->getProjectionMatrix();
	XMMATRIX worldInvTranspose = SceneNode::computeInverseTranspose( world );

    mEffectBB->mfxWorld->SetMatrix(reinterpret_cast<float*>(&wld));
    mEffectBB->mfxViewProj->SetMatrix(reinterpret_cast<float*>(&viewProj));

	setMaterialShaderConstants( mEffectBB->mfxMaterial, mEffectBB->mfxDiffuseMap, NULL, NULL );

    D3DX11_TECHNIQUE_DESC techDesc;
    techn->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        techn->GetPassByIndex(p)->Apply(0, d3dImmediateContext);
        d3dImmediateContext->Draw(mMesh->getNumVertices(), 0);
    }

}


void EntityBillboard::Draw4Shadow( CXMMATRIX world )
{
	Draw4Light( world, SceneNode::DT_SHADOW );
}

