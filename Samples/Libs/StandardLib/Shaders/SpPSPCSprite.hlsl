//--------------------------------------------------------------------------------------
// File: SpPSPCSprite.hlsl
//
//--------------------------------------------------------------------------------------

#include "SpINC2dConstant.hlsli"
#include "SpINCStructs.hlsli"

float4 main(PSPCInput input) : SV_TARGET
{
	//���_�F���烉�C�e�B���O���쐬
	float4 Light = (saturate(input.color) * Diffuse) + Emissive;
	return Light;
}
