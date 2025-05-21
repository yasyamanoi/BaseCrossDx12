#include "ExINCStructs.hlsli"


PSPNTInput main(VSPNTInput input)
{
	PSPNTInput result;

	float4 newPosition = float4(input.position, 1.0f);

	input.norm.z *= -1.0f;
	newPosition = mul(newPosition, model);

	result.worldpos = newPosition;

	newPosition = mul(newPosition, view);
	newPosition = mul(newPosition, projection);

	result.position = newPosition;
	result.uv = input.tex;
	result.normal = input.norm;

	return result;
}

