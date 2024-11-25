//--------------------------------------------------------------------------------------
// File: PSPCSprite.hlsl
//
//--------------------------------------------------------------------------------------

#include "INC2dConstant.hlsli"
#include "INCStructs.hlsli"

float4 main(PSPCInput input) : SV_TARGET
{
	//頂点色からライティングを作成
	float4 Light = (saturate(input.color) * Diffuse) + Emissive;
	return Light;
}
