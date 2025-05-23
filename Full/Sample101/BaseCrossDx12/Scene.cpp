/*!
@file Scene.cpp
@brief シーンクラス　実体
*/


#include "stdafx.h"
#include "Project.h"


namespace basecross {
	
	using namespace SceneEnums;


	Scene::Scene(UINT frameCount, PrimDevice* pPrimDevice) :
		BaseScene(frameCount, pPrimDevice)
	{
	}

	Scene::~Scene()
	{
	}

	std::shared_ptr<Stage> Scene::GetActiveStage(bool ExceptionActive) const {
		if (!m_activeStage) {
			//アクティブなステージが無効なら
			if (ExceptionActive) {
				throw BaseException(
					L"アクティブなステージがありません",
					L"if(!m_activeStage)",
					L"BaseScene::GetActiveStage()"
				);
			}
			else {
				return nullptr;
			}
		}
		return m_activeStage;
	}


	void Scene::CreateAssetResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
	{
		m_pTgtCommandList = pCommandList;
		ResetActiveStage<GameStage>(pDevice);
	}

	void Scene::CreatePipelineStates(ID3D12Device* pDevice) {
		// シャドウマップパイプラインステート
		{
			ComPtr<ID3D12PipelineState> PNTShadowmapPipelineState
				= PipelineStatePool::GetPipelineState(L"PNTShadowmap");
			auto rootSignature = RootSignaturePool::GetRootSignature(L"BaseCrossDefault", true);

			// シャドウマップ用
			CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);
			depthStencilDesc.DepthEnable = TRUE;
			depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
			depthStencilDesc.StencilEnable = FALSE;

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = { VertexPositionNormalTexture::GetVertexElement(), VertexPositionNormalTexture::GetNumElements() };;
			psoDesc.pRootSignature = rootSignature.Get();
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
				ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&PNTShadowmapPipelineState)));
				NAME_D3D12_OBJECT(PNTShadowmapPipelineState);
				PipelineStatePool::AddPipelineState(L"PNTShadowmap", PNTShadowmapPipelineState);
			}
		}
		// Bcシーンパイプラインステート
		{
			ComPtr<ID3D12PipelineState> PNTPipelineState
				= PipelineStatePool::GetPipelineState(L"BcPNTStaticShadow");

			//ラスタライザステート
			CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
			//カリング
			rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;
			//シーンパイプラインステート
			ComPtr<ID3D12PipelineState> bcPNTStaticShadowPipelineState;
			auto rootSignature = RootSignaturePool::GetRootSignature(L"BaseCrossDefault");
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			ZeroMemory(&psoDesc, sizeof(psoDesc));
			psoDesc.InputLayout = { VertexPositionNormalTexture::GetVertexElement(), VertexPositionNormalTexture::GetNumElements() };
			psoDesc.pRootSignature = rootSignature.Get();
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
			psoDesc.RasterizerState = rasterizerStateDesc;
			psoDesc.BlendState = BlendState::GetOpaqueBlend();
			psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc.Count = 1;
			if (!PNTPipelineState) {
				ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&bcPNTStaticShadowPipelineState)));
				NAME_D3D12_OBJECT(bcPNTStaticShadowPipelineState);
				PipelineStatePool::AddPipelineState(L"BcPNTStaticShadow", bcPNTStaticShadowPipelineState);
			}
		}
		//Spシーンパイプラインステート
		{
			ComPtr<ID3D12PipelineState> SpPNTStaticShadowPibline
				= PipelineStatePool::GetPipelineState(L"SpPNTStaticShadow");

			CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
			//カリング
			rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;
			auto rootSignature = RootSignaturePool::GetRootSignature(L"BaseCrossDefault");

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			ZeroMemory(&psoDesc, sizeof(psoDesc));
			psoDesc.InputLayout = { VertexPositionNormalTexture::GetVertexElement(), VertexPositionNormalTexture::GetNumElements() };
			psoDesc.pRootSignature = rootSignature.Get();
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
			psoDesc.RasterizerState = rasterizerStateDesc;
			psoDesc.BlendState = BlendState::GetOpaqueBlend();
			psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc.Count = 1;
			if (!SpPNTStaticShadowPibline) {
				ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&SpPNTStaticShadowPibline)));
				NAME_D3D12_OBJECT(SpPNTStaticShadowPibline);
				PipelineStatePool::AddPipelineState(L"SpPNTStaticShadow", SpPNTStaticShadowPibline);
			}
		}
	}

	void Scene::Update(double elapsedTime) {
		if (m_activeStage) {
			m_activeStage->OnUpdate(elapsedTime);
			m_activeStage->OnUpdate2(elapsedTime);
			UpdateConstantBuffers();
			CommitConstantBuffers();
		}
	}

	void Scene::Destroy() {
		if (m_activeStage) {
			m_activeStage->OnDestroy();
		}
	}


	void Scene::UpdateConstantBuffers() {
		if (m_activeStage) {
			m_activeStage->OnUpdateConstantBuffers();
		}
	}

	void Scene::CommitConstantBuffers() {
		if (m_activeStage) {
			m_activeStage->OnCommitConstantBuffers();
		}
	}


	void Scene::ShadowPass(ID3D12GraphicsCommandList* pCommandList) {
		// Set necessary state.
		auto rootSignature = RootSignaturePool::GetRootSignature(L"BaseCrossDefault", true);
		pCommandList->SetGraphicsRootSignature(rootSignature.Get());
		//PipelineState
		auto shadowPipeline = PipelineStatePool::GetPipelineState(L"PNTShadowmap", true);
		pCommandList->SetPipelineState(shadowPipeline.Get());
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pCommandList->RSSetViewports(1, &m_viewport);
		pCommandList->RSSetScissorRects(1, &m_scissorRect);
		// No render target needed for the shadow pass.
		pCommandList->OMSetRenderTargets(0, nullptr, FALSE, &m_depthDsvs[DepthGenPass::Shadow]);

		if (m_activeStage) {
			m_pTgtCommandList = pCommandList;
			m_activeStage->OnShadowDraw();
		}
	}


	void Scene::ScenePass(ID3D12GraphicsCommandList* pCommandList)
	{
		auto& viewport = GetViewport();
		auto& scissorRect = GetScissorRect();
		auto depthDsvs = GetDepthDsvs();
		// Set necessary state.
		auto rootSignature = RootSignaturePool::GetRootSignature(L"BaseCrossDefault", true);
		pCommandList->SetGraphicsRootSignature(rootSignature.Get());
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pCommandList->RSSetViewports(1, &viewport);
		pCommandList->RSSetScissorRects(1, &scissorRect);

		if (m_activeStage) {
			m_pTgtCommandList = pCommandList;
			m_activeStage->OnSceneDraw();
		}
	}

	void Scene::UpdateUI(std::unique_ptr<UILayer>& uiLayer) {
		auto device = BaseDevice::GetBaseDevice();
		//1秒間に１回更新される安定したfpsを得る
		auto fps = device->GetStableFps();
		//1秒間に１回更新される安定したelapsedTimeを得る
		auto elapsedTime = device->GetStableElapsedTime();

		std::wstring uiText = L"";
		wchar_t buff[512];
		swprintf_s(buff, 500, L"FPS: %.1f\n", fps);
		uiText = buff;
		swprintf_s(buff, 500, L"ElapsedTime: %.6f\n", elapsedTime);
		uiText += buff;
		uiLayer->UpdateLabels(uiText);
	}

	void Scene::SetToBefore() {
		if (m_activeStage) {
			m_activeStage->SetToBefore();
		}
	}


	


}
// end namespace basecross




