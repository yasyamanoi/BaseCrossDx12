/*!
@file INCPixel.hlsli
@brief �s�N�Z���V�F�[�_�Ŏg�p����C���^�[�t�F�C�X
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/

Texture2D g_texture : register(t0);
Texture2D normalMap : register(t1);
Texture2D g_DepthMap : register(t2);



SamplerState g_sampler : register(s0);
SamplerComparisonState g_SamplerDepthMap : register(s1);