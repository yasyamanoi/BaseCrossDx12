/*!
@file BasePipelineState.cpp
@brief パイプラインステートまとめ　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {

	std::map<std::wstring, ComPtr<ID3D12PipelineState>> PipelineStatePool::m_pipelineStateMap;

	ComPtr<ID3D12PipelineState> PipelineStatePool::GetPipelineState(const std::wstring& key, bool ExceptionActive) {
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
				L"PipelineStatePool::GetPipelineState()"
			);
		}
		return nullptr;
	}

	void PipelineStatePool::AddPipelineState(const std::wstring& key, ComPtr<ID3D12PipelineState> pipelineState) {
		m_pipelineStateMap[key] = pipelineState;
	}

	void PipelineStatePool::ReleasePipelineStates() {
		auto it = m_pipelineStateMap.begin();
		while (it != m_pipelineStateMap.end()) {
			it->second.Reset();
			it++;
		}
		m_pipelineStateMap.clear();
	}

}
// end namespace basecross
