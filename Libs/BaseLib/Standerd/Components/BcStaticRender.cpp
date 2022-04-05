/*!
@file BcStaticRender.cpp
@brief BasicStatic描画コンポーネント群
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	IMPLEMENT_DX12SHADER(BcVSPNTStaticPL, App::GetShadersDir() + L"BcVSPNTStaticPL.cso")
	IMPLEMENT_DX12SHADER(BcPSPNTPL, App::GetShadersDir() + L"BcPSPNTPL.cso")

	//--------------------------------------------------------------------------------------
	///	BcStatic描画コンポーネント
	//--------------------------------------------------------------------------------------
	void BcStaticRender::SetConstants(BasicConstants& constants, const shared_ptr<Transform>& transform) {
		//初期化
		constants = {};
		constants.activeFlg.x = 3;
		constants.activeFlg.y = 1;
		auto stage = GetStage();
		auto world = transform->GetWorldMatrix();
		auto view = stage->GetActiveCamera()->GetViewMatrix();
		auto proj = stage->GetActiveCamera()->GetProjectionMatrix();
		auto worldView = world * view;
		constants.worldViewProj = XMMatrixTranspose(XMMatrixMultiply(worldView, proj));
		constants.fogVector = g_XMZero;
		constants.fogColor = g_XMZero;
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

	}

	//--------------------------------------------------------------------------------------
	///	BcPNTStatic描画コンポーネント
	//--------------------------------------------------------------------------------------
	BcPNTStaticRender::BcPNTStaticRender(const shared_ptr<GameObject>& gameObjectPtr) :
		BcStaticRender(gameObjectPtr)
	{
		SetRenderActive(true);
	}

	ComPtr<ID3D12PipelineState> BcPNTStaticRender::m_defaultPipelineState(nullptr);
	ComPtr<ID3D12PipelineState> BcPNTStaticRender::m_alphaPipelineState(nullptr);

	void BcPNTStaticRender::CreatePipelineStates() {
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
				reinterpret_cast<UINT8*>(BcVSPNTStaticPL::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				BcVSPNTStaticPL::GetPtr()->GetShaderComPtr()->GetBufferSize()
			};
			psoDesc.PS =
			{
				reinterpret_cast<UINT8*>(BcPSPNTPL::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				BcPSPNTPL::GetPtr()->GetShaderComPtr()->GetBufferSize()
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

			psoDesc.BlendState = BaseRenderState::GetAlphaBlendEx();
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_alphaPipelineState)));
			NAME_D3D12_OBJECT(m_alphaPipelineState);
		}
	}

	void BcPNTStaticRender::WriteConstantBuffers(BaseFrame* pBaseFrame) {
		BasicConstants constants;
		SetConstants(constants, GetGameObject()->GetComponent<Transform>());
		memcpy(pBaseFrame->m_frameParamVec[m_frameParamIndex].m_pConstantBuffer, &constants, sizeof(BasicConstants));
	}


	void BcPNTStaticRender::OnInitFrame(BaseFrame* pBaseFrame) {
		auto pDefaultDev = App::GetBaseDevice();
		auto pDevice = App::GetID3D12Device();

		// コンスタントバッファのサイズは256バイト境界ごとに作成する
		UINT constsize = (sizeof(BasicConstants) + 255) & ~255;

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


	void BcPNTStaticRender::OnInit() {
		CreatePipelineStates();
	}


	void BcPNTStaticRender::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();
		auto pCommandList = pDevice->GetComandList().Get();
		//Srv
		if (!m_baseTexture) {
			throw BaseException(
				L"テクスチャが特定できません。",
				L"BcPNTStaticRender::PopulateCommandList()"
			);
		}
		CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			m_baseTexture->GetSrvIndex(),
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"t1"), srvHandle);

		//Sampler
		UINT index = pDevice->GetSamplerIndex(L"LinearClamp");
		CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle(
			pDevice->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			index,
			pDevice->GetSamplerDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"s0"), samplerHandle);

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
				L"BcPNTStaticRender::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);
	}

}
// end namespace basecross
