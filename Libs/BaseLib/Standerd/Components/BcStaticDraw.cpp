/*!
@file BcStaticDraw.cpp
@brief Basicスタティック描画コンポーネント群
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	BcStatic描画コンポーネント
	//--------------------------------------------------------------------------------------
	void BcStaticDraw::SetConstants(BasicConstants& constants, const shared_ptr<Transform>& transform) {
		//初期化
		constants = {};
		constants.activeFlg.x = 3;
		constants.activeFlg.y = 1;
		auto stage = GetStage();
		auto world = transform->GetWorldMatrix();
		auto view = stage->GetActiveCamera()->GetViewMatrix();
		auto proj = stage->GetActiveCamera()->GetProjMatrix();
		auto worldView = world * view;
		constants.worldViewProj = XMMatrixTranspose(XMMatrixMultiply(worldView, proj));
		//フォグの設定
		if (IsFogEnabled())
		{
			auto start = m_fogStart;
			auto end = m_fogEnd;
			if (start == end)
			{
				// Degenerate case: force everything to 100% fogged if start and end are the same.
				static const XMVECTORF32 fullyFogged = { 0, 0, 0, 1 };
				constants.fogVector = fullyFogged;
			}
			else
			{
				XMMATRIX worldViewTrans = worldView;
				// _13, _23, _33, _43
				XMVECTOR worldViewZ = XMVectorMergeXY(XMVectorMergeZW(worldViewTrans.r[0], worldViewTrans.r[2]),
					XMVectorMergeZW(worldViewTrans.r[1], worldViewTrans.r[3]));
				XMVECTOR wOffset = XMVectorSwizzle<1, 2, 3, 0>(XMLoadFloat(&start));
				constants.fogVector = (worldViewZ + wOffset) / (start - end);
			}
			constants.fogColor = m_fogColor;
		}
		else
		{
			constants.fogVector = g_XMZero;
			constants.fogColor = g_XMZero;
		}
		//ライトの決定
		auto lightSet = stage->GetActiveLightSet();
		for (int i = 0; i < lightSet->GetNumLights(); i++) {
			constants.lightDirection[i] = lightSet->GetLight(i).m_directional;
			constants.lightDiffuseColor[i] = lightSet->GetLight(i).m_diffuseColor;
			constants.lightSpecularColor[i] = lightSet->GetLight(i).m_specularColor;
		}
		//ワールド行列
		constants.world = XMMatrixTranspose(world);

		XMMATRIX worldInverse = XMMatrixInverse(nullptr, world);
		constants.worldInverseTranspose[0] = worldInverse.r[0];
		constants.worldInverseTranspose[1] = worldInverse.r[1];
		constants.worldInverseTranspose[2] = worldInverse.r[2];

		XMMATRIX viewInverse = XMMatrixInverse(nullptr, view);
		constants.eyePosition = viewInverse.r[3];

		XMVECTOR diffuse = Col4(1.0f);
		XMVECTOR alphaVector = XMVectorReplicate(1.0);
		XMVECTOR emissiveColor = Col4(0.0f);
		XMVECTOR ambientLightColor = lightSet->GetAmbient();
		// emissive と ambientとライトをマージする
		constants.emissiveColor = (emissiveColor + ambientLightColor * diffuse) * alphaVector;
		constants.specularColorAndPower = Col4(0, 0, 0, 1);

		// xyz = diffuse * alpha, w = alpha.
		constants.diffuseColor = XMVectorSelect(alphaVector, diffuse * alphaVector, g_XMSelect1110);

		if (IsOwnShadowActive()) {
			auto camaraPtr = stage->GetActiveCamera();
			auto mainLight = lightSet->GetMainBaseLight();
			bsm::Vec3 calcLightDir(mainLight.m_directional * -1.0);
			bsm::Vec3 lightAt(camaraPtr->GetAt());
			bsm::Vec3 lightEye(calcLightDir);
			lightEye *= Shadowmap::GetLightHeight();
			lightEye = lightAt + lightEye;
			bsm::Vec4 LightEye4(lightEye, 1.0f);
			LightEye4.w = 1.0f;
			constants.lightPos = LightEye4;
			bsm::Vec4 eyePos4(camaraPtr->GetEye(), 1.0f);
			eyePos4.w = 1.0f;
			constants.eyePos = eyePos4;
			bsm::Mat4x4 LightView, LightProj;
			//ライトのビューと射影を計算
			LightView = XMMatrixLookAtLH(lightEye, lightAt, bsm::Vec3(0, 1.0f, 0));
			LightProj = XMMatrixOrthographicLH(Shadowmap::GetViewWidth(), Shadowmap::GetViewHeight(),
				Shadowmap::GetLightNear(), Shadowmap::GetLightFar());
			constants.lightView = bsm::transpose(LightView);
			constants.lightProjection = bsm::transpose(LightProj);
		}


	}

	void BcStaticDraw::OnInitFrame(BaseFrame* pBaseFrame) {
		auto pDefaultDev = App::GetBaseDevice();
		auto pDevice = App::GetID3D12Device();

		// コンスタントバッファのサイズは256バイト境界ごとに作成する
		UINT constsize = (sizeof(BasicConstants) + 255) & ~255;

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

	void BcStaticDraw::WriteConstantBuffers(BaseFrame* pBaseFrame) {
		BasicConstants constants;
		SetConstants(constants, GetGameObject()->GetComponent<Transform>());
		memcpy(pBaseFrame->m_constBuffParamVec[m_constBuffParamIndex].m_pConstantBuffer, &constants, sizeof(BasicConstants));
	}

	IMPLEMENT_DX12SHADER(BcVSPNTStaticPL, App::GetShadersDir() + L"BcVSPNTStaticPL.cso")
	IMPLEMENT_DX12SHADER(BcPSPNTPL, App::GetShadersDir() + L"BcPSPNTPL.cso")

	IMPLEMENT_DX12SHADER(BcVSPNTStaticPLShadow, App::GetShadersDir() + L"BcVSPNTStaticPLShadow.cso")
	IMPLEMENT_DX12SHADER(BcPSPNTPLShadow, App::GetShadersDir() + L"BcPSPNTPLShadow.cso")

	BcPNTStaticDraw::BcPNTStaticDraw(const shared_ptr<GameObject>& gameObjectPtr) :
		BcStaticDraw(gameObjectPtr)
	{
		SetDrawActive(true);
	}

	BcPNTStaticDraw::BcPNTStaticDraw(const shared_ptr<GameObject>& gameObjectPtr,
		vector<VertexPositionNormalTexture>& vertices, vector<uint32_t>& indices) :
		BcStaticDraw(gameObjectPtr)
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



	void BcPNTStaticDraw::CreatePipelineStates() {
		auto pDevice = App::GetBaseDevice();

		ComPtr<ID3D12PipelineState> defaultPipelineState 
			= BasicPipelineStatePool::GetPipelineState(L"BcPNTStaticDefault");
		ComPtr<ID3D12PipelineState> defaultShadowPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"BcPNTStaticDefaultShadow");
		ComPtr<ID3D12PipelineState> alphaPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"BcPNTStaticAlpha");
		ComPtr<ID3D12PipelineState> alphaShadowPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"BcPNTStaticAlphaShadow");


		CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
		//カリング
		rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		ZeroMemory(&psoDesc, sizeof(psoDesc));
		psoDesc.InputLayout = { VertexPositionNormalTexture::GetVertexElement(), VertexPositionNormalTexture::GetNumElements() };
		psoDesc.pRootSignature = pDevice->GetRootSignature().Get();
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(BcVSPNTStaticPL::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			BcVSPNTStaticPL::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<UINT8*>(BcPSPNTPL::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			BcPSPNTPL::GetPtr()->GetShaderComPtr()->GetBufferSize()
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
			BasicPipelineStatePool::AddPipelineState(L"BcPNTStaticDefault", defaultPipelineState);
		}
		//アルファ影なし
		psoDesc.BlendState = BaseBlendState::GetAlphaBlendEx();
		if (!alphaPipelineState) {
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&alphaPipelineState)));
			NAME_D3D12_OBJECT(alphaPipelineState);
			BasicPipelineStatePool::AddPipelineState(L"BcPNTStaticAlpha", alphaPipelineState);
		}
		//デフォルト影あり
		psoDesc.BlendState = BaseBlendState::GetOpaqueBlend();
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(BcVSPNTStaticPLShadow::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			BcVSPNTStaticPLShadow::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<UINT8*>(BcPSPNTPLShadow::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			BcPSPNTPLShadow::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		if (!defaultShadowPipelineState) {
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&defaultShadowPipelineState)));
			NAME_D3D12_OBJECT(defaultShadowPipelineState);
			BasicPipelineStatePool::AddPipelineState(L"BcPNTStaticDefaultShadow", defaultShadowPipelineState);
		}
		psoDesc.BlendState = BaseBlendState::GetAlphaBlendEx();
		//アルファ影あり
		if (!alphaShadowPipelineState) {
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&alphaShadowPipelineState)));
			NAME_D3D12_OBJECT(alphaShadowPipelineState);
			BasicPipelineStatePool::AddPipelineState(L"BcPNTStaticAlphaShadow", alphaShadowPipelineState);
		}
	}


	void BcPNTStaticDraw::OnCreate() {
		CreatePipelineStates();
	}


	void BcPNTStaticDraw::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();
		ComPtr<ID3D12PipelineState> defaultPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"BcPNTStaticDefault",true);
		ComPtr<ID3D12PipelineState> defaultShadowPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"BcPNTStaticDefaultShadow", true);
		ComPtr<ID3D12PipelineState> alphaPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"BcPNTStaticAlpha", true);
		ComPtr<ID3D12PipelineState> alphaShadowPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"BcPNTStaticAlphaShadow", true);


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
					L"BcPNTStaticDraw::PopulateCommandList()"
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
					L"BcPNTStaticDraw::PopulateCommandList()"
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
				L"BcPNTStaticDraw::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);
	}




}
// end namespace basecross
