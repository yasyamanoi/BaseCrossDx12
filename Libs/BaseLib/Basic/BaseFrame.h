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
		ComPtr<ID3D12PipelineState> m_pipelineStateShadowMap;
		ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_commandList;

		ComPtr<ID3D12CommandAllocator> m_shadowCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_shadowCommandList;

		ComPtr<ID3D12CommandAllocator> m_sceneCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_sceneCommandList;


		UINT64 m_fenceValue;
		BaseFrame(ID3D12PipelineState* pPso, ID3D12PipelineState* pShadowPso);
		~BaseFrame();
	};



}
// end namespace basecross
