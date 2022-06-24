/*!
@file BasicPipelineState.cpp
@brief パイプラインステートまとめ
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#include "stdafx.h"

namespace basecross {

	map<wstring, ComPtr<ID3D12PipelineState>> BasicPipelineStatePool::m_pipelineStateMap;

	ComPtr<ID3D12PipelineState> BasicPipelineStatePool::GetPipelineState(const wstring& key, bool ExceptionActive) {
		auto it = m_pipelineStateMap.begin();
		while (it != m_pipelineStateMap.end()) {
			if (it->first == key) {
				return it->second;
			}
			it++;
		}
		if (ExceptionActive) {
			throw BaseException(
				L"指定のパイプラインステートが見つかりません",
				key,
				L"BasicPipelineState::GetPipelineState()"
			);
		}
		return nullptr;
	}

	void BasicPipelineStatePool::AddPipelineState(const wstring& key, ComPtr<ID3D12PipelineState> pipelineState) {
		m_pipelineStateMap[key] = pipelineState;
	}

}
// end namespace basecross
