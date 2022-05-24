/*!
@file SpSpriteRender.cpp
@brief Simpleスプライト描画コンポーネント群
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	IMPLEMENT_DX12SHADER(SpVSPCSprite, App::GetShadersDir() + L"SpVSPCSprite.cso")
	IMPLEMENT_DX12SHADER(SpPSPCSprite, App::GetShadersDir() + L"SpPSPCSprite.cso")

	IMPLEMENT_DX12SHADER(SpVSPTSprite, App::GetShadersDir() + L"SpVSPTSprite.cso")
	IMPLEMENT_DX12SHADER(SpPSPTSprite, App::GetShadersDir() + L"SpPSPTSprite.cso")

	IMPLEMENT_DX12SHADER(SpVSPCTSprite, App::GetShadersDir() + L"SpVSPCTSprite.cso")
	IMPLEMENT_DX12SHADER(SpPSPCTSprite, App::GetShadersDir() + L"SpPSPCTSprite.cso")


	//--------------------------------------------------------------------------------------
	///	SpSprite描画コンポーネント
	//--------------------------------------------------------------------------------------
	SpSpriteRender::SpSpriteRender(const shared_ptr<GameObject>& gameObjectPtr) :
		Component(gameObjectPtr),
		m_emissive(0.0f),
		m_diffuse(1.0f)
	{
		SetRenderActive(true);
	}

	void SpSpriteRender::SetConstant(SpriteConstant& constant, const shared_ptr<Transform>& transform) {
		//行列の取得
		auto PtrTrans = GetGameObject()->GetComponent<Transform>();
		//行列の定義
		bsm::Mat4x4 World, Proj;
		World = PtrTrans->Get2DWorldMatrix();
		//射影行列の決定
		Proj = GetGameObject()->Get2DDrawProjMatrix();
		//行列の合成
		World *= Proj;
		//エミッシブ
		constant.emissive = GetEmissive();
		//デフィーズ
		constant.diffuse = GetDiffuse();
		//行列の設定
		constant.world = World;
	}

	void SpSpriteRender::WriteConstantBuffers(BaseFrame* pBaseFrame) {
		SpriteConstant constant;
		SetConstant(constant, GetGameObject()->GetComponent<Transform>());
		memcpy(pBaseFrame->m_frameParamVec[m_frameParamIndex].m_pConstantBuffer, &constant, sizeof(SpriteConstant));
	}

	void SpSpriteRender::OnInitFrame(BaseFrame* pBaseFrame) {
		auto pDefaultDev = App::GetBaseDevice();
		auto pDevice = App::GetID3D12Device();

		// コンスタントバッファのサイズは256バイト境界ごとに作成する
		UINT constsize = (sizeof(SpriteConstant) + 255) & ~255;

		FrameParam param;
		// アップロードヒープ
		ThrowIfFailed(pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(constsize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&param.m_cbvUploadHeap)));
		//マップ
		CD3DX12_RANGE readRange(0, 0);
		ThrowIfFailed(param.m_cbvUploadHeap->Map(0, &readRange, reinterpret_cast<void**>(&param.m_pConstantBuffer)));
		//ハンドルの作成
		param.m_constBuffIndex = pDefaultDev->GetCbvUavNextIndex();
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle(
			pDefaultDev->GetCbvSrvUavDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
			param.m_constBuffIndex,
			pDefaultDev->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		//コンスタントバッファビューの作成
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = param.m_cbvUploadHeap->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = constsize;
		pDevice->CreateConstantBufferView(&cbvDesc, handle);
		m_frameParamIndex = pBaseFrame->m_frameParamVec.size();
		pBaseFrame->m_frameParamVec.push_back(param);

	}



	//--------------------------------------------------------------------------------------
	///	SpPCSprite描画コンポーネント
	//--------------------------------------------------------------------------------------
	SpPCSpriteRender::SpPCSpriteRender(const shared_ptr<GameObject>& gameObjectPtr) :
		SpSpriteRender(gameObjectPtr)
	{
	}

	SpPCSpriteRender::SpPCSpriteRender(const shared_ptr<GameObject>& gameObjectPtr,
		vector<VertexPositionColor>& vertices, vector<uint32_t>& indices) :
		SpSpriteRender(gameObjectPtr)
	{
		try {
			SetRenderActive(true);
			auto pDevice = App::GetBaseDevice();
			auto commandList = pDevice->GetComandList();
			m_baseMesh = BaseMesh::CreateBaseMesh<VertexPositionColor>(commandList, vertices, indices);
		}
		catch (...) {
			throw;
		}
	}


	void SpPCSpriteRender::CreatePipelineStates() {
		auto pDevice = App::GetBaseDevice();
		//ブレンド
		CD3DX12_BLEND_DESC blendDesk(D3D12_DEFAULT);
		//ラスタライザ
		CD3DX12_RASTERIZER_DESC rasterizerDesc(D3D12_DEFAULT);
		//デプスステンシル
		CD3DX12_DEPTH_STENCIL_DESC depthStencilDesk(D3D12_DEFAULT);
		depthStencilDesk.DepthEnable = FALSE;
		depthStencilDesk.StencilEnable = FALSE;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		ZeroMemory(&psoDesc, sizeof(psoDesc));
		psoDesc.InputLayout = { VertexPositionColor::GetVertexElement(), VertexPositionColor::GetNumElements() };
		psoDesc.pRootSignature = pDevice->GetRootSignature().Get();
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(SpVSPCSprite::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpVSPCSprite::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<UINT8*>(SpPSPCSprite::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpPSPCSprite::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.RasterizerState = rasterizerDesc;
		psoDesc.BlendState = BaseRenderState::GetOpaqueBlend();
		psoDesc.DepthStencilState = depthStencilDesk;
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_defaultPipelineState)));
		NAME_D3D12_OBJECT(m_defaultPipelineState);

		psoDesc.BlendState = BaseRenderState::GetAlphaBlend();
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_alphaPipelineState)));
		NAME_D3D12_OBJECT(m_alphaPipelineState);

	}

	void SpPCSpriteRender::OnCreate() {
		CreatePipelineStates();
	}

	void SpPCSpriteRender::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();
		auto pCommandList = pDevice->GetComandList().Get();
		if (GetGameObject()->IsAlphaActive()) {
			pCommandList->SetPipelineState(m_alphaPipelineState.Get());
		}
		else {
			pCommandList->SetPipelineState(m_defaultPipelineState.Get());
		}

		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		auto paramIndex = GetFrameParamIndex();
		//Cbv
		CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_frameParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), cbvHandle);

		if (!m_baseMesh) {
			throw BaseException(
				L"メッシュが特定できません。",
				L"SpPCSpriteRender::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);
	}

	//--------------------------------------------------------------------------------------
	///	SpPTSprite描画コンポーネント
	//--------------------------------------------------------------------------------------
	SpPTSpriteRender::SpPTSpriteRender(const shared_ptr<GameObject>& gameObjectPtr) :
		SpSpriteRender(gameObjectPtr)
	{
	}

	SpPTSpriteRender::SpPTSpriteRender(const shared_ptr<GameObject>& gameObjectPtr,
		vector<VertexPositionTexture>& vertices, vector<uint32_t>& indices) :
		SpSpriteRender(gameObjectPtr)
	{
		try {
			SetRenderActive(true);
			auto pDevice = App::GetBaseDevice();
			auto commandList = pDevice->GetComandList();
			m_baseMesh = BaseMesh::CreateBaseMesh<VertexPositionTexture>(commandList, vertices, indices);
		}
		catch (...) {
			throw;
		}
	}


	void SpPTSpriteRender::CreatePipelineStates() {
		auto pDevice = App::GetBaseDevice();
		//ブレンド
		CD3DX12_BLEND_DESC blendDesk(D3D12_DEFAULT);
		//ラスタライザ
		CD3DX12_RASTERIZER_DESC rasterizerDesc(D3D12_DEFAULT);
		//デプスステンシル
		CD3DX12_DEPTH_STENCIL_DESC depthStencilDesk(D3D12_DEFAULT);
		depthStencilDesk.DepthEnable = FALSE;
		depthStencilDesk.StencilEnable = FALSE;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		ZeroMemory(&psoDesc, sizeof(psoDesc));
		psoDesc.InputLayout = { VertexPositionTexture::GetVertexElement(), VertexPositionTexture::GetNumElements() };
		psoDesc.pRootSignature = pDevice->GetRootSignature().Get();
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(SpVSPTSprite::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpVSPTSprite::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<UINT8*>(SpPSPTSprite::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpPSPTSprite::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.RasterizerState = rasterizerDesc;
		psoDesc.BlendState = BaseRenderState::GetOpaqueBlend();
		psoDesc.DepthStencilState = depthStencilDesk;
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_defaultPipelineState)));
		NAME_D3D12_OBJECT(m_defaultPipelineState);

		psoDesc.BlendState = BaseRenderState::GetAlphaBlend();
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_alphaPipelineState)));
		NAME_D3D12_OBJECT(m_alphaPipelineState);

	}

	void SpPTSpriteRender::OnCreate() {
		CreatePipelineStates();
	}

	void SpPTSpriteRender::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();
		auto pCommandList = pDevice->GetComandList().Get();
		if (GetGameObject()->IsAlphaActive()) {
			pCommandList->SetPipelineState(m_alphaPipelineState.Get());
		}
		else {
			pCommandList->SetPipelineState(m_defaultPipelineState.Get());
		}
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//Srv
		if (!m_baseTexture) {
			throw BaseException(
				L"テクスチャが特定できません。",
				L"SpPTSpriteRender::PopulateCommandList()"
			);
		}
		CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			m_baseTexture->GetSrvIndex(),
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"t1"), srvHandle);
		//Sampler
		UINT index = pDevice->GetSamplerIndex(GetSamplerKey());
		if (index == UINT_MAX) {
			throw BaseException(
				L"サンプラーが特定できません。",
				L"SpPTSpriteRender::PopulateCommandList()"
			);
		}
		CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle(
			pDevice->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			index,
			pDevice->GetSamplerDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"s0"), samplerHandle);
		//Cbv
		auto paramIndex = GetFrameParamIndex();
		CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_frameParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), cbvHandle);
		if (!m_baseMesh) {
			throw BaseException(
				L"メッシュが特定できません。",
				L"SpPTSpriteRender::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);
	}

	//--------------------------------------------------------------------------------------
	///	SpPCTSprite描画コンポーネント
	//--------------------------------------------------------------------------------------
	SpPCTSpriteRender::SpPCTSpriteRender(const shared_ptr<GameObject>& gameObjectPtr) :
		SpSpriteRender(gameObjectPtr)
	{
	}

	SpPCTSpriteRender::SpPCTSpriteRender(const shared_ptr<GameObject>& gameObjectPtr,
		vector<VertexPositionColorTexture>& vertices, vector<uint32_t>& indices) :
		SpSpriteRender(gameObjectPtr)
	{
		try {
			SetRenderActive(true);
			auto pDevice = App::GetBaseDevice();
			auto commandList = pDevice->GetComandList();
			m_baseMesh = BaseMesh::CreateBaseMesh<VertexPositionColorTexture>(commandList, vertices, indices);
		}
		catch (...) {
			throw;
		}
	}


	void SpPCTSpriteRender::CreatePipelineStates() {
		auto pDevice = App::GetBaseDevice();
		//ブレンド
		CD3DX12_BLEND_DESC blendDesk(D3D12_DEFAULT);
		//ラスタライザ
		CD3DX12_RASTERIZER_DESC rasterizerDesc(D3D12_DEFAULT);
		//デプスステンシル
		CD3DX12_DEPTH_STENCIL_DESC depthStencilDesk(D3D12_DEFAULT);
		depthStencilDesk.DepthEnable = FALSE;
		depthStencilDesk.StencilEnable = FALSE;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		ZeroMemory(&psoDesc, sizeof(psoDesc));
		psoDesc.InputLayout = { VertexPositionColorTexture::GetVertexElement(), VertexPositionColorTexture::GetNumElements() };
		psoDesc.pRootSignature = pDevice->GetRootSignature().Get();
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(SpVSPCTSprite::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpVSPCTSprite::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<UINT8*>(SpPSPCTSprite::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpPSPCTSprite::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.RasterizerState = rasterizerDesc;
		psoDesc.BlendState = BaseRenderState::GetOpaqueBlend();
		psoDesc.DepthStencilState = depthStencilDesk;
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_defaultPipelineState)));
		NAME_D3D12_OBJECT(m_defaultPipelineState);

		psoDesc.BlendState = BaseRenderState::GetAlphaBlend();
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_alphaPipelineState)));
		NAME_D3D12_OBJECT(m_alphaPipelineState);

	}

	void SpPCTSpriteRender::OnCreate() {
		CreatePipelineStates();
	}

	void SpPCTSpriteRender::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();
		auto pCommandList = pDevice->GetComandList().Get();
		if (GetGameObject()->IsAlphaActive()) {
			pCommandList->SetPipelineState(m_alphaPipelineState.Get());
		}
		else {
			pCommandList->SetPipelineState(m_defaultPipelineState.Get());
		}
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//Srv
		if (!m_baseTexture) {
			throw BaseException(
				L"テクスチャが特定できません。",
				L"SpPCTSpriteRender::PopulateCommandList()"
			);
		}
		CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			m_baseTexture->GetSrvIndex(),
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"t1"), srvHandle);
		//Sampler
		UINT index = pDevice->GetSamplerIndex(GetSamplerKey());
		if (index == UINT_MAX) {
			throw BaseException(
				L"サンプラーが特定できません。",
				L"SpPCTSpriteRender::PopulateCommandList()"
			);
		}
		CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle(
			pDevice->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			index,
			pDevice->GetSamplerDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"s0"), samplerHandle);
		//Cbv
		auto paramIndex = GetFrameParamIndex();
		CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_frameParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), cbvHandle);
		if (!m_baseMesh) {
			throw BaseException(
				L"メッシュが特定できません。",
				L"SpPCTSpriteRender::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);
	}


}
// end namespace basecross
