/*!
@file FrameResource.cpp
@brief フレームリソースクラス
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#include "stdafx.h"

namespace basecross {

	FrameResource::FrameResource() :
		m_fenceValue(0)
	{
		auto pDevice = App::GetID3D12Device();
		ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
	}

	FrameResource::~FrameResource() {
		for (auto v : m_frameParamVec) {
			v.m_cbvUploadHeap->Unmap(0, nullptr);
			v.m_pConstantBuffer = nullptr;
		}
	}

}
// end basecross
