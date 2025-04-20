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
	}



	void Scene::Update(double elapsedTime) {
		UpdateConstantBuffers();
		CommitConstantBuffers();
	}

	void Scene::UpdateConstantBuffers()
	{
	}

	void Scene::CommitConstantBuffers()
	{
	}

	void Scene::ShadowPass(ID3D12GraphicsCommandList* pCommandList) {
	}


	void Scene::ScenePass(ID3D12GraphicsCommandList* pCommandList)
	{
	}


}
// end namespace basecross




