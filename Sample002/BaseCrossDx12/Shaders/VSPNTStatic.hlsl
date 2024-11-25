/*!
@file VSPNTStatic.hlsli
@brief PNT頂点シェーダ
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/

#include "INCStructs.hlsli"
#include "INCConstant.hlsli"

PSPNTInput main(VSPNTInput input)
{
    
    PSPNTInput result;
	//頂点の位置を変換
    float4 pos = float4(input.position.xyz, 1.0f);
	//ワールド変換
    pos = mul(pos, World);
	//ビュー変換
    pos = mul(pos, View);
	//射影変換
    pos = mul(pos, Projection);
	//ピクセルシェーダに渡す変数に設定
    result.position = pos;
	//ライティング
    result.norm = mul(input.norm, (float3x3) World);
    result.norm = normalize(result.norm);
	//スペキュラー
    float3 H = normalize(normalize(-LightDir.xyz) + normalize(EyePos.xyz - pos.xyz));
    result.color = Specular * dot(result.norm, H);
	//テクスチャUV
    result.tex = input.tex;
    return result;
}
