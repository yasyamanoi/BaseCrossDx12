#define NUM_LIGHTS 3
#define SHADOW_DEPTH_BIAS 0.00005f

struct LightState
{
	float3 position;
	float3 direction;
	float4 color;
	float4 falloff;
	float4x4 view;
	float4x4 projection;
};

cbuffer SceneConstantBuffer : register(b0)
{
	float4x4 model;
	float4x4 view;
	float4x4 projection;
	float4 ambientColor;
	bool sampleShadowMap;
	LightState lights[NUM_LIGHTS];
};

struct VSPNTInput
{
	float3 position : POSITION;
	float3 norm : NORMAL;
	float2 tex : TEXCOORD;
};

struct PSPNTInput
{
	float4 position : SV_POSITION;
	float4 worldpos : POSITION;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
};



