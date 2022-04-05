/*!
@file BaseFrame.cpp
@brief フレームクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	BaseFrame::BaseFrame(ID3D12PipelineState* pPso, ID3D12PipelineState* pShadowPso) :
		m_pipelineState(pPso),
		m_pipelineStateShadowMap(pShadowPso),
		m_fenceValue(0)
	{
		auto pDevice = App::GetID3D12Device();
		ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
		ThrowIfFailed(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList)));
		ThrowIfFailed(m_commandList->Close());
	}

	BaseFrame::~BaseFrame() {
		m_commandAllocator = nullptr;
		m_commandList = nullptr;
		for (auto v : m_frameParamVec) {
			v.m_cbvUploadHeap->Unmap(0, nullptr);
			v.m_pConstantBuffer = nullptr;
		}
	}

}
//