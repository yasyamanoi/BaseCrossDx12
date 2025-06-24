//--------------------------------------------------------------------------------------
// File: SpPSPTStatic.hlsl
//
//--------------------------------------------------------------------------------------

#include "SpINCStructs.hlsli"
#include "SpINCConstant.hlsli"
#include "SpINCPixel.hlsli"


float4 main(PSPTInput input) : SV_TARGET
{
	float4 Light = Diffuse + Emissive;
	Light.a = Diffuse.a;
	if (Activeflags.x) {
		Light = g_texture.Sample(g_sampler, input.tex) * Light;
	}
	return Light;
}

