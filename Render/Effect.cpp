
#include "Effect.h"
#include "Root.h"
#include "RenderSystem.h"

using namespace Render;

//
//Constructor of effect base class
//Calls buildFX() to load the FX from file
//
Effect::Effect( std::string name ) : GameObject(name), mEffect()
{
	buildFX();			//call member function of Effect (we are in the constructor so virtual is NOT called!)
						//will load the FX file from disk and compile it
}

//destructor
Effect::~Effect(void)
{
}

//
//Create a vertex input layout for this effect
//This is needed wehn drawing an entity
//The input assembler stage needs to know the vertex layout
//In: vertexFlags... specifies which elements to use
//
ID3D11InputLayoutPtr Effect::getInputLayout( D3DX11_PASS_DESC *passDesc, UINT vertexFlags )
{
	Root * root = Root::getInstance();
	RenderSystem *rs = RenderSystem::getInstance();

    ID3D11DevicePtr md3dDevice = rs->getDevice();

    //Possible input elements
    D3D11_INPUT_ELEMENT_DESC vertexDescSource[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
 		{"SIZE",	 0, DXGI_FORMAT_R32G32_FLOAT,       0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
 		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
  		{"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
  };

	//go through all input possibilities
    D3D11_INPUT_ELEMENT_DESC vertexDesc[10];
	UINT numData=0, offset=0;
	if( vertexFlags & Mesh::VB_POSITION ) {
		vertexDesc[numData] = vertexDescSource[0];
		numData++; offset+=3*sizeof(float);
	}
	if( vertexFlags & Mesh::VB_NORMAL ) {
		vertexDesc[numData] = vertexDescSource[1];
		vertexDesc[numData].AlignedByteOffset = offset;
		numData++; offset+=3*sizeof(float);
	}
	if( vertexFlags & Mesh::VB_SIZE ) {
		vertexDesc[numData] = vertexDescSource[2];
		vertexDesc[numData].AlignedByteOffset = offset;
		numData++; offset+=2*sizeof(float);
	}
	if( vertexFlags & Mesh::VB_TEXCOORD ) {
		vertexDesc[numData] = vertexDescSource[3];
		vertexDesc[numData].AlignedByteOffset = offset;
		numData++; offset+=2*sizeof(float);
	}
	if( vertexFlags & Mesh::VB_TANGENT ) {
		vertexDesc[numData] = vertexDescSource[4];
		vertexDesc[numData].AlignedByteOffset = offset;
		numData++; offset+=3*sizeof(float);
	}
	if( vertexFlags & Mesh::VB_COLOR ) {
		vertexDesc[numData] = vertexDescSource[5];
		vertexDesc[numData].AlignedByteOffset = offset;
		numData++; offset+=2*sizeof(float);
	}

	// Create the input layout for the InputAssembler
	ID3D11InputLayoutPtr inputLayout;
    HR(md3dDevice->CreateInputLayout(vertexDesc, numData, passDesc->pIAInputSignature, 
		    passDesc->IAInputSignatureSize, &inputLayout));

	return inputLayout;
}


//
//Load the FX file from disk, compile it, and create the effect from it
//
void Effect::buildFX()
{
	 DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
    shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
    
    Root * root = Root::getInstance();
	RenderSystem *rs = RenderSystem::getInstance();

    ID3D11DevicePtr md3dDevice = rs->getDevice();

    ID3D10BlobPtr compiledShader = 0;
    ID3D10BlobPtr compilationMsgs = 0;

	std::wstring fname;
	fname.assign( mName.begin(), mName.end() );						//convert string to wstring
	std::wstring filename = std::wstring(L"..\\..\\FX\\").append( fname );

	HRESULT hr = D3DX11CompileFromFile( filename.c_str(), 0, 0, 0, "fx_5_0", shaderFlags, 
        0, 0, &compiledShader, &compilationMsgs, 0);

    // compilationMsgs can store errors or warnings.
    if( compilationMsgs != 0 )
    {
        MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
        ReleaseCOM(compilationMsgs);
    }

    // Even if there are no compilationMsgs, check to make sure there were no other errors.
    if(FAILED(hr))
    {
        DXTrace(__WFILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
    }

    HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(),		//create the effect and store it in mEffect
									compiledShader->GetBufferSize(), 
									0, md3dDevice, &mEffect));

    // Done with compiled shader.
    //ReleaseCOM(compiledShader);
}
