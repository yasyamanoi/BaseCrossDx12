/*!
@file Scene.h
@brief シーンクラス
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	DECLARE_DX12SHADER(BcVSPNTStaticPL)
	DECLARE_DX12SHADER(BcPSPNTPL)
	DECLARE_DX12SHADER(BcVSPNTStaticPLShadow)
	DECLARE_DX12SHADER(BcPSPNTPLShadow)
	DECLARE_DX12SHADER(PNTShadowmap)

	class MyObject;
	class Stage;


	//--------------------------------------------------------------------------------------
	// シーン
	//--------------------------------------------------------------------------------------
	class Scene : public BaseScene
	{
		std::shared_ptr<Stage> m_activeStage;
	public:
		Scene(UINT frameCount, PrimDevice* pPrimDevice);
		virtual ~Scene();
		std::shared_ptr<Stage> GetActiveStage(bool ExceptionActive = true) const;

		template<typename T, typename... Ts>
		std::shared_ptr<T> ResetActiveStage(Ts&&... params) {
			auto actStagePtr = GetActiveStage(false);
			if (actStagePtr) {
				//破棄を伝える
				actStagePtr->OnDestroy();
				actStagePtr = nullptr;
			}
			auto ptr = ObjectFactory::Create<T>(params...);
			auto stagePtr = std::dynamic_pointer_cast<Stage>(ptr);
			if (!stagePtr) {
				throw BaseException(
					L"以下はStageに型キャストできません。",
					Util::GetWSTypeName<T>(),
					L"Scene::ResetActiveStage<T>()"
				);
			}
			SetActiveStage(stagePtr);
			return ptr;
		}
	protected:
		void SetActiveStage(const std::shared_ptr<Stage>& stage) {
			m_activeStage = stage;
		}
		virtual void CreateAssetResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)override;
		virtual void CreatePipelineStates(ID3D12Device* pDevice)override;
		virtual void UpdateConstantBuffers()override;
		virtual void CommitConstantBuffers()override;
		virtual void Update(double elapsedTime)override;
		virtual void ShadowPass(ID3D12GraphicsCommandList* pCommandList)override;
		virtual void ScenePass(ID3D12GraphicsCommandList* pCommandList)override;
		virtual void UpdateUI(std::unique_ptr<UILayer>& uiLayer)override;
	};

}
//end namespace basecross

