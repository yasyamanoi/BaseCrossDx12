/*!
@file Scene.cpp
@brief シーンクラス　実体
*/
#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// シーン
	//--------------------------------------------------------------------------------------
	Scene::Scene(UINT frameCount, PrimDevice* pPrimDevice) :
	BaseScene(frameCount, pPrimDevice)
	{
	}
	Scene::~Scene()
	{
	}

	void Scene::CreateAssetResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) {
		//カメラとライトの作成
		m_camera = ObjectFactory::Create<PerspecCamera>();
		m_camera->SetEye(Vec3(0, 5.0f, -10.0f));
		m_camera->SetAt(Vec3(0, 0, 0));
		m_lightSet = ObjectFactory::Create<LightSet>();
		//三角形の作成
		m_triangle = ObjectFactory::Create<Triangle>();
	}

	void Scene::Update(double elapsedTime) {
		m_triangle->OnUpdate(elapsedTime);
		UpdateConstantBuffers();
		CommitConstantBuffers();
	}

	void Scene::UpdateConstantBuffers() {
		m_triangle->OnUpdateConstantBuffers();
	}

	void Scene::CommitConstantBuffers() {
		m_triangle->OnCommitConstantBuffers();
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

	void Scene::ScenePass(ID3D12GraphicsCommandList* pCommandList)
	{
		//ViewportsとScissorRectsの設定
		auto& viewport = GetViewport();
		auto& scissorRect = GetScissorRect();
		pCommandList->RSSetViewports(1, &viewport);
		pCommandList->RSSetScissorRects(1, &scissorRect);
		//ルート認証（RootSignature）の設定
		auto rootSignature = RootSignaturePool::GetRootSignature(L"BaseCrossDefault", true);
		pCommandList->SetGraphicsRootSignature(rootSignature.Get());
		auto depthDsvs = GetDepthDsvs();
		//RenderTargetsの設定
		pCommandList->OMSetRenderTargets(1, &GetCurrentBackBufferRtvCpuHandle(), FALSE, &depthDsvs[SceneEnums::DepthGenPass::Scene]);
		m_triangle->OnSceneDraw(pCommandList);
	}

}
// end namespace basecross
