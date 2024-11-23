/*!
@file BaseFrame.h
@brief フレームクラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/

#pragma once

#include "stdafx.h"

using namespace Microsoft::WRL;

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	定数バッファのパラメータ（構造体）
	//--------------------------------------------------------------------------------------
	struct ConstBuffParam {
		ComPtr<ID3D12Resource> m_constantBuffer;
		void* m_pConstantBufferWO;
		UINT m_constBuffIndex;
	};

	//--------------------------------------------------------------------------------------
	//	フレーム
	//--------------------------------------------------------------------------------------
	class BaseFrame
	{
	public:
		//最小限のコマンドアロケータ
		ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		ComPtr<ID3D12CommandAllocator> m_contextCommandAllocator;
		//コンスタントバッファの配列
		ComPtr<ID3D12Resource> m_constantBuffers[SceneEnums::RenderPass::Count];
		//コンスタントバッファハンドルの番号の配列
		UINT m_constantHandleIndeces[SceneEnums::RenderPass::Count];
		//書き込みオンリーのコンスタントバッファの配列
		void* m_pConstantBuffersWO[SceneEnums::RenderPass::Count];

		//ConstBuffParamの作成
		template<typename T>
		ConstBuffParam CreateConstBuffParam() {
			auto pDevice = BaseDevice::GetBaseDevice()->GetID3D12Device();
			auto scene = BaseDevice::GetScene();
			ConstBuffParam param = {};
			auto index = scene->GetCbvUavNextIndex();
			CD3DX12_CPU_DESCRIPTOR_HANDLE handle(
				scene->GetCbvSrvUavDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
				index,
				scene->GetCbvSrvUavDescriptorHandleIncrementSize()
			);
			ThrowIfFailed(
				CreateConstantBuffer(
					pDevice.Get(),
					sizeof(T),
					&param.m_constantBuffer,
					&handle,
					D3D12_RESOURCE_STATE_GENERIC_READ)
			);
			param.m_constBuffIndex = index;
			const CD3DX12_RANGE readRange(0, 0);
			ThrowIfFailed(param.m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&param.m_pConstantBufferWO)));
			return param;
		}


		BaseFrame(ID3D12Device* pDevice, ID3D12CommandQueue* pCommandQueue);
		virtual ~BaseFrame();
		//フレームの初期化
		virtual void InitFrame();
		//各ターンのフレームの開始
		virtual void BeginFrame(ID3D12GraphicsCommandList* pCommandList);
		//各ターンのフレームの終了
		virtual void EndFrame(ID3D12GraphicsCommandList* pCommandList);
		//GPUの仮想アドレスを得る
		inline D3D12_GPU_VIRTUAL_ADDRESS GetConstantBufferGPUVirtualAddress(SceneEnums::RenderPass::Value renderPass) const
		{
			return m_constantBuffers[renderPass]->GetGPUVirtualAddress();
		}
		//コンスタントバッファのハンドルのインデックスを得る
		inline UINT GetConstantHandleIndex(SceneEnums::RenderPass::Value  renderPass) {
			return m_constantHandleIndeces[renderPass];
		}
	};
	//コンスタントバッファの作成
	inline HRESULT CreateConstantBuffer(
		ID3D12Device* pDevice,
		UINT size,
		ID3D12Resource** ppResource,
		D3D12_CPU_DESCRIPTOR_HANDLE* pCpuCbvHandle = nullptr,
		D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_COMMON)
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
using namespace basecross;
//end namespace basecross