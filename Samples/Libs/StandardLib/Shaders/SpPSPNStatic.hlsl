//--------------------------------------------------------------------------------------
// File: SpPSPNStatic.hlsl
//
//--------------------------------------------------------------------------------------

#include "SpINCStructs.hlsli"
#include "SpINCConstant.hlsli"
#include "SpINCPixel.hlsli"

float4 main(PSPNInput input) : SV_TARGET
{
	//�@�����C�e�B���O
	float3 lightdir = normalize(LightDir.xyz);
	float3 N1 = normalize(input.norm);
	float4 Light = saturate(dot(N1, -lightdir) * Diffuse + Emissive);
	Light.a = Diffuse.a;
	return Light;
}
