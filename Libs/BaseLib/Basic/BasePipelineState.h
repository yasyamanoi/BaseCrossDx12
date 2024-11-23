/*!
@file BasePipelineState.h
@brief パイプラインステートまとめ
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	パイプラインステートクラス(static呼び出しをする)
	//--------------------------------------------------------------------------------------
	class PipelineStatePool {
		//パイプラインステートのmap
		static std::map<std::wstring, ComPtr<ID3D12PipelineState>> m_pipelineStateMap;
	public:
		//パイプラインステートの取得
		static ComPtr<ID3D12PipelineState> GetPipelineState(const std::wstring& key, bool ExceptionActive = false);
		//パイプラインステートの追加
		static void AddPipelineState(const std::wstring& key, ComPtr<ID3D12PipelineState> pipelineState);
		static void ResetComPtrs();
	};
}
using namespace basecross;
// end namespace basecross
