//--------------------------------------------------------------------------------------
// File: SpPSPTStatic.hlsl
//
//--------------------------------------------------------------------------------------

#include "INCStructs.hlsli"
#include "SpINCConstant.hlsli"


float4 main(PSPTInput input) : SV_TARGET
{
	float4 Light = Diffuse + Emissive;
	Light.a = Diffuse.a;
	if (Activeflags.x) {
		Light = g_texture.Sample(g_sampler, input.tex) * Light;
	}
	return Light;
}

