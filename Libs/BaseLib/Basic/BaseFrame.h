/*!
@file BaseFrame.h
@brief フレームクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	フレームのパラメータ（構造体）
	//--------------------------------------------------------------------------------------
	struct FrameParam {
		ComPtr<ID3D12Resource> m_cbvUploadHeap;
		void* m_pConstantBuffer;
		UINT m_constBuffIndex;
	};

	//--------------------------------------------------------------------------------------
	///	フレームクラス(構造体)
	//--------------------------------------------------------------------------------------
	struct BaseFrame {
		vector<FrameParam> m_frameParamVec;
		ComPtr<ID3D12PipelineState> m_pipelineState;

		ComPtr<ID3D12CommandAllocator> m_updateCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_updateCommandList;

		ComPtr<ID3D12CommandAllocator> m_beginCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_beginCommandList;

		ComPtr<ID3D12CommandAllocator> m_shadowCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_shadowCommandList;

		ComPtr<ID3D12CommandAllocator> m_sceneCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_sceneCommandList;


		ComPtr<ID3D12CommandAllocator> m_endCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_endCommandList;

//		CD3DX12_CPU_DESCRIPTOR_HANDLE m_shadowSRVCPUHandle;

		ComPtr<ID3D12Resource> m_shadowTexture;
		D3D12_CPU_DESCRIPTOR_HANDLE m_shadowDepthView;
		D3D12_GPU_DESCRIPTOR_HANDLE m_shadowDepthHandle;


		UINT64 m_fenceValue;
		BaseFrame(ID3D12PipelineState* pPso, D3D12_VIEWPORT* pViewport, UINT frameResourceIndex);
		~BaseFrame();
	private:
	};



}
// end namespace basecross
