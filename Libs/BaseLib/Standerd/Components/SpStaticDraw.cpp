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
		m_constBuffParamIndex = pBaseFrame->m_constBuffParamVec.size();
		pBaseFrame->m_constBuffParamVec.push_back(param);

	}

	void SpStaticDraw::WriteConstantBuffers(BaseFrame* pBaseFrame) {
		SimpleConstant constants;
		SetConstant(constants, GetGameObject()->GetComponent<Transform>());
		memcpy(pBaseFrame->m_constBuffParamVec[m_constBuffParamIndex].m_pConstantBuffer, &constants, sizeof(SimpleConstant));
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
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_defaultPipelineState)));
		NAME_D3D12_OBJECT(m_defaultPipelineState);
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
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_defaultShadowPipelineState)));
		NAME_D3D12_OBJECT(m_defaultShadowPipelineState);
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
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_alphaPipelineState)));
		NAME_D3D12_OBJECT(m_alphaPipelineState);
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
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_alphaShadowPipelineState)));
		NAME_D3D12_OBJECT(m_alphaShadowPipelineState);
	}


	void SpPNTStaticDraw::OnCreate() {
		CreatePipelineStates();
	}


	void SpPNTStaticDraw::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();
		auto pCommandList = pDevice->GetComandList().Get();
		if (GetGameObject()->IsAlphaActive()) {
			if (IsOwnShadowActive()) {
				pCommandList->SetPipelineState(m_alphaShadowPipelineState.Get());
				pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"t0"), pBaseFrame->m_shadowDepthHandle);        // Set the shadow texture as an SRV.
			}
			else {
				pCommandList->SetPipelineState(m_alphaPipelineState.Get());
				pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"t0"), pBaseFrame->m_nullSrvHandle0);        // Set the shadow texture as an SRV.
			}
		}
		else {
			if (IsOwnShadowActive()) {
				pCommandList->SetPipelineState(m_defaultShadowPipelineState.Get());
				pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"t0"), pBaseFrame->m_shadowDepthHandle);        // Set the shadow texture as an SRV.
			}
			else {
				pCommandList->SetPipelineState(m_defaultPipelineState.Get());
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
		auto paramIndex = GetConstBuffParamIndex();
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
