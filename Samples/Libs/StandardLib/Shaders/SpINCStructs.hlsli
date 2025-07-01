
/*!
@file INCStructs.hlsli
@brief シェーダー構造体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

struct VSInput
{
    float4 Position : POSITION;
};


struct VSPCInput
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct PSPCInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct VSPTInput
{
    float4 position : POSITION;
    float2 tex : TEXCOORD;
};

struct PSPTInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
};

struct VSPCTInput
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD;
};

struct PSPCTInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD;
};

struct VSPNInput
{
    float4 position : POSITION;
    float3 norm : NORMAL;
};

struct PSPNInput
{
    float4 position : SV_POSITION;
    float3 norm : NORMAL;
};



struct VSPNTInput
{
    float4 position : POSITION;
    float3 norm : NORMAL;
    float2 tex : TEXCOORD;
};

struct VSPNTBoneInput
{
    float4 position : Position;
    float3 norm : NORMAL;
    float2 tex : TEXCOORD0;
    uint4 indices : BLENDINDICES0;
    float4 weights : BLENDWEIGHT0;
};




struct VSInputVc
{
    float4 Position : POSITION;
    float4 Color    : COLOR;
};



struct VSInputTx
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct VSInputTxVc
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Color    : COLOR;
};


struct VSInputVcTx
{
	float4 Position : POSITION;
	float4 Color    : COLOR;
	float2 TexCoord : TEXCOORD0;
};


struct VSInputNm
{
    float4 Position : POSITION;
    float3 Normal   : NORMAL;
};

struct VSInputNmVc
{
    float4 Position : POSITION;
    float3 Normal   : NORMAL;
    float4 Color    : COLOR;
};


struct VSInputNmTx
{
    float4 Position : POSITION;
    float3 Normal   : NORMAL;
    float2 TexCoord : TEXCOORD0;
};

struct VSInputNmTxVc
{
    float4 Position : POSITION;
    float3 Normal   : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float4 Color    : COLOR;
};


struct VSInputNmTxTangent
{
    float4 Position : POSITION;
    float3 Normal   : NORMAL;
    float4 Tangent  : TANGENT;
    float2 TexCoord : TEXCOORD0;
};



struct VSInputNmTxVcTangent
{
    float4 Position : POSITION;
    float3 Normal   : NORMAL;
    float4 Tangent  : TANGENT;
    float2 TexCoord : TEXCOORD0;
    float4 Color    : COLOR;
};



struct VSInputTx2
{
    float4 Position  : POSITION;
    float2 TexCoord  : TEXCOORD0;
    float2 TexCoord2 : TEXCOORD1;
};

struct VSInputTx2Vc
{
    float4 Position  : POSITION;
    float2 TexCoord  : TEXCOORD0;
    float2 TexCoord2 : TEXCOORD1;
    float4 Color     : COLOR;
};

struct VSInputNmTxWeights
{
    float4 Position : POSITION;
    float3 Normal   : NORMAL;
    float2 TexCoord : TEXCOORD0;
    uint4  Indices  : BLENDINDICES0;
    float4 Weights  : BLENDWEIGHT0;
};

struct VSInputNmTxTangentWeights
{
	float4 Position : POSITION;
	float3 Normal   : NORMAL;
	float4 Tangent  : TANGENT;
	float2 TexCoord : TEXCOORD0;
	uint4  Indices  : BLENDINDICES0;
	float4 Weights  : BLENDWEIGHT0;
};


// Vertex shader output structures.

struct VSOutput
{
    float4 Diffuse    : COLOR0;
    float4 Specular   : COLOR1;
    float4 PositionPS : SV_Position;
};

struct VSOutputNoFog
{
    float4 Diffuse    : COLOR0;
    float4 PositionPS : SV_Position;
};

struct VSOutputTx
{
    float4 Diffuse    : COLOR0;
    float4 Specular   : COLOR1;
    float2 TexCoord   : TEXCOORD0;
    float4 PositionPS : SV_Position;
};

struct VSOutputTxShadow
{
	float4 Diffuse    : COLOR0;
	float4 Specular   : COLOR1;
	float2 TexCoord   : TEXCOORD0;
	float3 norm      : NORMAL0;
	float3 lightRay  : NORMAL1;
	float3 lightView : NORMAL2;
	float4 lightSpacePos : POSITION1;
	float4 PositionPS : SV_Position;
};



struct VSOutputTxNoFog
{
    float4 Diffuse    : COLOR0;
    float2 TexCoord   : TEXCOORD0;
    float4 PositionPS : SV_Position;
};

struct VSOutputPixelLighting
{
    float4 PositionWS : TEXCOORD0;
    float3 NormalWS   : TEXCOORD1;
    float4 Diffuse    : COLOR0;
    float4 PositionPS : SV_Position;
};

struct VSOutputPixelLightingTx
{
    float2 TexCoord   : TEXCOORD0;
    float4 PositionWS : TEXCOORD1;
    float3 NormalWS   : TEXCOORD2;
    float4 Diffuse    : COLOR0;
    float4 PositionPS : SV_Position;
};



struct VSOutputPixelLightingTxShadow
{
	float2 TexCoord   : TEXCOORD0;
	float4 PositionWS : TEXCOORD1;
	float3 NormalWS   : TEXCOORD2;
	float4 Diffuse    : COLOR0;
	float3 norm      : NORMAL0;
	float3 lightRay  : NORMAL1;
	float3 lightView : NORMAL2;
	float4 lightSpacePos : POSITION1;
	float4 PositionPS : SV_Position;
};




struct VSOutputPixelLightingTxTangent
{
    float2 TexCoord   : TEXCOORD0;
    float4 PositionWS : TEXCOORD1;
    float3 NormalWS   : TEXCOORD2;
    float3 TangentWS  : TEXCOORD3;
    float4 Diffuse    : COLOR0;
    float4 PositionPS : SV_Position;
};

struct VSOutputPixelLightingTxTangentShadow
{
	float2 TexCoord   : TEXCOORD0;
	float4 PositionWS : TEXCOORD1;
	float3 NormalWS   : TEXCOORD2;
	float3 TangentWS  : TEXCOORD3;
	float4 Diffuse    : COLOR0;
	float3 norm      : NORMAL0;
	float3 lightRay  : NORMAL1;
	float3 lightView : NORMAL2;
	float4 lightSpacePos : POSITION1;
	float4 PositionPS : SV_Position;
};


struct VSOutputTx2
{
    float4 Diffuse    : COLOR0;
    float4 Specular   : COLOR1;
    float2 TexCoord   : TEXCOORD0;
    float2 TexCoord2  : TEXCOORD1;
    float4 PositionPS : SV_Position;
};

struct VSOutputTx2NoFog
{
    float4 Diffuse    : COLOR0;
    float2 TexCoord   : TEXCOORD0;
    float2 TexCoord2  : TEXCOORD1;
    float4 PositionPS : SV_Position;
};

struct VSOutputTxEnvMap
{
    float4 Diffuse    : COLOR0;
    float4 Specular   : COLOR1;
    float2 TexCoord   : TEXCOORD0;
    float3 EnvCoord   : TEXCOORD1;
    float4 PositionPS : SV_Position;
};



// Pixel shader input structures.

struct PSPNTInput
{
    float4 position : SV_POSITION;
    float3 norm : NORMAL;
    float4 color : COLOR;
    float2 tex : TEXCOORD;
};

struct PSPNTInputShadow
{
    float4 position : SV_Position;
    float3 norm : NORMAL0;
    float3 lightRay : NORMAL1;
    float3 lightView : NORMAL2;
    float4 color : COLOR;
    float2 tex : TEXCOORD0;
    float4 lightSpacePos : POSITION1;
};



struct PSInput
{
    float4 Diffuse  : COLOR0;
    float4 Specular : COLOR1;
};

struct PSInputNoFog
{
    float4 Diffuse : COLOR0;
};

struct PSInputTx
{
    float4 Diffuse  : COLOR0;
    float4 Specular : COLOR1;
    float2 TexCoord : TEXCOORD0;
};

struct PSInputTxShadow
{
	float4 Diffuse  : COLOR0;
	float4 Specular : COLOR1;
	float2 TexCoord : TEXCOORD0;
	float3 norm      : NORMAL0;
	float3 lightRay  : NORMAL1;
	float3 lightView : NORMAL2;
	float4 lightSpacePos : POSITION1;
};



struct PSInputTxNoFog
{
    float4 Diffuse  : COLOR0;
    float2 TexCoord : TEXCOORD0;
};

struct PSInputPixelLighting
{
    float4 PositionWS : TEXCOORD0;
    float3 NormalWS   : TEXCOORD1;
    float4 Diffuse    : COLOR0;
};

struct PSInputPixelLightingTx
{
    float2 TexCoord   : TEXCOORD0;
    float4 PositionWS : TEXCOORD1;
    float3 NormalWS   : TEXCOORD2;
    float4 Diffuse    : COLOR0;
};

struct PSInputPixelLightingTxShadow
{
	float2 TexCoord   : TEXCOORD0;
	float4 PositionWS : TEXCOORD1;
	float3 NormalWS   : TEXCOORD2;
	float4 Diffuse    : COLOR0;
	float3 norm      : NORMAL0;
	float3 lightRay  : NORMAL1;
	float3 lightView : NORMAL2;
	float4 lightSpacePos : POSITION1;
};



struct PSInputPixelLightingTxTangent
{
    float2 TexCoord   : TEXCOORD0;
    float4 PositionWS : TEXCOORD1;
    float3 NormalWS   : TEXCOORD2;
    float3 TangentWS  : TEXCOORD3;
    float4 Diffuse    : COLOR0;
};

struct PSInputPixelLightingTxTangentShadow
{
	float2 TexCoord   : TEXCOORD0;
	float4 PositionWS : TEXCOORD1;
	float3 NormalWS   : TEXCOORD2;
	float3 TangentWS  : TEXCOORD3;
	float4 Diffuse    : COLOR0;
	float3 norm      : NORMAL0;
	float3 lightRay  : NORMAL1;
	float3 lightView : NORMAL2;
	float4 lightSpacePos : POSITION1;
};



struct PSInputTx2
{
    float4 Diffuse   : COLOR0;
    float4 Specular  : COLOR1;
    float2 TexCoord  : TEXCOORD0;
    float2 TexCoord2 : TEXCOORD1;
};

struct PSInputTx2NoFog
{
    float4 Diffuse   : COLOR0;
    float2 TexCoord  : TEXCOORD0;
    float2 TexCoord2 : TEXCOORD1;
};

struct PSInputTxEnvMap
{
    float4 Diffuse  : COLOR0;
    float4 Specular : COLOR1;
    float2 TexCoord : TEXCOORD0;
    float3 EnvCoord : TEXCOORD1;
};

