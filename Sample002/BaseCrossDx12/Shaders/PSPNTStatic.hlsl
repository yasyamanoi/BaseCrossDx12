/*!
@file PSPNTStatic.hlsli
@brief PNT�s�N�Z���V�F�[�_
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/

#include "INCStructs.hlsli"
#include "INCConstant.hlsli"
#include "INCPixel.hlsli"

float4 main(PSPNTInput input) : SV_TARGET
{
 //   return float4(1, 0, 0, 1);
	//�@�����C�e�B���O
    float3 lightdir = normalize(LightDir.xyz);
    float3 N1 = normalize(input.norm);
    float4 Light = (saturate(dot(N1, -lightdir)) * Diffuse) + Emissive;
    Light += input.color;
    Light.a = Diffuse.a;
    if (Activeflags.x)
    {
        Light = g_texture.Sample(g_sampler, input.tex) * Light;
    }
    return Light;
}