/*!
@file Scene.cpp
@brief シーンクラス　実体
*/


#include "stdafx.h"
#include "Project.h"


namespace basecross {

	using namespace std;
	using namespace SceneEnums;


	Scene::Scene(UINT frameCount, PrimDevice* pSample) :
		BaseScene(frameCount, pSample)
	{
	}

	Scene::~Scene()
	{
	}

	void Scene::CreateAssetResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
	{
		m_myCamera = shared_ptr<PerspecCamera>(new PerspecCamera());
		m_myLightSet = shared_ptr<LightSet>(new LightSet());

		TransParam param;
		param.scale = XMFLOAT3(1, 1, 1);
		param.rotOrigin = XMFLOAT3(0, 0, 0);
		auto quat = XMQuaternionIdentity();
		param.quaternion = bsm::makeF4(quat);
		param.position = XMFLOAT3(0.0, 5.0, 0.0f);
		AddMyObject<WallBox>(pCommandList, param);
		param.scale = XMFLOAT3(50, 1, 50);
		param.position = XMFLOAT3(0, -0.5, 0.0f);
		AddMyObject<SkyStage>(pCommandList, param);
	}



	void Scene::Update(double elapsedTime) {
		for (auto& v : m_myObjectvec) {
			v->OnUpdate(elapsedTime);
		}
		UpdateConstantBuffers();
		CommitConstantBuffers();
	}

	void Scene::UpdateConstantBuffers()
	{
		for (auto& v : m_myObjectvec) {
			v->UpdateConstantBuffers(this);
		}
	}

	void Scene::CommitConstantBuffers()
	{
		for (auto& v : m_myObjectvec) {
			v->CommitConstantBuffers(this);
		}
	}

	void Scene::ShadowPass(ID3D12GraphicsCommandList* pCommandList) {
		// Set necessary state.
		//basecross
		auto rootSignature = RootSignaturePool::GetRootSignature(L"BaseCrossDefault", true);
		pCommandList->SetGraphicsRootSignature(rootSignature.Get());
		ComPtr<ID3D12PipelineState> PNTShadowmapPipelineState
			= PipelineStatePool::GetPipelineState(L"PNTShadowmap", true);
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pCommandList->RSSetViewports(1, &m_viewport);
		pCommandList->RSSetScissorRects(1, &m_scissorRect);
		// No render target needed for the shadow pass.
		pCommandList->OMSetRenderTargets(0, nullptr, FALSE, &m_depthDsvs[DepthGenPass::Shadow]);
		//以下は各オブジェクト
		for (auto& v : m_myObjectvec) {
			v->OnShadowDraw(pCommandList);
		}
	}


	void Scene::ScenePass(ID3D12GraphicsCommandList* pCommandList)
	{
		auto& viewport = GetViewport();
		auto& scissorRect = GetScissorRect();
		auto depthDsvs = GetDepthDsvs();

		//basecross
		auto rootSignature = RootSignaturePool::GetRootSignature(L"BaseCrossDefault", true);
		pCommandList->SetGraphicsRootSignature(rootSignature.Get());

		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pCommandList->RSSetViewports(1, &viewport);
		pCommandList->RSSetScissorRects(1, &scissorRect);
		pCommandList->SetGraphicsRootDescriptorTable(GetGpuSlotID(L"t0"), m_depthSrvGpuHandles[DepthGenPass::Shadow]);        // Set the shadow texture as an SRV.
		pCommandList->OMSetRenderTargets(1, &GetCurrentBackBufferRtvCpuHandle(), FALSE, &depthDsvs[DepthGenPass::Scene]);
		//パイプラインステートを得る
		ComPtr<ID3D12PipelineState> defaultShadowPipelineState
			= PipelineStatePool::GetPipelineState(L"BcPNTStaticShadow", true);
		pCommandList->SetPipelineState(defaultShadowPipelineState.Get());
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

		for (auto& v : m_myObjectvec) {
			v->OnSceneDraw(pCommandList);
		}
	}

}
// end namespace basecross




