//=============================================================================
// Lighting.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Transforms and lights geometry.
//=============================================================================

#include "LightHelper.fx"
 
cbuffer cbPerFrame
{
	DirectionalLight gDirLight;
	PointLight gPointLight;
	SpotLight gSpotLight;
	float3 gEyePosW;

	float4 gFogColor;
	float  gFogStart;
	float  gFogRange;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gShadowTransforms[4];
	float    gShadowDistances[4];
	Material gMaterial;
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;
Texture2DArray gShadowMaps;

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
 
SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerComparisonState samShadow
{
	Filter   = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    ComparisonFunc = LESS_EQUAL;
};
 
RasterizerState CullCW
{
    FrontCounterClockwise = FALSE;
};


struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex     : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION0;
    float3 NormalW : NORMAL;
	float2 Tex        : TEXCOORD0;
	float4 ShadowPosH[4] : POSITION1;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to world space space.
	vout.PosW    = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose).xyz;
		
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	vout.Tex = vin.Tex;

	// Generate projective tex-coords to project shadow map onto scene.
	[unroll]
	for( int i=0; i<4; i++ ) {
		vout.ShadowPosH[i] = mul(float4(vin.PosL, 1.0f), gShadowTransforms[i]);
	}

	return vout;
}


float4 PS( VertexOut pin, uniform bool gLightingEnabled, uniform bool gFogEnabled, uniform int gNumShadows ) : SV_Target
{
	// Interpolating normal can unnormalize it, so normalize it.
    pin.NormalW = normalize(pin.NormalW); 

	float3 toEyeW = gEyePosW - pin.PosW;
	float distToEye = length( toEyeW  );
	toEyeW /= distToEye;

	float4 litColor = gMaterial.NumTextures ? float4(gDiffuseMap.Sample( samAnisotropic, pin.Tex ).rgb, gMaterial.Diffuse.a) : float4(1.0f, 1.0f, 1.0f, gMaterial.Diffuse.a);

	if( gLightingEnabled ) {
		// Start with a sum of zero. 
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

		//choose shadow map
		int shadowIdx = 0*(distToEye<gShadowDistances[1]) + 
						1*(distToEye>=gShadowDistances[1] && distToEye<gShadowDistances[2]) +
						2*(distToEye>=gShadowDistances[2] && distToEye<gShadowDistances[3]) + 
						3*(distToEye>=gShadowDistances[3]);

		float shadow = CalcShadowFactor(samShadow, gShadowMaps, pin.ShadowPosH[0]);
		//float shadow = CalcSimpleShadowFactor(samLinear, gShadowMaps, pin.ShadowPosH[0]);

		// Sum the light contribution from each light source.
		float4 A, D, S;

		ComputeDirectionalLight(gMaterial, gDirLight, pin.NormalW, toEyeW, A, D, S);
		ambient += A;  
		diffuse += shadow*D;
		spec    += shadow*S;

		ComputePointLight(gMaterial, gPointLight, pin.PosW, pin.NormalW, toEyeW, A, D, S);
		ambient += A;
		diffuse += D;
		spec    += S;

		ComputeSpotLight(gMaterial, gSpotLight, pin.PosW, pin.NormalW, toEyeW, A, D, S);
		ambient += A;
		diffuse += D;
		spec    += S;

		litColor =  litColor*(ambient + diffuse) + spec;
	}

	//fogging
	if( gFogEnabled ) {
		float fogLerp = saturate( (distToEye - gFogStart) / gFogRange ); 
		litColor = lerp(litColor, gFogColor, fogLerp);	// Blend the fog color and the lit color.
	}
    return litColor;
}



technique11 NoLighting
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS(false, false, 1) ) );

        SetRasterizerState(CullCW);
    }
}

technique11 LightTechNoFog
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(true, false, 1) ) );

        SetRasterizerState(CullCW);
    }
}

technique11 LightTechFog
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(true, true, 1) ) );

        SetRasterizerState(CullCW);
    }
}




