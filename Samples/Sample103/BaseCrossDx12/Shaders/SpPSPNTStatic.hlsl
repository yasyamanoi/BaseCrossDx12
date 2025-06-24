//--------------------------------------------------------------------------------------
// File: SpPSPNTStatic.hlsl
//
//--------------------------------------------------------------------------------------

#include "SpINCStructs.hlsli"
#include "SpINCConstant.hlsli"
#include "SpINCPixel.hlsli"

float4 main(PSPNTInput input) : SV_TARGET
{
	//法線ライティング
	float3 lightdir = normalize(LightDir.xyz);
	float3 N1 = normalize(input.norm);
	float4 Light = (saturate(dot(N1, -lightdir)) * Diffuse) + Emissive;
	Light += input.color;
	Light.a = Diffuse.a;
	if (Activeflags.x) {
		Light = g_texture.Sample(g_sampler, input.tex) * Light;
	}
	return Light;
}
