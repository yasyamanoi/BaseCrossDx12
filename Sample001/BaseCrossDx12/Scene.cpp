/*!
@file Scene.cpp
@brief シーンクラス　実体
*/


#include "stdafx.h"
#include "Project.h"


namespace basecross {

	IMPLEMENT_DX12SHADER(BcVSPNTStaticPL, App::GetShadersDir() + L"BcVSPNTStaticPL.cso")
	IMPLEMENT_DX12SHADER(BcPSPNTPL, App::GetShadersDir() + L"BcPSPNTPL.cso")

	IMPLEMENT_DX12SHADER(BcVSPNTStaticPLShadow, App::GetShadersDir() + L"BcVSPNTStaticPLShadow.cso")
	IMPLEMENT_DX12SHADER(BcPSPNTPLShadow, App::GetShadersDir() + L"BcPSPNTPLShadow.cso")


	using namespace std;
	using namespace SceneEnums;


	Scene::Scene(UINT frameCount, PrimDevice* pPrimDevice) :
		BaseScene(frameCount, pPrimDevice)
	{
	}

	Scene::~Scene()
	{
	}

	shared_ptr<Stage> Scene::GetActiveStage(bool ExceptionActive) const {
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
		// シーンパイプラインステート
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
	}

	void Scene::Update(double elapsedTime) {
		if (m_activeStage) {
			m_activeStage->OnUpdate(elapsedTime);
			m_activeStage->OnUpdate2(elapsedTime);
			UpdateConstantBuffers();
			CommitConstantBuffers();
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
		//t0: DepthGenPass::Shadow
		pCommandList->SetGraphicsRootDescriptorTable(GetGpuSlotID(L"t0"), m_depthSrvGpuHandles[DepthGenPass::Shadow]);        // Set the shadow texture as an SRV.
		pCommandList->OMSetRenderTargets(1, &GetCurrentBackBufferRtvCpuHandle(), FALSE, &depthDsvs[DepthGenPass::Scene]);
		//PipelineState
		auto scenePipelineState = PipelineStatePool::GetPipelineState(L"BcPNTStaticShadow", true);
		pCommandList->SetPipelineState(scenePipelineState.Get());
		//Sampler
		UINT index = GetSamplerIndex(L"LinearClamp");
		if (index == UINT_MAX) {
			throw BaseException(
				L"LinearClampサンプラーが特定できません。",
				L"Scene::ScenePass()"
			);
		}
		CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle(
			GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			index,
			GetSamplerDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(GetGpuSlotID(L"s0"), samplerHandle);

		index = GetSamplerIndex(L"ComparisonLinear");
		if (index == UINT_MAX) {
			throw BaseException(
				L"ComparisonLinearサンプラーが特定できません。",
				L"Scene::ScenePass()"
			);
		}
		CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle2(
			GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			index,
			GetSamplerDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(GetGpuSlotID(L"s1"), samplerHandle2);

		if (m_activeStage) {
			m_pTgtCommandList = pCommandList;
			m_activeStage->OnSceneDraw();
		}
	}

	void Scene::UpdateUI(std::unique_ptr<UILayer>& uiLayer) {
		vector<wstring> labels;
		{
			auto device = BaseDevice::GetBaseDevice();
			//1秒間に１回更新される安定したfpsを得る
			auto fps = device->GetStableFps();
			//1秒間に１回更新される安定したelapsedTimeを得る
			auto elapsedTime = device->GetStableElapsedTime();
			wstringstream wLabel;
			wLabel.precision(1);
			wLabel << fixed << L"FPS: " << fps
				<< L"\n";
			wLabel.precision(6);
			wLabel << L"ElapsedTime: " << elapsedTime
				<< L"\n";
			labels.push_back(wLabel.str());
		}

		wstring uiText = L"";
		for (auto s : labels)
		{
			uiText += s;
		}
		uiLayer->UpdateLabels(uiText);

	}


}
// end namespace basecross




