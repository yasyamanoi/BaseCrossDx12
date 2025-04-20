/*!
@file BlendState.cpp
@brief ブレンンドステートまとめ
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	レンダリングステートクラス
	//--------------------------------------------------------------------------------------
	D3D12_BLEND_DESC BlendState::GetOpaqueBlend() {
		D3D12_BLEND_DESC brendDesk{};
		brendDesk.AlphaToCoverageEnable = FALSE;
		brendDesk.IndependentBlendEnable = FALSE;
		const D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc =
		{
			FALSE,							//BOOL BlendEnable
			FALSE,							//BOOL LogicOpEnable
			D3D12_BLEND_ONE,				//D3D12_BLEND SrcBlend
			D3D12_BLEND_ZERO,				//D3D12_BLEND DestBlend
			D3D12_BLEND_OP_ADD,				//D3D12_BLEND_OP BlendOp
			D3D12_BLEND_ONE,				//D3D12_BLEND SrcBlendAlpha
			D3D12_BLEND_ZERO,				//D3D12_BLEND DestBlendAlpha
			D3D12_BLEND_OP_ADD,				//D3D12_BLEND_OP BlendOpAlpha
			D3D12_LOGIC_OP_NOOP,			//D3D12_LOGIC_OP LogicOp
			D3D12_COLOR_WRITE_ENABLE_ALL,	//UINT8 RenderTargetWriteMask
		};
		for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
			brendDesk.RenderTarget[i] = renderTargetBlendDesc;
		return brendDesk;
	}
	//--------------------------------------------------------------------------------------
	/*!
	@brief	単純な透明処理の取得
	@return	ブレンド定義
	*/
	//--------------------------------------------------------------------------------------
	D3D12_BLEND_DESC BlendState::GetAlphaBlend() {
		D3D12_BLEND_DESC brendDesk{};
		brendDesk.AlphaToCoverageEnable = FALSE;
		brendDesk.IndependentBlendEnable = FALSE;
		const D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc =
		{
			TRUE,							//BOOL BlendEnable
			FALSE,							//BOOL LogicOpEnable
			D3D12_BLEND_ONE,				//D3D12_BLEND SrcBlend
			D3D12_BLEND_INV_SRC_ALPHA,		//D3D12_BLEND DestBlend
			D3D12_BLEND_OP_ADD,				//D3D12_BLEND_OP BlendOp
			D3D12_BLEND_ONE,				//D3D12_BLEND SrcBlendAlpha
			D3D12_BLEND_ZERO,				//D3D12_BLEND DestBlendAlpha
			D3D12_BLEND_OP_ADD,				//D3D12_BLEND_OP BlendOpAlpha
			D3D12_LOGIC_OP_NOOP,			//D3D12_LOGIC_OP LogicOp
			D3D12_COLOR_WRITE_ENABLE_ALL,	//UINT8 RenderTargetWriteMask
		};
		for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
			brendDesk.RenderTarget[i] = renderTargetBlendDesc;
		return brendDesk;
	}
	//--------------------------------------------------------------------------------------
	/*!
	@brief	半透明処理の取得
	@return	ブレンド定義
	*/
	//--------------------------------------------------------------------------------------
	D3D12_BLEND_DESC BlendState::GetAlphaBlendEx() {
		D3D12_BLEND_DESC brendDesk{};

		brendDesk.AlphaToCoverageEnable = FALSE;
		brendDesk.IndependentBlendEnable = FALSE;
		const D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc =
		{
			TRUE,							//BOOL BlendEnable
			FALSE,							//BOOL LogicOpEnable
			D3D12_BLEND_SRC_ALPHA,			//D3D12_BLEND SrcBlend
			D3D12_BLEND_INV_SRC_ALPHA,		//D3D12_BLEND DestBlend
			D3D12_BLEND_OP_ADD,				//D3D12_BLEND_OP BlendOp
			D3D12_BLEND_ONE,				//D3D12_BLEND SrcBlendAlpha
			D3D12_BLEND_ZERO,				//D3D12_BLEND DestBlendAlpha
			D3D12_BLEND_OP_ADD,				//D3D12_BLEND_OP BlendOpAlpha
			D3D12_LOGIC_OP_NOOP,			//D3D12_LOGIC_OP LogicOp
			D3D12_COLOR_WRITE_ENABLE_ALL,	//UINT8 RenderTargetWriteMask
		};
		for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
			brendDesk.RenderTarget[i] = renderTargetBlendDesc;

		return brendDesk;
	}
	//--------------------------------------------------------------------------------------
	/*!
	@brief	加算処理の取得
	@return	ブレンド定義
	*/
	//--------------------------------------------------------------------------------------
	D3D12_BLEND_DESC BlendState::GetAdditiveBlend() {
		D3D12_BLEND_DESC brendDesk = {};

		return brendDesk;
	}
	//--------------------------------------------------------------------------------------
	/*!
	@brief	アルファを使用した元のデータと対象データをブレンド処理の取得
	@return	ブレンド定義
	*/
	//--------------------------------------------------------------------------------------
	D3D12_BLEND_DESC BlendState::GetNonPremultipliedBlend() {
		D3D12_BLEND_DESC brendDesk = {};

		return brendDesk;
	}
	//--------------------------------------------------------------------------------------
	/*!
	@brief	アルファテスト
	@return	ブレンドステートインターフェイスのポインタ
	*/
	//--------------------------------------------------------------------------------------
	D3D12_BLEND_DESC BlendState::GetAlphaToCoverageBlend() {
		D3D12_BLEND_DESC brendDesk = {};

		return brendDesk;
	}


}
// end namespace basecross
