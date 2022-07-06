/*!
@file BaseFrame.cpp
@brief フレームクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	BaseFrame::BaseFrame(ID3D12Device* pDevice, ID3D12PipelineState* pPso, ID3D12PipelineState* pShadowMapPso, ID3D12DescriptorHeap* pDsvHeap, ID3D12DescriptorHeap* pCbvSrvHeap, D3D12_VIEWPORT* pViewport, UINT frameIndex) :
		m_fenceValue(0),
		m_frameIndex(frameIndex)
		//m_pipelineState(pPso),
		//m_pipelineStateShadowMap(pShadowMapPso)
	{
		auto pBaseDev = App::GetBaseDevice();

		auto pipelineState = BasicPipelineStatePool::GetPipelineState(L"initPipelineState");
		if (!pipelineState) {
			throw BaseException(
				L"初期化用のパイプラインステートが取得できません",
				L"BaseFrame::BaseFrame()"
			);
		}
		auto pipelineStateShadowMap = BasicPipelineStatePool::GetPipelineState(L"shadowMapPipelineState");
		if (!pipelineStateShadowMap) {
			throw BaseException(
				L"シャドウマップ用のパイプラインステートが取得できません",
				L"BaseFrame::BaseFrame()"
			);
		}



		//stateごとのコマンドリスト
		for (UINT i = 0; i < BaseDevice::m_commandListCount; i++)
		{
			ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[i])));
			ThrowIfFailed(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[i].Get(), pipelineState.Get(), IID_PPV_ARGS(&m_commandLists[i])));

			NAME_D3D12_OBJECT_INDEXED(m_commandLists, i);

			// Close these command lists; don't record into them for now.
			ThrowIfFailed(m_commandLists[i]->Close());
		}

		//update用のコマンドリスト
		ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&m_updateCommandAllocator)));
		ThrowIfFailed(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_updateCommandAllocator.Get(), pipelineState.Get(), IID_PPV_ARGS(&m_updateCommandList)));
		NAME_D3D12_OBJECT(m_updateCommandList);
		ThrowIfFailed(m_updateCommandList->Close());

		//シャドウ描画用コマンドリスト
		ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&m_shadowCommandAllocator)));
		ThrowIfFailed(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_shadowCommandAllocator.Get(), pipelineStateShadowMap.Get(), IID_PPV_ARGS(&m_shadowCommandList)));
		NAME_D3D12_OBJECT(m_shadowCommandList);
		ThrowIfFailed(m_shadowCommandList->Close());

		//シーン描画用コマンドリスト
		ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
			IID_PPV_ARGS(&m_sceneCommandAllocator)));
		ThrowIfFailed(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_sceneCommandAllocator.Get(), pipelineState.Get(), IID_PPV_ARGS(&m_sceneCommandList)));
		NAME_D3D12_OBJECT(m_sceneCommandList);
		ThrowIfFailed(m_sceneCommandList->Close());

		// Describe and create the shadow map texture.
		CD3DX12_RESOURCE_DESC shadowTexDesc(
			D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			0,
			static_cast<UINT>(pViewport->Width),
			static_cast<UINT>(pViewport->Height),
			1,
			1,
			DXGI_FORMAT_R32_TYPELESS,
			1,
			0,
			D3D12_TEXTURE_LAYOUT_UNKNOWN,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		D3D12_CLEAR_VALUE clearValue;        // Performance tip: Tell the runtime at resource creation the desired clear value.
		clearValue.Format = DXGI_FORMAT_D32_FLOAT;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;

		ThrowIfFailed(pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&shadowTexDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(&m_shadowTexture)));

		NAME_D3D12_OBJECT(m_shadowTexture);

		// Get a handle to the start of the descriptor heap then offset 
		// it based on the frame resource index.
		const UINT dsvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		CD3DX12_CPU_DESCRIPTOR_HANDLE depthHandle(pDsvHeap->GetCPUDescriptorHandleForHeapStart(), 1 + frameIndex, dsvDescriptorSize); // + 1 for the shadow map.

		// Describe and create the shadow depth view and cache the CPU 
		// descriptor handle.
		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		pDevice->CreateDepthStencilView(m_shadowTexture.Get(), &depthStencilViewDesc, depthHandle);
		m_shadowDepthView = depthHandle;

		// Get a handle to the start of the descriptor heap then offset it 
		// based on the existing textures and the frame resource index. Each 
		// frame has 1 SRV (shadow tex) and 2 CBVs.
		const UINT nullSrvCount = 2;	// Null descriptors at the start of the heap.
		const UINT cbvSrvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CD3DX12_CPU_DESCRIPTOR_HANDLE cbvSrvCpuHandle(pCbvSrvHeap->GetCPUDescriptorHandleForHeapStart());
		CD3DX12_GPU_DESCRIPTOR_HANDLE cbvSrvGpuHandle(pCbvSrvHeap->GetGPUDescriptorHandleForHeapStart());

		m_nullSrvHandle0 = cbvSrvGpuHandle;
		cbvSrvCpuHandle.Offset(cbvSrvDescriptorSize);
		cbvSrvGpuHandle.Offset(cbvSrvDescriptorSize);
		m_nullSrvHandle1 = cbvSrvGpuHandle;

		UINT srvIndex = pBaseDev->GetSrvNextIndex();
		CD3DX12_CPU_DESCRIPTOR_HANDLE cbvSrvCpuHandle2(
			pBaseDev->GetCbvSrvUavDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
			srvIndex,
			pBaseDev->GetCbvSrvUavDescriptorHandleIncrementSize()
		);

		CD3DX12_GPU_DESCRIPTOR_HANDLE cbvSrvGpuHandle2(
			pBaseDev->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			srvIndex,
			pBaseDev->GetCbvSrvUavDescriptorHandleIncrementSize()
		);


		// Describe and create a shader resource view (SRV) for the shadow depth 
		// texture and cache the GPU descriptor handle. This SRV is for sampling 
		// the shadow map from our shader. It uses the same texture that we use 
		// as a depth-stencil during the shadow pass.
		D3D12_SHADER_RESOURCE_VIEW_DESC shadowSrvDesc = {};
		shadowSrvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		shadowSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		shadowSrvDesc.Texture2D.MipLevels = 1;
		shadowSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		pDevice->CreateShaderResourceView(m_shadowTexture.Get(), &shadowSrvDesc, cbvSrvCpuHandle2);
		m_shadowDepthHandle = cbvSrvGpuHandle2;

	}

	BaseFrame::~BaseFrame() {

		//更新用のコマンドリスト
		m_updateCommandAllocator = nullptr;
		m_updateCommandList = nullptr;
		//シーン描画用コマンドリスト
		m_sceneCommandAllocator = nullptr;
		m_sceneCommandList = nullptr;

		m_shadowTexture = nullptr;
	}


	void BaseFrame::Init()
	{
		auto pipelineState = BasicPipelineStatePool::GetPipelineState(L"initPipelineState");
		if (!pipelineState) {
			throw BaseException(
				L"初期化用のパイプラインステートが取得できません",
				L"BaseFrame::Init()"
			);
		}
		auto pipelineStateShadowMap = BasicPipelineStatePool::GetPipelineState(L"shadowMapPipelineState");
		if (!pipelineStateShadowMap) {
			throw BaseException(
				L"シャドウマップ用のパイプラインステートが取得できません",
				L"BaseFrame::Init()"
			);
		}

		// Reset the command allocators and lists for the main thread.
		for (int i = 0; i < BaseDevice::m_commandListCount; i++)
		{
			ThrowIfFailed(m_commandAllocators[i]->Reset());
			ThrowIfFailed(m_commandLists[i]->Reset(m_commandAllocators[i].Get(), pipelineState.Get()));
		}

		// Clear the depth stencil buffer in preparation for rendering the shadow map.
		m_commandLists[BaseDevice::m_commandListPre]->ClearDepthStencilView(m_shadowDepthView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		ThrowIfFailed(m_shadowCommandAllocator->Reset());
		ThrowIfFailed(m_shadowCommandList->Reset(m_shadowCommandAllocator.Get(), pipelineStateShadowMap.Get()));

		ThrowIfFailed(m_sceneCommandAllocator->Reset());
		ThrowIfFailed(m_sceneCommandList->Reset(m_sceneCommandAllocator.Get(), pipelineState.Get()));
	}

	void BaseFrame::SwapBarriers()
	{
		// Transition the shadow map from writeable to readable.
		m_commandLists[BaseDevice::m_commandListMid]->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_shadowTexture.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
	}

	void BaseFrame::Finish()
	{
		m_commandLists[BaseDevice::m_commandListPost]->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_shadowTexture.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE));
	}


	void BaseFrame::Bind(ID3D12GraphicsCommandList* pCommandList, BOOL scenePass, D3D12_CPU_DESCRIPTOR_HANDLE* pRtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE* pDsvHandle) {
		auto pBaseDev = App::GetBaseDevice();
		if (scenePass)
		{
			// Scene pass. We use constant buf #2 and depth stencil #2
			// with rendering to the render target enabled.
			pCommandList->SetGraphicsRootDescriptorTable(pBaseDev->GetGpuSlotID(L"t0"), m_shadowDepthHandle);        // Set the shadow texture as an SRV.
			pCommandList->OMSetRenderTargets(1, pRtvHandle, FALSE, pDsvHandle);
		}
		else
		{
			// Shadow pass. We use constant buf #1 and depth stencil #1
			// with rendering to the render target disabled.
			pCommandList->SetGraphicsRootDescriptorTable(pBaseDev->GetGpuSlotID(L"t0"), m_nullSrvHandle0);
			pCommandList->OMSetRenderTargets(0, nullptr, FALSE, &m_shadowDepthView);    // No render target needed for the shadow pass.
		}
	}


}
// end namespace basecross
