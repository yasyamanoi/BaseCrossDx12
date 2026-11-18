/*!
@file VSShadowmapBone.hlsl
@brief ボーン（スキニング）対応のシャドウマップ頂点シェーダー
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "INCShadowmap.hlsli"

//--------------------------------------------------------------------------------------
// Vertex Shader（4ボーンのスキニング）
//--------------------------------------------------------------------------------------
VS_OUTPUT main(VS_INPUT_WEIGHTS input)
{
	VS_OUTPUT output;
	float4x3 skinning = 0;
	[unroll]
	for (int i = 0; i < 4; i++)
	{
		skinning += bones[input.indices[i]] * input.weights[i];
	}
	float4 pos;
	pos.xyz = mul(float4(input.position.xyz, 1.0f), skinning);
	pos.w = 1.0f;
	output.vPosition = mul(pos, world);
	output.vPosition = mul(output.vPosition, view);
	output.vPosition = mul(output.vPosition, projection);

	return output;
}
