/*!
@file Scene.cpp
@brief �V�[���N���X�@����
*/
#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// �V�[��
	//--------------------------------------------------------------------------------------
	Scene::Scene(UINT frameCount, PrimDevice* pPrimDevice) :
		BaseScene(frameCount, pPrimDevice)
	{
	}
	Scene::~Scene()
	{
	}
	void Scene::UpdateUI(std::unique_ptr<UILayer>& uiLayer) {
		auto device = BaseDevice::GetBaseDevice();
		//1�b�ԂɂP��X�V�������肵��fps�𓾂�
		auto fps = device->GetStableFps();
		//1�b�ԂɂP��X�V�������肵��elapsedTime�𓾂�
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

