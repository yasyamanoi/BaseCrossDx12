#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	フレームのパラメータ（構造体）
	//--------------------------------------------------------------------------------------
	struct ConstBuffParam {
		ComPtr<ID3D12Resource> m_cbvUploadHeap;
		void* m_pConstantBuffer;
		UINT m_constBuffIndex;
	};

	//--------------------------------------------------------------------------------------
	///	フレーム（構造体）
	//--------------------------------------------------------------------------------------
	struct BaseFrame
	{
		vector<ConstBuffParam> m_constBuffParamVec;
		UINT m_frameIndex;
		ID3D12CommandList* m_batchSubmit[BaseDevice::m_numContexts * 2 + BaseDevice::m_commandListCount];

		ComPtr<ID3D12CommandAllocator> m_commandAllocators[BaseDevice::m_commandListCount];
		ComPtr<ID3D12GraphicsCommandList> m_commandLists[BaseDevice::m_commandListCount];

		//更新用のコマンドリスト
		ComPtr<ID3D12CommandAllocator> m_updateCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_updateCommandList;

		//シャドウ描画用コマンドリスト
		ComPtr<ID3D12CommandAllocator> m_shadowCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_shadowCommandList;

		//シーン描画用コマンドリスト
		ComPtr<ID3D12CommandAllocator> m_sceneCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_sceneCommandList;

		UINT64 m_fenceValue;

		ComPtr<ID3D12Resource> m_shadowTexture;
		D3D12_CPU_DESCRIPTOR_HANDLE m_shadowDepthView;
		//NULLのシェーダーリソースのハンドル（2つ）
		D3D12_GPU_DESCRIPTOR_HANDLE m_nullSrvHandle0;
		D3D12_GPU_DESCRIPTOR_HANDLE m_nullSrvHandle1;

		D3D12_GPU_DESCRIPTOR_HANDLE m_shadowDepthHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE m_shadowCbvHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE m_sceneCbvHandle;

		BaseFrame(ID3D12Device* pDevice, ID3D12PipelineState* pPso, ID3D12PipelineState* pShadowMapPso, ID3D12DescriptorHeap* pDsvHeap, ID3D12DescriptorHeap* pCbvSrvHeap, D3D12_VIEWPORT* pViewport, UINT frameIndex);
		~BaseFrame();

		void Bind(ID3D12GraphicsCommandList* pCommandList, BOOL scenePass, D3D12_CPU_DESCRIPTOR_HANDLE* pRtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE* pDsvHandle);
		void Init();
		void SwapBarriers();
		void Finish();
	};

}
// end namespace basecross
