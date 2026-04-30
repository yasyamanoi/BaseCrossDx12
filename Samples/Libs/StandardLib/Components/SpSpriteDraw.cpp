/*!
@file SpPCTSpriteDraw.h
@brief 標準的なスプライトコンポーネント　実体
*/

#include "stdafx.h"

namespace basecross {

	IMPLEMENT_DX12SHADER(SpVSPCTSprite, App::GetShadersDir() + L"SpVSPCTSprite.cso")
	IMPLEMENT_DX12SHADER(SpPSPCTSprite, App::GetShadersDir() + L"SpPSPCTSprite.cso")


	SpPCTSpriteDraw::SpPCTSpriteDraw(const std::shared_ptr<GameObject>& gameObjectPtr,
		std::vector<VertexPositionColorTexture>& vertices, std::vector<uint32_t>& indices):
		Component(gameObjectPtr),
		m_alphaActive(true)
	{
		try {
			ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
			m_mesh = BaseMesh::CreateBaseMesh(pCommandList, vertices, indices);
		}
		catch (...) {
			throw;
		}

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
				//デフィーズはすべて通す
			}
		}
	}

	void SpPCTSpriteDraw::OnCommitConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto pCurrentFrameResource = scene->GetCurrentFrameResource();
		//シーン
		memcpy(pCurrentFrameResource->m_baseConstantBufferSetVec[m_constantIndex].m_pBaseConstantBufferWO,
			&m_spriteConstant, sizeof(m_spriteConstant));
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
		auto mesh = m_mesh;;
		auto texture = pBaseScene->GetTexture(L"MESSAGE_TX");
		if (mesh && texture) {
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
			pCommandList->IASetVertexBuffers(0, 1, &mesh->GetVertexBufferView());
			pCommandList->IASetIndexBuffer(&mesh->GetIndexBufferView());
			pCommandList->DrawIndexedInstanced(mesh->GetNumIndices(), 1, 0, 0, 0);
		}

	}



}
// end namespace basecross
