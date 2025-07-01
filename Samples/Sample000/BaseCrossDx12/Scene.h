/*!
@file Scene.h
@brief シーンクラス
*/
#pragma once
#include "stdafx.h"
#include "Project.h"

namespace basecross {
	//--------------------------------------------------------------------------------------
	// シーン
	//--------------------------------------------------------------------------------------
	class Scene : public BaseScene
	{
	public:
		Scene(UINT frameCount, PrimDevice* pPrimDevice);
		virtual ~Scene();
	protected:
		virtual void CreateAssetResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)override {}
		virtual void Update(double elapsedTime)override {}
		virtual void UpdateConstantBuffers()override {}
		virtual void CommitConstantBuffers()override {}
		virtual void UpdateUI(std::unique_ptr<UILayer>& uiLayer)override;
		virtual void ShadowPass(ID3D12GraphicsCommandList* pCommandList)override {}
		virtual void ScenePass(ID3D12GraphicsCommandList* pCommandList) {}
	};
}
//end namespace basecross
