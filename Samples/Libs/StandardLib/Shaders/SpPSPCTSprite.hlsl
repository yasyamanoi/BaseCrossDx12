//--------------------------------------------------------------------------------------
// File: SpPSPCTSprite.hlsl
//
//--------------------------------------------------------------------------------------

#include "SpINC2dConstant.hlsli"
#include "SpINCStructs.hlsli"



float4 main(PSPCTInput input) : SV_TARGET
{
	float4 Light = (saturate(input.color) * Diffuse) + Emissive;
	Light.a = Diffuse.a;
	Light = g_texture.Sample(g_sampler, input.tex) * Light;
	return Light;
}

