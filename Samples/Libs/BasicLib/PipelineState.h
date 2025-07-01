/*!
@file PipelineState.h
@brief �p�C�v���C���X�e�[�g�܂Ƃ�
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	�p�C�v���C���X�e�[�g�N���X(static�Ăяo��������)
	//--------------------------------------------------------------------------------------
	class PipelineStatePool {
		//�p�C�v���C���X�e�[�g��map
		static std::map<std::wstring, ComPtr<ID3D12PipelineState>> m_pipelineStateMap;
	public:
		static ComPtr<ID3D12PipelineState> GetPipelineState(const std::wstring& key, bool ExceptionActive = false);
		static void AddPipelineState(const std::wstring& key, ComPtr<ID3D12PipelineState> pipelineState);
		static void ReleasePipelineStates();
	};
}
// end namespace basecross
