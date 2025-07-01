/*!
@file Scene.h
@brief シーンクラス
*/
#pragma once
#include "stdafx.h"
#include "Project.h"

namespace basecross {


	DECLARE_DX12SHADER(SpVSPCStatic)
	DECLARE_DX12SHADER(SpPSPCStatic)

	//--------------------------------------------------------------------------------------
	// シーン
	//--------------------------------------------------------------------------------------
	class Scene : public BaseScene
	{
		SimpleConstant m_constantBuffer;
		size_t m_constantBufferIndex;
		std::shared_ptr<BaseMesh> m_mesh;
		double m_totalTime;
		TransParam m_param;
		std::shared_ptr<Camera> m_camera;
		std::shared_ptr<LightSet> m_lightSet;
	public:
		Scene(UINT frameCount, PrimDevice* pPrimDevice);
		virtual ~Scene();
	protected:
		virtual void CreateAssetResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)override;
		virtual void Update(double elapsedTime)override;
		virtual void UpdateConstantBuffers()override;
		virtual void CommitConstantBuffers()override;
		virtual void UpdateUI(std::unique_ptr<UILayer>& uiLayer)override;
		virtual void ShadowPass(ID3D12GraphicsCommandList* pCommandList)override {}
		virtual void ScenePass(ID3D12GraphicsCommandList* pCommandList);
	};
}
//end namespace basecross
