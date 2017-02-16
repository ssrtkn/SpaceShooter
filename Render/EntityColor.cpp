

#include "EntityColor.h"
#include "Root.h"
#include "RenderSystem.h"
#include "SceneManager.h"
#include "AssetManager.h"
#include "Effect.h"

using namespace Render;

void EffectColor::buildFX()
{
	if( !mEffect ) return;

    mTech				= mEffect->GetTechniqueByName("ColorTech");
    mfxWorldViewProj	= mEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	mfxMaterial			= mEffect->GetVariableByName("gMaterial");

	mfxFogColor			= mEffect->GetVariableByName("gFogColor");
	mfxFogStart			= mEffect->GetVariableByName("gFogStart");
	mfxFogRange			= mEffect->GetVariableByName("gFogRange");

	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	mInputLayout = getInputLayout( &passDesc, Mesh::VB_POSITION|Mesh::VB_COLOR );
}



EntityColor::EntityColor(std::string name) : Entity(name), mEffectColor(NULL)
{
}


EntityColor::~EntityColor(void)
{
}


void EntityColor::buildFXColor()
{
	const std::string effectname("color.fx");

	mEffectColor = dynamic_pointer_cast<EffectColor>( getEffect( effectname ) );
	if( mEffectColor != NULL) return;

	mEffectColor = shared_ptr<EffectColor>( new EffectColor(effectname) );

	storeEffect( mEffectColor );
}



void EntityColor::Draw4Light( CXMMATRIX world )
{
	if( mEffectColor == NULL )
		buildFXColor();

    Root * root = Root::getInstance();
	RenderSystem *rs = RenderSystem::getInstance();
	SceneManager *sm = SceneManager::getInstance();

    ID3D11DeviceContext* d3dImmediateContext = rs->getDeviceContext();		//get immediate context

	d3dImmediateContext->IASetInputLayout( mEffectColor->mInputLayout );
    d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = 7*sizeof(float);
	UINT offset = 0;
	ID3D11Buffer* vertexBuffer = mMesh->getVertexBuffer( Mesh::VB_POSITION|Mesh::VB_COLOR );
	ID3D11Buffer* indexBuffer  = mMesh->getIndexBuffer();
	d3dImmediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    d3dImmediateContext->IASetIndexBuffer( indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	shared_ptr<Camera> cam = sm->getCurrentCamera();

	// Set constants
    XMMATRIX worldViewProj = world * cam->getViewMatrix() * cam->getProjectionMatrix();

    mEffectColor->mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	setMaterialShaderConstants( mEffectColor->mfxMaterial, NULL, NULL, NULL );

    D3DX11_TECHNIQUE_DESC techDesc;
    mEffectColor->mTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mEffectColor->mTech->GetPassByIndex(p)->Apply(0, d3dImmediateContext);
        d3dImmediateContext->DrawIndexed(mMesh->getNumIndices(), 0, 0);
    }
}


 

