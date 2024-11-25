/*!
@file INCPixel.hlsli
@brief ピクセルシェーダで使用するインターフェイス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/

Texture2D g_texture : register(t0);
Texture2D normalMap : register(t1);
Texture2D g_DepthMap : register(t2);



SamplerState g_sampler : register(s0);
SamplerComparisonState g_SamplerDepthMap : register(s1);
