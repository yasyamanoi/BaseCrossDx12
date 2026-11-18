/*!
@file SpPCTSpriteDraw.cpp
@brief 標準的なスプライトコンポーネント　実体
*/

#include "stdafx.h"

namespace basecross {

	IMPLEMENT_DX12SHADER(SpVSPCTSprite, App::GetShadersDir() + L"SpVSPCTSprite.cso")
	IMPLEMENT_DX12SHADER(SpPSPCTSprite, App::GetShadersDir() + L"SpPSPCTSprite.cso")


	SpPCTSpriteDraw::SpPCTSpriteDraw(const std::shared_ptr<GameObject>& gameObjectPtr,
		std::vector<VertexPositionColorTexture>& vertices, std::vector<uint32_t>& indices):
		Component(gameObjectPtr),
		m_diffuse(1.0f, 1.0f, 1.0f, 1.0f),
		m_alphaActive(true),
		m_vertices(vertices),
		m_pVertexBufferWOs{},
		m_vertexBufferViews{},
		m_indexBufferView{},
		m_numIndices(0)
	{
		try {
			if (vertices.empty() || indices.empty()) {
				throw BaseException(
					L"頂点またはインデックスが空です。",
					L"if (vertices.empty() || indices.empty())",
					L"SpPCTSpriteDraw::SpPCTSpriteDraw()"
				);
			}
			//コマンドリストは使わず、UPLOADヒープに直接書く（ゲームの途中でも作れる）
			auto device = App::GetD3D12Device();
			CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_UPLOAD);
			CD3DX12_RANGE readRange(0, 0);
			//頂点バッファ（フレームの数だけ作る）
			UINT vertexBufferSize = static_cast<UINT>(sizeof(VertexPositionColorTexture) * vertices.size());
			CD3DX12_RESOURCE_DESC vertexDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
			for (UINT i = 0; i < BaseDevice::FrameCount; i++) {
				ThrowIfFailedEx(device->CreateCommittedResource(
					&heapProp,
					D3D12_HEAP_FLAG_NONE,
					&vertexDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_vertexBuffers[i])),
					L"頂点バッファ作成に失敗しました。",
					L"if(FAILED(device->CreateCommittedResource())",
					L"SpPCTSpriteDraw::SpPCTSpriteDraw()"
				);
				//Mapしたままにする（書き込み専用として使う）
				ThrowIfFailedEx(m_vertexBuffers[i]->Map(0, &readRange,
					reinterpret_cast<void**>(&m_pVertexBufferWOs[i])),
					L"頂点バッファのMapに失敗しました。",
					L"if(FAILED(m_vertexBuffers[i]->Map())",
					L"SpPCTSpriteDraw::SpPCTSpriteDraw()"
				);
				//作成直後の描画に備えて、最初の頂点を写しておく
				memcpy(m_pVertexBufferWOs[i], vertices.data(), vertexBufferSize);
				m_vertexBufferViews[i].BufferLocation = m_vertexBuffers[i]->GetGPUVirtualAddress();
				m_vertexBufferViews[i].StrideInBytes = static_cast<UINT>(sizeof(VertexPositionColorTexture));
				m_vertexBufferViews[i].SizeInBytes = vertexBufferSize;
			}
			//インデックスバッファ（1つだけ作り、1回だけ書く）
			UINT indexBufferSize = static_cast<UINT>(sizeof(uint32_t) * indices.size());
			CD3DX12_RESOURCE_DESC indexDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);
			ThrowIfFailedEx(device->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&indexDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_indexBuffer)),
				L"インデックスバッファ作成に失敗しました。",
				L"if(FAILED(device->CreateCommittedResource())",
				L"SpPCTSpriteDraw::SpPCTSpriteDraw()"
			);
			UINT8* pIndexData = nullptr;
			ThrowIfFailedEx(m_indexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pIndexData)),
				L"インデックスバッファのMapに失敗しました。",
				L"if(FAILED(m_indexBuffer->Map())",
				L"SpPCTSpriteDraw::SpPCTSpriteDraw()"
			);
			memcpy(pIndexData, indices.data(), indexBufferSize);
			m_indexBuffer->Unmap(0, nullptr);
			m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
			m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
			m_indexBufferView.SizeInBytes = indexBufferSize;
			m_numIndices = static_cast<UINT>(indices.size());
		}
		catch (...) {
			throw;
		}

	}

	//現在のフレームの番号（フレームリソースの並びの中での位置）
	size_t SpPCTSpriteDraw::GetCurrentFrameIndex()const {
		auto pBaseScene = BaseScene::Get();
		auto pCurrentFrameResource = pBaseScene->GetCurrentFrameResource();
		auto& frameResources = pBaseScene->GetFrameResources();
		for (size_t i = 0; i < BaseDevice::FrameCount; i++) {
			if (frameResources[i].get() == pCurrentFrameResource) {
				return i;
			}
		}
		throw BaseException(
			L"現在のフレームリソースが見つかりません。",
			L"if (frameResources[i].get() == pCurrentFrameResource)",
			L"SpPCTSpriteDraw::GetCurrentFrameIndex()"
		);
		return 0;
	}

	//頂点の設定（数が違うときは例外）
	void SpPCTSpriteDraw::SetVertices(const std::vector<VertexPositionColorTexture>& vertices) {
		if (vertices.size() != m_vertices.size()) {
			throw BaseException(
				L"頂点の数が作成時と違います。",
				L"if (vertices.size() != m_vertices.size())",
				L"SpPCTSpriteDraw::SetVertices()"
			);
		}
		m_vertices = vertices;
	}

	void SpPCTSpriteDraw::OnCreate() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		//シンプルコンスタントバッファ
		for (size_t i = 0; i < BaseDevice::FrameCount; i++) {
			m_constantIndex =
				frameResources[i]->AddBaseConstantBufferSet<SimpleConstant>(pBaseDevice->GetD3D12Device());
		}
		// シーンパイプラインステート
		{
			ComPtr<ID3D12PipelineState> alphaPipelineState
				= PipelineStatePool::GetPipelineState(L"SpPCTSpriteAlpha");
			CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
			//カリング
			rasterizerStateDesc.CullMode = D3D12_CULL_MODE_BACK;

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			ZeroMemory(&psoDesc, sizeof(psoDesc));
			psoDesc.InputLayout = { VertexPositionColorTexture::GetVertexElement(), VertexPositionColorTexture::GetNumElements() };
			psoDesc.pRootSignature = RootSignaturePool::GetRootSignature(L"BaseCrossDefault").Get();
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
			psoDesc.RasterizerState = rasterizerStateDesc;
			psoDesc.BlendState = BlendState::GetOpaqueBlend();
//			psoDesc.DepthStencilState = 0;
//			psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc.Count = 1;
			psoDesc.BlendState = BlendState::GetAlphaBlend();
			if (!alphaPipelineState) {
				ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&alphaPipelineState)));
				NAME_D3D12_OBJECT(alphaPipelineState);
				PipelineStatePool::AddPipelineState(L"SpPCTSpriteAlpha", alphaPipelineState);

			}
		}
	}


	void SpPCTSpriteDraw::OnUpdateConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto stage = std::dynamic_pointer_cast<Stage>(scene->GetActiveStage(true));
		auto& frameResources = scene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		auto& viewport = scene->GetViewport();
		std::shared_ptr<PerspecCamera> myCamera;
		std::shared_ptr<LightSet> myLightSet;
		if (!stage) {
			return;
		}

		auto gameObject = m_gameObject.lock();
		if (gameObject) {

			myCamera = std::dynamic_pointer_cast<PerspecCamera>(gameObject->GetCamera());
			myLightSet = gameObject->GetLightSet();

			//Transformコンポーネントを取り出す
			auto ptrTrans = gameObject->GetComponent<Transform>();
			auto& param = ptrTrans->GetTransParam();
			//シーンのコンスタントバッファ
			{
				//初期化
				m_spriteConstant = {};
				Mat4x4 world = ptrTrans->Get2DWorldMatrix();
				Mat4x4 proj;
				gameObject->Get2DDrawProjMatrix(proj);
				world *= proj;
				m_spriteConstant.world = world;
				//エミッシブ
				m_spriteConstant.emissive = GetEmissive();
				//ディフューズ
				m_spriteConstant.diffuse = GetDiffuse();
			}
		}
	}

	void SpPCTSpriteDraw::OnCommitConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto pCurrentFrameResource = scene->GetCurrentFrameResource();
		//シーン
		memcpy(pCurrentFrameResource->m_baseConstantBufferSetVec[m_constantIndex].m_pBaseConstantBufferWO,
			&m_spriteConstant, sizeof(m_spriteConstant));
		//頂点を現在のフレームのバッファへ写す（毎フレーム）
		memcpy(m_pVertexBufferWOs[GetCurrentFrameIndex()], m_vertices.data(),
			sizeof(VertexPositionColorTexture) * m_vertices.size());
	}

	void SpPCTSpriteDraw::OnSceneDraw(ID3D12GraphicsCommandList* pCommandList) {
		//		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pCurrentFrameResource = pBaseScene->GetCurrentFrameResource();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		auto& viewport = pBaseScene->GetViewport();
		auto& scissorRect = pBaseScene->GetScissorRect();
		auto depthDsvs = pBaseScene->GetDepthDsvs();
		auto depthGPUDsvs = pBaseScene->GetDepthSrvGpuHandles();

		auto CbvSrvUavDescriptorHeap = pBaseScene->GetCbvSrvUavDescriptorHeap();
		//テクスチャは部品に指定されたものを使う。無ければMESSAGE_TX
		auto texture = GetBaseTexture();
		if (!texture) {
			texture = pBaseScene->GetTexture(L"MESSAGE_TX");
		}
		if (texture) {
			ComPtr<ID3D12PipelineState> alphaPipelineState
				= PipelineStatePool::GetPipelineState(L"SpPCTSpriteAlpha", true);
			//null rv
			CD3DX12_GPU_DESCRIPTOR_HANDLE cbvSrvGpuNullHandle(CbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

			// set PipelineState and GetGpuSlotID(L"t0")
			pCommandList->SetPipelineState(alphaPipelineState.Get());
			pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t0"), cbvSrvGpuNullHandle);        // Set the shadow texture as an SRV.
			//Sampler
			UINT index = pBaseScene->GetSamplerIndex(L"LinearWrap");
			if (index == UINT_MAX) {
				throw BaseException(
					L"LinearClampサンプラーが特定できません。",
					L"Scene::ScenePass()"
				);
			}
			CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle(
				pBaseScene->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				index,
				pBaseScene->GetSamplerDescriptorHandleIncrementSize()
			);
			pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"s0"), samplerHandle);

			//シェーダリソース（テクスチャ）のハンドルの設定
			CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(
				pBaseScene->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				texture->GetSrvIndex(),
				pBaseScene->GetCbvSrvUavDescriptorHandleIncrementSize()
			);
			pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t1"), srvHandle);
			//Cbv
			// Set scene constant buffer.
			pCommandList->SetGraphicsRootConstantBufferView(pBaseScene->GetGpuSlotID(L"b0"),
				pCurrentFrameResource->m_baseConstantBufferSetVec[m_constantIndex].m_baseConstantBuffer->GetGPUVirtualAddress());
			//Draw
			pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			//現在のフレームの頂点バッファビューを使う
			size_t frameIndex = GetCurrentFrameIndex();
			pCommandList->IASetVertexBuffers(0, 1, &m_vertexBufferViews[frameIndex]);
			pCommandList->IASetIndexBuffer(&m_indexBufferView);
			pCommandList->DrawIndexedInstanced(m_numIndices, 1, 0, 0, 0);
		}

	}



}
// end namespace basecross
