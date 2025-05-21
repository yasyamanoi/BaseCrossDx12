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
@file FrameResource.h
@brief フレームクラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	using namespace Microsoft::WRL;

	struct BaseConstantBufferSet {
		ComPtr<ID3D12Resource> m_baseConstantBuffer;
		void* m_pBaseConstantBufferWO; // WRITE-ONLY pointers to the constant base buffer
	};

	class FrameResource
	{
	public:
		// Performance tip: Minimize the number of command allocators and command lists.
		// Each open command list needs it's own command allocator.
		ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		ComPtr<ID3D12CommandAllocator> m_contextCommandAllocator;

		//BaseCross用
		std::vector<BaseConstantBufferSet> m_baseConstantBufferSetVec;

	public:
		FrameResource(ID3D12Device* pDevice, ID3D12CommandQueue* pCommandQueue);
		virtual ~FrameResource();

		virtual void InitFrame();

		template<typename T>
		size_t AddBaseConstantBufferSet(ID3D12Device* pDevice) {
			BaseConstantBufferSet cbSet;
			const CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
			auto size = m_baseConstantBufferSetVec.size();
			ThrowIfFailed(CreateConstantBuffer(pDevice, sizeof(T), &cbSet.m_baseConstantBuffer, nullptr, D3D12_RESOURCE_STATE_GENERIC_READ));
			NAME_D3D12_OBJECT(cbSet.m_baseConstantBuffer);
			ThrowIfFailed(cbSet.m_baseConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&cbSet.m_pBaseConstantBufferWO)));
			m_baseConstantBufferSetVec.push_back(cbSet);
			return size;
		}

	};

	inline HRESULT CreateConstantBuffer(
		ID3D12Device* pDevice,
		UINT size,
		ID3D12Resource** ppResource,
		D3D12_CPU_DESCRIPTOR_HANDLE* pCpuCbvHandle = nullptr,
		D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_GENERIC_READ)
	{
		try
		{
			*ppResource = nullptr;

			const UINT alignedSize = CalculateConstantBufferByteSize(size);
			ThrowIfFailed(pDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(alignedSize),
				initState,
				nullptr,
				IID_PPV_ARGS(ppResource)));

			if (pCpuCbvHandle)
			{
				// Describe and create the shadow constant buffer view (CBV).
				D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
				cbvDesc.SizeInBytes = alignedSize;
				cbvDesc.BufferLocation = (*ppResource)->GetGPUVirtualAddress();
				pDevice->CreateConstantBufferView(&cbvDesc, *pCpuCbvHandle);
			}
		}
		catch (HrException& e)
		{
			SAFE_RELEASE(*ppResource);
			return e.Error();
		}
		return S_OK;
	}
}
// end namespace basecross

