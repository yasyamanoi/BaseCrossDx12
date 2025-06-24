//--------------------------------------------------------------------------------------
// File: SpVSPCSprite.hlsl
//
//--------------------------------------------------------------------------------------

#include "SpINC2dConstant.hlsli"
#include "SpINCStructs.hlsli"


PSPCInput main(VSPCInput input)
{
	PSPCInput result;

	result.position = mul(input.position, MatrixTransform);
	result.color = input.color;

	return result;
}
