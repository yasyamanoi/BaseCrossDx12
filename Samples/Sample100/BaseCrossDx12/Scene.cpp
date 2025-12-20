/*!
@file Scene.cpp
@brief シーンクラス　実体
*/
#include "stdafx.h"
#include "Project.h"

namespace basecross {


	using namespace SceneEnums;

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

	void Scene::CreateAssetResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
	{
		//ステージ作成
		ResetActiveStage<GameStage>(pDevice);
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

	void Scene::Update(double elapsedTime) {
		s_elapsedTime = elapsedTime;
		if (m_activeStage) {
			m_activeStage->UpdateStage();
			UpdateConstantBuffers();
			CommitConstantBuffers();

		}
	}


	void Scene::ShadowPass(ID3D12GraphicsCommandList* pCommandList) {
		if (m_activeStage) {
			m_pTgtCommandList = pCommandList;
			m_activeStage->OnShadowDraw(pCommandList);
		}
	}

	void Scene::ScenePass(ID3D12GraphicsCommandList* pCommandList)
	{
		if (m_activeStage) {
			m_pTgtCommandList = pCommandList;
			m_activeStage->OnSceneDraw(pCommandList);
		}
	}


}
// end namespace basecross




