/*!
@file Scene.h
@brief シーンクラス
*/

#pragma once

#include "stdafx.h"

namespace basecross {


	class MyObject;
	class Stage;

	using namespace std;

	//--------------------------------------------------------------------------------------
	// シーン
	//--------------------------------------------------------------------------------------
	class Scene : public BaseScene
	{
		shared_ptr<Stage> m_activeStage;
	public:
		Scene(UINT frameCount, PrimDevice* pPrimDevice);
		virtual ~Scene();
	protected:
		virtual void CreateAssetResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)override{}
		virtual void CreatePipelineStates(ID3D12Device* pDevice)override{}
		virtual void Update(double elapsedTime)override {}
		virtual void UpdateConstantBuffers()override {}
		virtual void CommitConstantBuffers()override {}
		virtual void ShadowPass(ID3D12GraphicsCommandList* pCommandList)override{}
		virtual void ScenePass(ID3D12GraphicsCommandList* pCommandList)override{}
		virtual void UpdateUI(std::unique_ptr<UILayer>& uiLayer)override;
	};

}
//end namespace basecross

