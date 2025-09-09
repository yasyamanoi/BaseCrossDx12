/*!
@file INCShadowmap.hlsli
@brief シャドウマップインクルード
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

cbuffer ShadowmapConstantBuffer : register(b0)
{
	float4x4 world				: packoffset(c0);
	float4x4 view				: packoffset(c4);
	float4x4 projection				: packoffset(c8);
	float4x3 bones[200]              : packoffset(c12);
};

struct VS_INPUT
{
	float4 position : POSITION;
};

struct VS_INPUT_PNT
{
	float4 position : POSITION;
	float3 normal   : NORMAL;
	float2 texcoord : TEXCOORD0;
};



struct VS_INPUT_WEIGHTS
{
	float4 position : POSITION;
	float3 normal   : NORMAL;
	float2 texcoord : TEXCOORD0;
	uint4  indices  : BLENDINDICES0;
	float4 weights  : BLENDWEIGHT0;
};

struct VS_INPUT_TANGENT_WEIGHTS
{
	float4 position : POSITION;
	float3 normal   : NORMAL;
	float4 tangent  : TANGENT;
	float2 texcoord : TEXCOORD0;
	uint4  indices  : BLENDINDICES0;
	float4 weights  : BLENDWEIGHT0;
};

struct VS_OUTPUT
{
	float4 vPosition    : SV_POSITION;
};


