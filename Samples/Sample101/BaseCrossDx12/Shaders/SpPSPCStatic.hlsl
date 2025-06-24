//--------------------------------------------------------------------------------------
// File: SpPSPCStatic.hlsl
//
//--------------------------------------------------------------------------------------

#include "SpINCStructs.hlsli"
#include "SpINCConstant.hlsli"
#include "SpINCPixel.hlsli"

float4 main(PSPCInput input) : SV_TARGET
{
	float4 Light = (saturate(input.color) * Diffuse) + Emissive;
	return Light;
}

