/*!
@file BcVSPNTStaticPL.hlsli
@brief PNTスタティックピクセルライティング頂点シェーダー
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "BcINCConstant.hlsli"
#include "BcINCStructs.hlsli"
#include "BcINCCommon.hlsli"
#include "BcINCLighting.hlsli"

// Vertex shader: pixel lighting + texture.
// VSBasicPixelLightingTx
VSOutputPixelLightingTx main(VSInputNmTx vin)
{
	VSOutputPixelLightingTx vout;

	CommonVSOutputPixelLighting cout = ComputeCommonVSOutputPixelLighting(vin.Position, vin.Normal);
	SetCommonVSOutputParamsPixelLighting;

	vout.Diffuse = float4(1, 1, 1, DiffuseColor.a);
	vout.TexCoord = vin.TexCoord;

	return vout;
}
