/*!
@file RootSignature.h
@brief ルートシグネチャまとめ
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	ルートシグネチャクラス(static呼び出しをする)
	//--------------------------------------------------------------------------------------
	class RootSignaturePool {
		//ルートシグネチャのmap
		static std::map<std::wstring, ComPtr<ID3D12RootSignature>> m_rootSignatureMap;
	public:
		static ComPtr<ID3D12RootSignature> GetRootSignature(const std::wstring& key, bool ExceptionActive = false);
		static void AddRootSignature(const std::wstring& key, ComPtr<ID3D12RootSignature> rootSignature);
		static void ReleaseRootSignature();
	};

}
// end namespace basecross
