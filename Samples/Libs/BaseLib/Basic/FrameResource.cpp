//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

/*!
@file FrameResource.cpp
@brief フレームクラス　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {


	using namespace SceneEnums;

	FrameResource::FrameResource(ID3D12Device* pDevice, ID3D12CommandQueue* pCommandQueue)
	{
		ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
		NAME_D3D12_OBJECT(m_commandAllocator);

		// Create command list allocators for worker threads. One per open command list.
		ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_contextCommandAllocator)));
		NAME_D3D12_OBJECT(m_contextCommandAllocator);

	}

	FrameResource::~FrameResource()
	{
	}

	void FrameResource::InitFrame()
	{
		// Reset the main thread command allocator.
		m_commandAllocator->Reset();
		// Reset the worker command allocators.
		ThrowIfFailed(m_contextCommandAllocator->Reset());
	}

}
// end namespace basecross

