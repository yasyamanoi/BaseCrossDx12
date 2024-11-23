//--------------------------------------------------------------------------------------
// File: VSPCSprite.hlsl
//
//--------------------------------------------------------------------------------------

#include "INC2dConstant.hlsli"
#include "INCStructs.hlsli"


PSPCInput main(VSPCInput input)
{
	PSPCInput result;

	result.position = mul(input.position, MatrixTransform);
	result.color = input.color;

	return result;
}
