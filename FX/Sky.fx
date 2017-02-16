cbuffer cbPerFrame
{
	float4x4 gWorldViewProj;

	float4 gFogColor;
	float  gFogStart;
	float  gFogRange;
};
 
TextureCube gCubeMap;

SamplerState samTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VertexIn
{
	float3 PosL : POSITION;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};
 
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	

	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj).xyww;
	

	vout.PosL = vin.PosL;
	
	return vout;
}

float4 PS(VertexOut pin, uniform bool gFogEnabled) : SV_Target
{
	float4 litColor = gCubeMap.Sample(samTriLinearSam, pin.PosL);


	if( gFogEnabled ) {
		float fogLerp = gFogColor.w; 
		litColor = lerp(litColor, gFogColor, fogLerp);	
	}
	return litColor;
}

RasterizerState NoCull
{
    CullMode = None;
};

DepthStencilState LessEqualDSS
{

    DepthFunc = LESS_EQUAL;
};

technique11 SkyTechNoFog
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS(false) ) );
        
        SetRasterizerState(NoCull);
        SetDepthStencilState(LessEqualDSS, 0);
    }
}

technique11 SkyTechFog
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS(true) ) );
        
        SetRasterizerState(NoCull);
        SetDepthStencilState(LessEqualDSS, 0);
    }
}
