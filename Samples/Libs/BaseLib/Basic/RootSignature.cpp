/*!
@file RootSignature.cpp
@brief ルートシグネチャまとめ　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {

	std::map<std::wstring, ComPtr<ID3D12RootSignature>> RootSignaturePool::m_rootSignatureMap;

	ComPtr<ID3D12RootSignature> RootSignaturePool::GetRootSignature(const std::wstring& key, bool ExceptionActive) {
		auto it = m_rootSignatureMap.begin();
		while (it != m_rootSignatureMap.end()) {
			if (it->first == key) {
				return it->second;
			}
			it++;
		}
		if (ExceptionActive) {
			throw BaseException(
				L"指定のルートシグネチャが見つかりません",
				key,
				L"RootSignaturePool::GetRootSignature()"
			);
		}
		return nullptr;
	}

	void RootSignaturePool::AddRootSignature(const std::wstring& key, ComPtr<ID3D12RootSignature> rootSignature) {
		m_rootSignatureMap[key] = rootSignature;
	}

	void RootSignaturePool::ReleaseRootSignature() {
		auto it = m_rootSignatureMap.begin();
		while (it != m_rootSignatureMap.end()) {
			it->second.Reset();
			it++;
		}
		m_rootSignatureMap.clear();
	}

}
// end namespace basecross
