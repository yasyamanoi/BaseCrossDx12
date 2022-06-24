/*!
@file ShadowmapDraw.cpp
@brief シャドウマップ描画コンポーネント
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	Shadowmap描画コンポーネント
	//--------------------------------------------------------------------------------------

	IMPLEMENT_DX12SHADER(PNTShadowmap, App::GetShadersDir() + L"VSShadowmap.cso")

	float Shadowmap::m_lightHeight(200.0f);
	float Shadowmap::m_lightNear(1.0f);
	float Shadowmap::m_lightFar(220.0f);
	float Shadowmap::m_viewWidth(32.0f);
	float Shadowmap::m_viewHeight(32.0f);
	float Shadowmap::m_posAdjustment(0.1f);


	void Shadowmap::CreatePipelineStates() {
		auto pDevice = App::GetBaseDevice();

		ComPtr<ID3D12PipelineState> PNTShadowmapPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"PNTShadowmap");

		// シャドウマップ用
		CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		depthStencilDesc.StencilEnable = FALSE;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { VertexPositionNormalTexture::GetVertexElement(), VertexPositionNormalTexture::GetNumElements() };;
		psoDesc.pRootSignature = pDevice->GetRootSignature().Get();
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(PNTShadowmap::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			PNTShadowmap::GetPtr()->GetShaderComPtr()->GetBufferSize()

		};
		psoDesc.PS =
		{
			CD3DX12_SHADER_BYTECODE(0, 0)
		};
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState = depthStencilDesc;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 0;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleDesc.Count = 1;
		if (!PNTShadowmapPipelineState) {
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&PNTShadowmapPipelineState)));
			NAME_D3D12_OBJECT(PNTShadowmapPipelineState);
			BasicPipelineStatePool::AddPipelineState(L"PNTShadowmap", PNTShadowmapPipelineState);

		}

	}

	//コンスタントバッファの登録
	void Shadowmap::SetConstant(ShadowConstant& constant, const shared_ptr<Transform>& transform) {
		//すべて0に初期化
		constant = {};
		bsm::Mat4x4 world;
		auto lights = GetGameObject()->GetActiveLightSet();
		auto light = lights->GetMainBaseLight();

		//位置の取得
		auto pos = transform->GetWorldMatrix().transInMatrix();
		bsm::Vec3 posSpan = light.m_directional;
		posSpan *= m_posAdjustment;
		pos += posSpan;
		//ワールド行列の決定
		world.affineTransformation(
			transform->GetScale(),			//スケーリング
			transform->GetPivot(),		//回転の中心（重心）
			transform->GetQuaternion(),				//回転角度
			pos				//位置
		);


		Vec3 lightDir = -1.0 * light.m_directional;
		auto camera = GetGameObject()->GetActiveCamera();
		Vec3 lightAt = camera->GetAt();
		Vec3 lightEye = lightAt + (lightDir * m_lightHeight);
		auto width = App::GetBaseDevice()->GetViewport().Width;
		auto height = App::GetBaseDevice()->GetViewport().Height;
		Mat4x4 lightView = XMMatrixLookAtLH(lightEye, lightAt, Vec3(0, 1.0f, 0));
		//		Mat4x4 lightProj = XMMatrixOrthographicLH(width, height, m_lightNear, m_lightFar);
		Mat4x4 lightProj = XMMatrixOrthographicLH(m_viewWidth, m_viewHeight, m_lightNear, m_lightFar);
		constant.world = transpose(world);
		constant.view = transpose(lightView);
		constant.projection = transpose(lightProj);
	}

	void Shadowmap::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();

		ComPtr<ID3D12PipelineState> PNTShadowmapPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"PNTShadowmap",true);

		auto pCommandList = pDevice->GetComandList().Get();
		pCommandList->SetPipelineState(PNTShadowmapPipelineState.Get());
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		auto paramIndex = GetConstBuffParamIndex();
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
				L"Shadowmap::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);

	}

	void Shadowmap::OnInitFrame(BaseFrame* pBaseFrame) {
		auto pDefaultDev = App::GetBaseDevice();
		auto pDevice = App::GetID3D12Device();

		// コンスタントバッファのサイズは256バイト境界ごとに作成する
		UINT constsize = (sizeof(ShadowConstant) + 255) & ~255;

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

	void Shadowmap::WriteConstantBuffers(BaseFrame* pBaseFrame) {
		ShadowConstant constants;
		SetConstant(constants, GetGameObject()->GetComponent<Transform>());
		memcpy(pBaseFrame->m_constBuffParamVec[m_constBuffParamIndex].m_pConstantBuffer, &constants, sizeof(ShadowConstant));

	}

	Shadowmap::Shadowmap(const shared_ptr<GameObject>& gameObjectPtr) :
		Component(gameObjectPtr)
	{
		SetDrawActive(true);
	}

	void Shadowmap::OnCreate() {
		CreatePipelineStates();
	}



}
// end namespace basecross
