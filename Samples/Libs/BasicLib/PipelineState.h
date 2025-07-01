/*!
@file PipelineState.h
@brief パイプラインステートまとめ
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
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
		static ComPtr<ID3D12PipelineState> GetPipelineState(const std::wstring& key, bool ExceptionActive = false);
		static void AddPipelineState(const std::wstring& key, ComPtr<ID3D12PipelineState> pipelineState);
		static void ReleasePipelineStates();
	};
}
// end namespace basecross
