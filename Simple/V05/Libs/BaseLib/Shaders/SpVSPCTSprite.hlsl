//--------------------------------------------------------------------------------------
// File: SpVSPCTSprite.hlsl
//
//--------------------------------------------------------------------------------------

#include "SpINC2dConstant.hlsli"
#include "INCStructs.hlsli"


PSPCTInput main(VSPCTInput input)
{
	PSPCTInput result;

	result.position = mul(input.position, MatrixTransform);
	result.color = input.color;
	result.tex = input.tex;

	return result;
}

