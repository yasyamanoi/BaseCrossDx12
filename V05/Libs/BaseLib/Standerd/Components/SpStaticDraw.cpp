/*!
@file SpStaticDraw.cpp
@brief Simpleスタティック描画コンポーネント群
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	SpStatic描画コンポーネント(親)
	//--------------------------------------------------------------------------------------

	void SpStaticDraw::SetConstant(SimpleConstant& constant, const shared_ptr<Transform>& transform) {
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
		Mat4x4 projMat = cameraPtr->GetProjMatrix();
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
		if (GetTexture()) {
			constant.activeFlg.x = 1;
		}
		//影用
		if (IsOwnShadowActive()) {
			bsm::Vec3 CalcLightDir = -1.0 * light.m_directional;
			bsm::Vec3 LightAt = cameraPtr->GetAt();
			bsm::Vec3 LightEye = CalcLightDir;
			LightEye *= Shadowmap::GetLightHeight();
			LightEye = LightAt + LightEye;
			constant.lightPos = LightEye;
			constant.lightPos.w = 1.0f;
			bsm::Mat4x4 lightView, lightProj;
			//ライトのビューと射影を計算
			lightView = XMMatrixLookAtLH(LightEye, LightAt, bsm::Vec3(0, 1.0f, 0));
			lightProj = XMMatrixOrthographicLH(Shadowmap::GetViewWidth(), Shadowmap::GetViewHeight(),
				Shadowmap::GetLightNear(), Shadowmap::GetLightFar());
			constant.lightView = bsm::transpose(lightView);
			constant.lightProjection = bsm::transpose(lightProj);
		}
	}

	void SpStaticDraw::OnInitFrame(BaseFrame* pBaseFrame) {
		auto pDefaultDev = App::GetBaseDevice();
		auto pDevice = App::GetID3D12Device();

		// コンスタントバッファのサイズは256バイト境界ごとに作成する
		UINT constsize = (sizeof(SimpleConstant) + 255) & ~255;

		ConstBuffParam param;
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
		m_constBuffParamIndices[pBaseFrame->m_frameIndex] = pBaseFrame->m_constBuffParamVec.size();
		pBaseFrame->m_constBuffParamVec.push_back(param);

	}

	void SpStaticDraw::WriteConstantBuffers(BaseFrame* pBaseFrame) {
		SimpleConstant constants;
		SetConstant(constants, GetGameObject()->GetComponent<Transform>());
		memcpy(pBaseFrame->m_constBuffParamVec[m_constBuffParamIndices[pBaseFrame->m_frameIndex]].m_pConstantBuffer, &constants, sizeof(SimpleConstant));
	}


	//--------------------------------------------------------------------------------------
	///	SpPCStatic描画コンポーネント
	//--------------------------------------------------------------------------------------

	IMPLEMENT_DX12SHADER(SpVSPCStatic, App::GetShadersDir() + L"SpVSPCStatic.cso")
	IMPLEMENT_DX12SHADER(SpPSPCStatic, App::GetShadersDir() + L"SpPSPCStatic.cso")

	SpPCStaticDraw::SpPCStaticDraw(const shared_ptr<GameObject>& gameObjectPtr) :
		SpStaticDraw(gameObjectPtr)
	{
		SetDrawActive(true);
	}

	SpPCStaticDraw::SpPCStaticDraw(const shared_ptr<GameObject>& gameObjectPtr,
		vector<VertexPositionColor>& vertices, vector<uint32_t>& indices) :
		SpStaticDraw(gameObjectPtr)
	{
		try {
			SetDrawActive(true);
			auto pDevice = App::GetBaseDevice();
			auto commandList = pDevice->GetComandList();
			m_baseMesh = BaseMesh::CreateBaseMesh<VertexPositionColor>(commandList, vertices, indices);
		}
		catch (...) {
			throw;
		}
	}



	void SpPCStaticDraw::CreatePipelineStates() {
		auto pDevice = App::GetBaseDevice();

		ComPtr<ID3D12PipelineState> defaultPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPCStaticDefault");
		ComPtr<ID3D12PipelineState> alphaPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPCStaticAlpha");


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
		psoDesc.BlendState = BaseBlendState::GetOpaqueBlend();
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		if (!defaultPipelineState) {
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&defaultPipelineState)));
			NAME_D3D12_OBJECT(defaultPipelineState);
			BasicPipelineStatePool::AddPipelineState(L"SpPCStaticDefault", defaultPipelineState);
		}
		psoDesc.BlendState = BaseBlendState::GetAlphaBlend();
		if (!alphaPipelineState) {
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&alphaPipelineState)));
			NAME_D3D12_OBJECT(alphaPipelineState);
			BasicPipelineStatePool::AddPipelineState(L"SpPCStaticAlpha", alphaPipelineState);

		}

	}


	void SpPCStaticDraw::OnCreate() {
		CreatePipelineStates();
	}


	void SpPCStaticDraw::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();

		ComPtr<ID3D12PipelineState> defaultPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPCStaticDefault",true);
		ComPtr<ID3D12PipelineState> alphaPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPCStaticAlpha",true);

		auto pCommandList = pDevice->GetComandList().Get();

		if (GetGameObject()->IsAlphaActive()) {
			pCommandList->SetPipelineState(alphaPipelineState.Get());
		}
		else {
			pCommandList->SetPipelineState(defaultPipelineState.Get());
		}

		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		auto paramIndex = m_constBuffParamIndices[pBaseFrame->m_frameIndex];
		//Cbv
		CD3DX12_GPU_DESCRIPTOR_HANDLE cbvGpuHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_constBuffParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), cbvGpuHandle);

		if (!m_baseMesh) {
			throw BaseException(
				L"メッシュが特定できません。",
				L"SpPCStaticDraw::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);
	}


	//--------------------------------------------------------------------------------------
	///	SpWireframeDraw描画コンポーネント(ワイヤフレーム)
	//--------------------------------------------------------------------------------------
	SpWireframeDraw::SpWireframeDraw(const shared_ptr<GameObject>& gameObjectPtr) :
		SpStaticDraw(gameObjectPtr)
	{
		SetDrawActive(true);
	}

	SpWireframeDraw::SpWireframeDraw(const shared_ptr<GameObject>& gameObjectPtr,
		vector<VertexPositionColor>& vertices, vector<uint32_t>& indices) :
		SpStaticDraw(gameObjectPtr)
	{
		try {
			SetDrawActive(true);
			auto pDevice = App::GetBaseDevice();
			auto commandList = pDevice->GetComandList();
			m_baseMesh = BaseMesh::CreateBaseMesh<VertexPositionColor>(commandList, vertices, indices);
		}
		catch (...) {
			throw;
		}
	}



	void SpWireframeDraw::CreatePipelineStates() {
		auto pDevice = App::GetBaseDevice();

		ComPtr<ID3D12PipelineState> defaultPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpWireframe");


		const D3D12_RASTERIZER_DESC rasterizerStateDesc =
		{
			D3D12_FILL_MODE_WIREFRAME,
			D3D12_CULL_MODE_NONE,
			FALSE, // FrontCounterClockwise
			D3D12_DEFAULT_DEPTH_BIAS,
			D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
			D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
			TRUE, // DepthClipEnable
			TRUE, // MultisampleEnable
			FALSE, // AntialiasedLineEnable
			0, // ForcedSampleCount
			D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
		};

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
		psoDesc.BlendState = BaseBlendState::GetOpaqueBlend();
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		if (!defaultPipelineState) {
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&defaultPipelineState)));
			NAME_D3D12_OBJECT(defaultPipelineState);
			BasicPipelineStatePool::AddPipelineState(L"SpWireframe", defaultPipelineState);

		}
	}


	void SpWireframeDraw::OnCreate() {
		CreatePipelineStates();
	}


	void SpWireframeDraw::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();

		ComPtr<ID3D12PipelineState> defaultPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpWireframe",true);

		auto pCommandList = pDevice->GetComandList().Get();

		pCommandList->SetPipelineState(defaultPipelineState.Get());

		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		auto paramIndex = m_constBuffParamIndices[pBaseFrame->m_frameIndex];
		//Cbv
		CD3DX12_GPU_DESCRIPTOR_HANDLE cbvGpuHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_constBuffParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), cbvGpuHandle);

		if (!m_baseMesh) {
			throw BaseException(
				L"メッシュが特定できません。",
				L"SpWireframeDraw::PopulateCommandList()"
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
	IMPLEMENT_DX12SHADER(SpVSPNTStaticShadow, App::GetShadersDir() + L"SpVSPNTStaticShadow.cso")

	IMPLEMENT_DX12SHADER(SpPSPNTStatic, App::GetShadersDir() + L"SpPSPNTStatic.cso")
	IMPLEMENT_DX12SHADER(SpPSPNTStaticShadow, App::GetShadersDir() + L"SpPSPNTStaticShadow.cso")

	SpPNTStaticDraw::SpPNTStaticDraw(const shared_ptr<GameObject>& gameObjectPtr) :
		SpStaticDraw(gameObjectPtr)
	{
		SetDrawActive(true);
	}

	SpPNTStaticDraw::SpPNTStaticDraw(const shared_ptr<GameObject>& gameObjectPtr,
		vector<VertexPositionNormalTexture>& vertices, vector<uint32_t>& indices) :
		SpStaticDraw(gameObjectPtr)
	{
		try {
			SetDrawActive(true);
			auto pDevice = App::GetBaseDevice();
			auto commandList = pDevice->GetComandList();
			m_baseMesh = BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(commandList, vertices, indices);
		}
		catch (...) {
			throw;
		}
	}



	void SpPNTStaticDraw::CreatePipelineStates() {
		auto pDevice = App::GetBaseDevice();

		ComPtr<ID3D12PipelineState> defaultPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPNTStaticDefault");
		ComPtr<ID3D12PipelineState> defaultShadowPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPNTStaticDefaultShadow");
		ComPtr<ID3D12PipelineState> alphaPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPNTStaticAlpha");
		ComPtr<ID3D12PipelineState> alphaShadowPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPNTStaticAlphaShadow");


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
		psoDesc.BlendState = BaseBlendState::GetOpaqueBlend();
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		//デフォルト影無し
		if (!defaultPipelineState) {
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&defaultPipelineState)));
			NAME_D3D12_OBJECT(defaultPipelineState);
			BasicPipelineStatePool::AddPipelineState(L"SpPNTStaticDefault", defaultPipelineState);

		}
		//デフォルト影あり
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(SpVSPNTStaticShadow::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpVSPNTStaticShadow::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<UINT8*>(SpPSPNTStaticShadow::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpPSPNTStaticShadow::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		if (!defaultShadowPipelineState) {
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&defaultShadowPipelineState)));
			NAME_D3D12_OBJECT(defaultShadowPipelineState);
			BasicPipelineStatePool::AddPipelineState(L"SpPNTStaticDefaultShadow", defaultShadowPipelineState);

		}
		//アルファ影無し
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
		psoDesc.BlendState = BaseBlendState::GetAlphaBlend();
		if (!alphaPipelineState) {
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&alphaPipelineState)));
			NAME_D3D12_OBJECT(alphaPipelineState);
			BasicPipelineStatePool::AddPipelineState(L"SpPNTStaticAlpha", alphaPipelineState);

		}
		//アルファ影あり
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(SpVSPNTStaticShadow::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpVSPNTStaticShadow::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<UINT8*>(SpPSPNTStaticShadow::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpPSPNTStaticShadow::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		if (!alphaShadowPipelineState) {
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&alphaShadowPipelineState)));
			NAME_D3D12_OBJECT(alphaShadowPipelineState);
			BasicPipelineStatePool::AddPipelineState(L"SpPNTStaticAlphaShadow", alphaPipelineState);
		}
	}


	void SpPNTStaticDraw::OnCreate() {
		CreatePipelineStates();
	}


	void SpPNTStaticDraw::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();

		ComPtr<ID3D12PipelineState> defaultPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPNTStaticDefault",true);
		ComPtr<ID3D12PipelineState> defaultShadowPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPNTStaticDefaultShadow", true);
		ComPtr<ID3D12PipelineState> alphaPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPNTStaticAlpha", true);
		ComPtr<ID3D12PipelineState> alphaShadowPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPNTStaticAlphaShadow", true);

		auto pCommandList = pDevice->GetComandList().Get();
		if (GetGameObject()->IsAlphaActive()) {
			if (IsOwnShadowActive()) {
				pCommandList->SetPipelineState(alphaShadowPipelineState.Get());
				pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"t0"), pBaseFrame->m_shadowDepthHandle);        // Set the shadow texture as an SRV.
			}
			else {
				pCommandList->SetPipelineState(alphaPipelineState.Get());
				pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"t0"), pBaseFrame->m_nullSrvHandle0);        // Set the shadow texture as an SRV.
			}
		}
		else {
			if (IsOwnShadowActive()) {
				pCommandList->SetPipelineState(defaultShadowPipelineState.Get());
				pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"t0"), pBaseFrame->m_shadowDepthHandle);        // Set the shadow texture as an SRV.
			}
			else {
				pCommandList->SetPipelineState(defaultPipelineState.Get());
				pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"t0"), pBaseFrame->m_nullSrvHandle0);        // Set the shadow texture as an SRV.
			}

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
			UINT index = pDevice->GetSamplerIndex(L"LinearClamp");
			if (index == UINT_MAX) {
				throw BaseException(
					L"LinearClampサンプラーが特定できません。",
					L"SpPNTStaticDraw::PopulateCommandList()"
				);
			}
			CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle(
				pDevice->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				index,
				pDevice->GetSamplerDescriptorHandleIncrementSize()
			);
			pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"s0"), samplerHandle);

			index = pDevice->GetSamplerIndex(L"ComparisonLinear");
			if (index == UINT_MAX) {
				throw BaseException(
					L"ComparisonLinearサンプラーが特定できません。",
					L"SpPNTStaticDraw::PopulateCommandList()"
				);
			}
			CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle2(
				pDevice->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				index,
				pDevice->GetSamplerDescriptorHandleIncrementSize()
			);
			pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"s1"), samplerHandle2);


		}
		//Cbv
		auto paramIndex = m_constBuffParamIndices[pBaseFrame->m_frameIndex];
		CD3DX12_GPU_DESCRIPTOR_HANDLE skyCbvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_constBuffParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), skyCbvHandle);

		if (!m_baseMesh) {
			throw BaseException(
				L"メッシュが特定できません。",
				L"SpPNTStaticDraw::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);
	}





}
// end namespace basecross
