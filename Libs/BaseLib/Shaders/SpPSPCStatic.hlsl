//--------------------------------------------------------------------------------------
// File: SpPSPCStatic.hlsl
//
//--------------------------------------------------------------------------------------

#include "INCStructs.hlsli"
#include "SpINCConstant.hlsli"

float4 main(PSPCInput input) : SV_TARGET
{
	float4 Light = (saturate(input.color) * Diffuse) + Emissive;
	return Light;
}
