/*!
@file BasicPipelineState.h
@brief パイプラインステートまとめ
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	パイプラインステートクラス(static呼び出しをする)
	//--------------------------------------------------------------------------------------
	class BasicPipelineStatePool {
		//パイプラインステートのmap
		static map<wstring, ComPtr<ID3D12PipelineState>> m_pipelineStateMap;
	public:
		static ComPtr<ID3D12PipelineState> GetPipelineState(const wstring& key, bool ExceptionActive = false);
		static void AddPipelineState(const wstring& key, ComPtr<ID3D12PipelineState> pipelineState);
	};
}
// end namespace basecross
