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

	void Scene::CreateAssetResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
	{
		//テクスチャ
		auto texFile = App::GetRelativeAssetsDir() + L"wall.jpg";
		auto texture = BaseTexture::CreateTextureFlomFile(pCommandList,texFile);
		RegisterTexture(L"WALL_TX", texture);
		texFile = App::GetRelativeAssetsDir() + L"sky.jpg";
		texture = BaseTexture::CreateTextureFlomFile(pCommandList, texFile);
		RegisterTexture(L"SKY_TX", texture);
		texFile = App::GetRelativeAssetsDir() + L"trace.png";
		texture = BaseTexture::CreateTextureFlomFile(pCommandList, texFile);
		RegisterTexture(L"TRACE_TX", texture);
		//ステージ作成
		ResetActiveStage<GameStage>(pDevice);
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


}
// end namespace basecross




