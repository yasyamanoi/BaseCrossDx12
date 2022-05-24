/*!
@file BaseFrame.cpp
@brief フレームクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	BaseFrame::BaseFrame(ID3D12PipelineState* pPso, ID3D12PipelineState* pShadowPso, D3D12_VIEWPORT* pViewport, UINT frameResourceIndex) :
		m_pipelineState(pPso),
		m_pipelineStateShadow(pShadowPso),
		m_fenceValue(0)
	{
		auto pDevice = App::GetID3D12Device();
		auto pDefaultDev = App::GetBaseDevice();
		//update用のコマンドリスト
		ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_updateCommandAllocator)));
		ThrowIfFailed(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_updateCommandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_updateCommandList)));
		ThrowIfFailed(m_updateCommandList->Close());

		//begin用のコマンドリスト
		ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_beginCommandAllocator)));
		ThrowIfFailed(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_beginCommandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_beginCommandList)));
		ThrowIfFailed(m_beginCommandList->Close());

		//Shadowmap用のコマンドリスト
		ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_shadowCommandAllocator)));
		ThrowIfFailed(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_shadowCommandAllocator.Get(), m_pipelineStateShadow.Get(), IID_PPV_ARGS(&m_shadowCommandList)));
		ThrowIfFailed(m_shadowCommandList->Close());


		//Render用のコマンドリスト
		ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_sceneCommandAllocator)));
		ThrowIfFailed(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_sceneCommandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_sceneCommandList)));
		ThrowIfFailed(m_sceneCommandList->Close());

		//end用のコマンドリスト
		ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_endCommandAllocator)));
		ThrowIfFailed(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_endCommandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_endCommandList)));
		ThrowIfFailed(m_endCommandList->Close());

		// シャドウマップ用のテクスチャ
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

		D3D12_CLEAR_VALUE clearValue;
		clearValue.Format = DXGI_FORMAT_D32_FLOAT;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;

		ThrowIfFailed(pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&shadowTexDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE, //D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(&m_shadowTexture)));

		NAME_D3D12_OBJECT(m_shadowTexture);

		const UINT dsvDescriptorSize = pDefaultDev->GetDsvDescriptorHandleIncrementSize();
		CD3DX12_CPU_DESCRIPTOR_HANDLE depthHandle(
			pDefaultDev->GetDsvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
			1 + frameResourceIndex, //シャドウマップのぶん、1をプラス
			dsvDescriptorSize 
		); 

		// シャドウマップ用のデプスビュー 
		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		pDevice->CreateDepthStencilView(m_shadowTexture.Get(), &depthStencilViewDesc, depthHandle);
		m_shadowDepthView = depthHandle;

		//ディスクプリタのハンドルを作成
		UINT srvIndex = pDefaultDev->GetSrvNextIndex();
		//CPU側
		CD3DX12_CPU_DESCRIPTOR_HANDLE cbvSrvCpuHandle(
			pDefaultDev->GetCbvSrvUavDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
			srvIndex,
			pDefaultDev->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		//GPU側
		CD3DX12_GPU_DESCRIPTOR_HANDLE cbvSrvGpuHandle(
			pDefaultDev->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			srvIndex,
			pDefaultDev->GetCbvSrvUavDescriptorHandleIncrementSize()
		);

		D3D12_SHADER_RESOURCE_VIEW_DESC shadowSrvDesc = {};
		shadowSrvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		shadowSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		shadowSrvDesc.Texture2D.MipLevels = 1;
		shadowSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		pDevice->CreateShaderResourceView(m_shadowTexture.Get(), &shadowSrvDesc, cbvSrvCpuHandle);

		//shadowテクスチャのシェーダリソースビューを作成
//		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		////フォーマット
//		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
//		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
//		srvDesc.Texture2D.MipLevels = 1;
//		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		//シェーダリソースビュー
//		pDevice->CreateShaderResourceView(
//			m_shadowTexture.Get(),
//			&srvDesc,
//			cbvSrvCpuHandle);

		//GPU側はメンバ変数としてとっておく
		m_shadowDepthHandle = cbvSrvGpuHandle;
		//nullのハンドル
		m_nullSrvHandle = pDefaultDev->GetNullSrvGpuHandle();

	}

	BaseFrame::~BaseFrame() {

		m_updateCommandAllocator = nullptr;
		m_updateCommandList = nullptr;

		m_beginCommandAllocator = nullptr;
		m_beginCommandList = nullptr;

		m_shadowCommandAllocator = nullptr;
		m_shadowCommandList = nullptr;

		m_sceneCommandAllocator = nullptr;
		m_sceneCommandList = nullptr;

		m_endCommandAllocator = nullptr;
		m_endCommandList = nullptr;

		m_shadowTexture = nullptr;

		for (auto v : m_frameParamVec) {
			v.m_cbvUploadHeap->Unmap(0, nullptr);
			v.m_pConstantBuffer = nullptr;
		}
	}

}
// end namespace basecross
