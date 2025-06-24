//--------------------------------------------------------------------------------------
// File: SpVSPTSprite.hlsl
//
//--------------------------------------------------------------------------------------

#include "SpINC2dConstant.hlsli"
#include "SpINCStructs.hlsli"


float4 main(PSPTInput input) : SV_TARGET
{
	float4 Light = Diffuse + Emissive;
	Light.a = Diffuse.a;
	Light = g_texture.Sample(g_sampler, input.tex) * Light;
	return Light;
}

