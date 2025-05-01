/*!
@file Scene.cpp
@brief シーンクラス　実体
*/


#include "stdafx.h"
#include "Project.h"


namespace basecross {

	using namespace std;

	Scene::Scene(UINT frameCount, PrimDevice* pPrimDevice) :
		BaseScene(frameCount, pPrimDevice)
	{
	}

	Scene::~Scene()
	{
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




