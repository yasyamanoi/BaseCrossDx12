//--------------------------------------------------------------------------------------
// File: VSShadowmap.hlsl
//
//--------------------------------------------------------------------------------------


#include "INCShadowmap.hlsli"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	input.position.w = 1.0f;
	output.vPosition = mul(input.position, world);
	output.vPosition = mul(output.vPosition, view);
	output.vPosition = mul(output.vPosition, projection);

	return output;
}

