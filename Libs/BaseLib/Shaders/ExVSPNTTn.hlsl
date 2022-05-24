#include "ExINCStructs.hlsli"


PSInput main(float3 position : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD0, float3 tangent : TANGENT)
{
	PSInput result;

	float4 newPosition = float4(position, 1.0f);

	normal.z *= -1.0f;
	newPosition = mul(newPosition, model);

	result.worldpos = newPosition;

	newPosition = mul(newPosition, view);
	newPosition = mul(newPosition, projection);

	result.position = newPosition;
	result.uv = uv;
	result.normal = normal;
	result.tangent = tangent;

	return result;
}
