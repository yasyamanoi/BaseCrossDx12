//--------------------------------------------------------------------------------------
// File: SpVSPTSprite.hlsl
//
//--------------------------------------------------------------------------------------

#include "SpINC2dConstant.hlsli"
#include "SpINCStructs.hlsli"

PSPTInput main(VSPTInput input)
{
	PSPTInput result;

	result.position = mul(input.position, MatrixTransform);
	result.tex = input.tex;

	return result;
}
