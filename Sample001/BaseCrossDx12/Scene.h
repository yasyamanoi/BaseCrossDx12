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


	class MyObject;

	using namespace std;
//	using namespace SceneEnums;

	//--------------------------------------------------------------------------------------
	// シーン
	//--------------------------------------------------------------------------------------
	class Scene : public BaseScene
	{
		vector<shared_ptr<GameObject>> m_gameObjectvec;
	public:
		Scene(UINT frameCount, PrimDevice* pPrimDevice);
		virtual ~Scene();
		template<typename T, typename... Ts>
		shared_ptr<T> AddGameObject(ID3D12GraphicsCommandList* pCommandList, Ts&&... params) {
			try {
				auto Ptr = ObjectFactory::Create<T>(pCommandList, params...);
				m_gameObjectvec.push_back(Ptr);
				return Ptr;
			}
			catch (...) {
				throw;
			}
		}

		//アクセサ
		std::shared_ptr<PerspecCamera> GetMyCamera() const {
			return m_myCamera;
		}
		std::shared_ptr<LightSet> GetLightSet() const {
			return m_myLightSet;
		}
	protected:
		std::shared_ptr<PerspecCamera> m_myCamera;
		std::shared_ptr<LightSet> m_myLightSet;

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

