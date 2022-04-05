/*!
@file SpStaticRender.cpp
@brief Simpleスタティック描画コンポーネント群
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	Shadowmap描画コンポーネント
	//--------------------------------------------------------------------------------------
	ComPtr<ID3D12PipelineState> ShadowmapRender::m_defaultPipelineState(nullptr);

	void ShadowmapRender::CreatePipelineStates() {

	}

	//コンスタントバッファの登録
	void ShadowmapRender::SetConstant(ShadowConstant& constant, const shared_ptr<Transform>& transform) {

	}

	void ShadowmapRender::PopulateCommandList(BaseFrame* pBaseFrame) {

	}

	void ShadowmapRender::OnInitFrame(BaseFrame* pBaseFrame) {

	}

	void ShadowmapRender::WriteConstantBuffers(BaseFrame* pBaseFrame) {

	}

	ShadowmapRender::ShadowmapRender(const shared_ptr<GameObject>& gameObjectPtr):Component(gameObjectPtr)
	{

	}

	void ShadowmapRender::OnInit() {

	}


	//--------------------------------------------------------------------------------------
	///	SpStatic描画コンポーネント(親)
	//--------------------------------------------------------------------------------------

	void SpStaticRender::SetConstant(SimpleConstant& constant, const shared_ptr<Transform>& transform) {
		//すべて0に初期化
		constant = {};
		Mat4x4 world = transform->GetWorldMatrix();
		//転置する
		world.transpose();
		auto cameraPtr = GetGameObject()->GetActiveCamera();
		//ビューと射影行列を得る
		Mat4x4 viewMat = cameraPtr->GetViewMatrix();
		//転置する
		viewMat.transpose();
		Mat4x4 projMat = cameraPtr->GetProjectionMatrix();
		//転置する
		projMat.transpose();
		constant.world = world;
		constant.view = viewMat;
		constant.projection = projMat;
		//エミッシブ
		constant.emissive = GetEmissive();
		//デフィーズ
		constant.diffuse = GetDiffuse();
		//スペキュラー
		constant.specular = GetSpecular();
		//ライティング
		auto activeLights = GetGameObject()->GetActiveLightSet();
		auto index = activeLights->GetMainIndex();
		auto light = activeLights->GetLight(index);
		constant.lightDir = light.m_directional;
		constant.lightDir.w = 1.0f;
		constant.eyePos = cameraPtr->GetEye();
		constant.eyePos.w = 1.0f;
		//テクスチャがあった場合
		if (GetBaseTexture()) {
			constant.activeFlg.x = 1;
		}
	}

	void SpStaticRender::OnInitFrame(BaseFrame* pBaseFrame) {
		auto pDefaultDev = App::GetBaseDevice();
		auto pDevice = App::GetID3D12Device();

		// コンスタントバッファのサイズは256バイト境界ごとに作成する
		UINT constsize = (sizeof(SimpleConstant) + 255) & ~255;

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

	void SpStaticRender::WriteConstantBuffers(BaseFrame* pBaseFrame) {
		SimpleConstant constants;
		SetConstant(constants, GetGameObject()->GetComponent<Transform>());
		memcpy(pBaseFrame->m_frameParamVec[m_frameParamIndex].m_pConstantBuffer, &constants, sizeof(SimpleConstant));
	}


	//--------------------------------------------------------------------------------------
	///	SpPCStatic描画コンポーネント
	//--------------------------------------------------------------------------------------

	IMPLEMENT_DX12SHADER(SpVSPCStatic, App::GetShadersDir() + L"SpVSPCStatic.cso")
	IMPLEMENT_DX12SHADER(SpPSPCStatic, App::GetShadersDir() + L"SpPSPCStatic.cso")

	SpPCStaticRender::SpPCStaticRender(const shared_ptr<GameObject>& gameObjectPtr) :
		SpStaticRender(gameObjectPtr)
	{
		SetRenderActive(true);
	}

	SpPCStaticRender::SpPCStaticRender(const shared_ptr<GameObject>& gameObjectPtr,
		vector<VertexPositionColor>& vertices, vector<uint32_t>& indices) :
		SpStaticRender(gameObjectPtr)
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


	ComPtr<ID3D12PipelineState> SpPCStaticRender::m_defaultPipelineState(nullptr);
	ComPtr<ID3D12PipelineState> SpPCStaticRender::m_alphaPipelineState(nullptr);

	void SpPCStaticRender::CreatePipelineStates() {
		if ((!m_defaultPipelineState) || (!m_alphaPipelineState)) {
			auto pDevice = App::GetBaseDevice();
			CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
			//カリング
			rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			ZeroMemory(&psoDesc, sizeof(psoDesc));
			psoDesc.InputLayout = { VertexPositionColor::GetVertexElement(), VertexPositionColor::GetNumElements() };
			psoDesc.pRootSignature = pDevice->GetRootSignature().Get();
			psoDesc.VS =
			{
				reinterpret_cast<UINT8*>(SpVSPCStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				SpVSPCStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
			};
			psoDesc.PS =
			{
				reinterpret_cast<UINT8*>(SpPSPCStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				SpPSPCStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
			};
			psoDesc.RasterizerState = rasterizerStateDesc;
			psoDesc.BlendState = BaseRenderState::GetOpaqueBlend();
			psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
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
	}


	void SpPCStaticRender::OnInit() {
		CreatePipelineStates();
	}


	void SpPCStaticRender::PopulateCommandList(BaseFrame* pBaseFrame) {
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
		CD3DX12_GPU_DESCRIPTOR_HANDLE skyCbvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_frameParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), skyCbvHandle);

		if (!m_baseMesh) {
			throw BaseException(
				L"メッシュが特定できません。",
				L"SpPCStaticRender::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);
	}


	//--------------------------------------------------------------------------------------
	///	SpPTStatic描画コンポーネント
	//--------------------------------------------------------------------------------------

	IMPLEMENT_DX12SHADER(SpVSPTStatic, App::GetShadersDir() + L"SpVSPTStatic.cso")
	IMPLEMENT_DX12SHADER(SpPSPTStatic, App::GetShadersDir() + L"SpPSPTStatic.cso")

	SpPTStaticRender::SpPTStaticRender(const shared_ptr<GameObject>& gameObjectPtr) :
		SpStaticRender(gameObjectPtr)
	{
		SetRenderActive(true);
	}

	SpPTStaticRender::SpPTStaticRender(const shared_ptr<GameObject>& gameObjectPtr,
		vector<VertexPositionTexture>& vertices, vector<uint32_t>& indices) :
		SpStaticRender(gameObjectPtr)
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


	ComPtr<ID3D12PipelineState> SpPTStaticRender::m_defaultPipelineState(nullptr);
	ComPtr<ID3D12PipelineState> SpPTStaticRender::m_alphaPipelineState(nullptr);

	void SpPTStaticRender::CreatePipelineStates() {
		if ((!m_defaultPipelineState) || (!m_alphaPipelineState)) {
			auto pDevice = App::GetBaseDevice();
			CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
			//カリング
			rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			ZeroMemory(&psoDesc, sizeof(psoDesc));
			psoDesc.InputLayout = { VertexPositionTexture::GetVertexElement(), VertexPositionTexture::GetNumElements() };
			psoDesc.pRootSignature = pDevice->GetRootSignature().Get();
			psoDesc.VS =
			{
				reinterpret_cast<UINT8*>(SpVSPTStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				SpVSPTStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
			};
			psoDesc.PS =
			{
				reinterpret_cast<UINT8*>(SpPSPTStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				SpPSPTStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
			};
			psoDesc.RasterizerState = rasterizerStateDesc;
			psoDesc.BlendState = BaseRenderState::GetOpaqueBlend();
			psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
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
	}


	void SpPTStaticRender::OnInit() {
		CreatePipelineStates();
	}


	void SpPTStaticRender::PopulateCommandList(BaseFrame* pBaseFrame) {
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
		if (m_baseTexture) {
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
					L"SpPTStaticRender::PopulateCommandList()"
				);
			}
			CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle(
				pDevice->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				index,
				pDevice->GetSamplerDescriptorHandleIncrementSize()
			);
			pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"s0"), samplerHandle);
		}
		//Cbv
		auto paramIndex = GetFrameParamIndex();
		CD3DX12_GPU_DESCRIPTOR_HANDLE skyCbvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_frameParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), skyCbvHandle);

		if (!m_baseMesh) {
			throw BaseException(
				L"メッシュが特定できません。",
				L"SpPTStaticRender::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);
	}


	//--------------------------------------------------------------------------------------
	///	SpPNStatic描画コンポーネント
	//--------------------------------------------------------------------------------------

	IMPLEMENT_DX12SHADER(SpVSPNStatic, App::GetShadersDir() + L"SpVSPNStatic.cso")
	IMPLEMENT_DX12SHADER(SpPSPNStatic, App::GetShadersDir() + L"SpPSPNStatic.cso")

	SpPNStaticRender::SpPNStaticRender(const shared_ptr<GameObject>& gameObjectPtr) :
		SpStaticRender(gameObjectPtr)
	{
		SetRenderActive(true);
	}

	SpPNStaticRender::SpPNStaticRender(const shared_ptr<GameObject>& gameObjectPtr,
		vector<VertexPositionNormal>& vertices, vector<uint32_t>& indices) :
		SpStaticRender(gameObjectPtr)
	{
		try {
			SetRenderActive(true);
			auto pDevice = App::GetBaseDevice();
			auto commandList = pDevice->GetComandList();
			m_baseMesh = BaseMesh::CreateBaseMesh<VertexPositionNormal>(commandList, vertices, indices);
		}
		catch (...) {
			throw;
		}
	}


	ComPtr<ID3D12PipelineState> SpPNStaticRender::m_defaultPipelineState(nullptr);
	ComPtr<ID3D12PipelineState> SpPNStaticRender::m_alphaPipelineState(nullptr);

	void SpPNStaticRender::CreatePipelineStates() {
		if ((!m_defaultPipelineState) || (!m_alphaPipelineState)) {
			auto pDevice = App::GetBaseDevice();
			CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
			//カリング
			rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			ZeroMemory(&psoDesc, sizeof(psoDesc));
			psoDesc.InputLayout = { VertexPositionNormal::GetVertexElement(), VertexPositionNormal::GetNumElements() };
			psoDesc.pRootSignature = pDevice->GetRootSignature().Get();
			psoDesc.VS =
			{
				reinterpret_cast<UINT8*>(SpVSPNStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				SpVSPNStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
			};
			psoDesc.PS =
			{
				reinterpret_cast<UINT8*>(SpPSPNStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				SpPSPNStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
			};
			psoDesc.RasterizerState = rasterizerStateDesc;
			psoDesc.BlendState = BaseRenderState::GetOpaqueBlend();
			psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
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
	}


	void SpPNStaticRender::OnInit() {
		CreatePipelineStates();
	}


	void SpPNStaticRender::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();
		auto pCommandList = pDevice->GetComandList().Get();

		if (GetGameObject()->IsAlphaActive()) {
			pCommandList->SetPipelineState(m_alphaPipelineState.Get());
		}
		else {
			pCommandList->SetPipelineState(m_defaultPipelineState.Get());
		}
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//Cbv
		auto paramIndex = GetFrameParamIndex();
		CD3DX12_GPU_DESCRIPTOR_HANDLE skyCbvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_frameParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), skyCbvHandle);

		if (!m_baseMesh) {
			throw BaseException(
				L"メッシュが特定できません。",
				L"SpPNStaticRender::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);
	}

	//--------------------------------------------------------------------------------------
	///	SpPCTStatic描画コンポーネント
	//--------------------------------------------------------------------------------------

	IMPLEMENT_DX12SHADER(SpVSPCTStatic, App::GetShadersDir() + L"SpVSPCTStatic.cso")
	IMPLEMENT_DX12SHADER(SpPSPCTStatic, App::GetShadersDir() + L"SpPSPCTStatic.cso")

	SpPCTStaticRender::SpPCTStaticRender(const shared_ptr<GameObject>& gameObjectPtr) :
		SpStaticRender(gameObjectPtr)
	{
		SetRenderActive(true);
	}

	SpPCTStaticRender::SpPCTStaticRender(const shared_ptr<GameObject>& gameObjectPtr,
		vector<VertexPositionColorTexture>& vertices, vector<uint32_t>& indices) :
		SpStaticRender(gameObjectPtr)
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


	ComPtr<ID3D12PipelineState> SpPCTStaticRender::m_defaultPipelineState(nullptr);
	ComPtr<ID3D12PipelineState> SpPCTStaticRender::m_alphaPipelineState(nullptr);

	void SpPCTStaticRender::CreatePipelineStates() {
		if ((!m_defaultPipelineState) || (!m_alphaPipelineState)) {
			auto pDevice = App::GetBaseDevice();
			CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
			//カリング
			rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			ZeroMemory(&psoDesc, sizeof(psoDesc));
			psoDesc.InputLayout = { VertexPositionColorTexture::GetVertexElement(), VertexPositionColorTexture::GetNumElements() };
			psoDesc.pRootSignature = pDevice->GetRootSignature().Get();
			psoDesc.VS =
			{
				reinterpret_cast<UINT8*>(SpVSPCTStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				SpVSPCTStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
			};
			psoDesc.PS =
			{
				reinterpret_cast<UINT8*>(SpPSPCTStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				SpPSPCTStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
			};
			psoDesc.RasterizerState = rasterizerStateDesc;
			psoDesc.BlendState = BaseRenderState::GetOpaqueBlend();
			psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
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
	}


	void SpPCTStaticRender::OnInit() {
		CreatePipelineStates();
	}


	void SpPCTStaticRender::PopulateCommandList(BaseFrame* pBaseFrame) {
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
		if (m_baseTexture) {
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
					L"SpPCTStaticRender::PopulateCommandList()"
				);
			}
			CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle(
				pDevice->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				index,
				pDevice->GetSamplerDescriptorHandleIncrementSize()
			);
			pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"s0"), samplerHandle);
		}
		//Cbv
		auto paramIndex = GetFrameParamIndex();
		CD3DX12_GPU_DESCRIPTOR_HANDLE skyCbvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_frameParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), skyCbvHandle);

		if (!m_baseMesh) {
			throw BaseException(
				L"メッシュが特定できません。",
				L"SpPCTStaticRender::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);
	}

	//--------------------------------------------------------------------------------------
	///	SpPNTStatic描画コンポーネント
	//--------------------------------------------------------------------------------------

	IMPLEMENT_DX12SHADER(SpVSPNTStatic, App::GetShadersDir() + L"SpVSPNTStatic.cso")
	IMPLEMENT_DX12SHADER(SpPSPNTStatic, App::GetShadersDir() + L"SpPSPNTStatic.cso")

	SpPNTStaticRender::SpPNTStaticRender(const shared_ptr<GameObject>& gameObjectPtr) :
		SpStaticRender(gameObjectPtr)
	{
		SetRenderActive(true);
	}

	SpPNTStaticRender::SpPNTStaticRender(const shared_ptr<GameObject>& gameObjectPtr,
		vector<VertexPositionNormalTexture>& vertices, vector<uint32_t>& indices) :
		SpStaticRender(gameObjectPtr)
	{
		try {
			SetRenderActive(true);
			auto pDevice = App::GetBaseDevice();
			auto commandList = pDevice->GetComandList();
			m_baseMesh = BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(commandList, vertices, indices);
		}
		catch (...) {
			throw;
		}
	}


	ComPtr<ID3D12PipelineState> SpPNTStaticRender::m_defaultPipelineState(nullptr);
	ComPtr<ID3D12PipelineState> SpPNTStaticRender::m_alphaPipelineState(nullptr);

	void SpPNTStaticRender::CreatePipelineStates() {
		if ((!m_defaultPipelineState) || (!m_alphaPipelineState)) {
			auto pDevice = App::GetBaseDevice();
			CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
			//カリング
			rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			ZeroMemory(&psoDesc, sizeof(psoDesc));
			psoDesc.InputLayout = { VertexPositionNormalTexture::GetVertexElement(), VertexPositionNormalTexture::GetNumElements() };
			psoDesc.pRootSignature = pDevice->GetRootSignature().Get();
			psoDesc.VS =
			{
				reinterpret_cast<UINT8*>(SpVSPNTStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				SpVSPNTStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
			};
			psoDesc.PS =
			{
				reinterpret_cast<UINT8*>(SpPSPNTStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				SpPSPNTStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
			};
			psoDesc.RasterizerState = rasterizerStateDesc;
			psoDesc.BlendState = BaseRenderState::GetOpaqueBlend();
			psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
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
	}


	void SpPNTStaticRender::OnInit() {
		CreatePipelineStates();
	}


	void SpPNTStaticRender::PopulateCommandList(BaseFrame* pBaseFrame) {
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
		if (m_baseTexture) {
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
					L"SpPNTStaticRender::PopulateCommandList()"
				);
			}
			CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle(
				pDevice->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				index,
				pDevice->GetSamplerDescriptorHandleIncrementSize()
			);
			pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"s0"), samplerHandle);
		}
		//Cbv
		auto paramIndex = GetFrameParamIndex();
		CD3DX12_GPU_DESCRIPTOR_HANDLE skyCbvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_frameParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), skyCbvHandle);

		if (!m_baseMesh) {
			throw BaseException(
				L"メッシュが特定できません。",
				L"SpPNTStaticRender::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);
	}




}
//end namespace basecross
