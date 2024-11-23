/*!
@file BaseBlendState.h
@brief ブレンンドステートまとめ
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	ブレンドステートクラス(static呼び出しをする)
	//--------------------------------------------------------------------------------------
	struct BaseBlendState {
		//塗りつぶしのブレンド定義の取得
		static D3D12_BLEND_DESC GetOpaqueBlend();
		//単純な透明処理の取得
		static D3D12_BLEND_DESC GetAlphaBlend();
		//半透明処理の取得
		static D3D12_BLEND_DESC GetAlphaBlendEx();
		//加算処理の取得
		static D3D12_BLEND_DESC GetAdditiveBlend();
		//アルファを使用した元のデータと対象データをブレンド処理の取得
		static D3D12_BLEND_DESC GetNonPremultipliedBlend();
		//アルファテスト
		static D3D12_BLEND_DESC GetAlphaToCoverageBlend();
	};


}
using namespace basecross;
// end namespace basecross
