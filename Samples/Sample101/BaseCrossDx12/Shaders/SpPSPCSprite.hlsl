//--------------------------------------------------------------------------------------
// File: SpPSPCSprite.hlsl
//
//--------------------------------------------------------------------------------------

#include "SpINC2dConstant.hlsli"
#include "SpINCStructs.hlsli"

float4 main(PSPCInput input) : SV_TARGET
{
	//頂点色からライティングを作成
	float4 Light = (saturate(input.color) * Diffuse) + Emissive;
	return Light;
}
