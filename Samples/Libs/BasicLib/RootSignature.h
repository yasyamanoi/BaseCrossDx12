/*!
@file RootSignature.h
@brief ���[�g�V�O�l�`���܂Ƃ�
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	���[�g�V�O�l�`���N���X(static�Ăяo��������)
	//--------------------------------------------------------------------------------------
	class RootSignaturePool {
		//���[�g�V�O�l�`����map
		static std::map<std::wstring, ComPtr<ID3D12RootSignature>> m_rootSignatureMap;
	public:
		static ComPtr<ID3D12RootSignature> GetRootSignature(const std::wstring& key, bool ExceptionActive = false);
		static void AddRootSignature(const std::wstring& key, ComPtr<ID3D12RootSignature> rootSignature);
		static void ReleaseRootSignature();
	};

}
// end namespace basecross
