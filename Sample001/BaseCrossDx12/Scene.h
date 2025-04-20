/*!
@file Scene.h
@brief シーンクラス
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	class GameObject;

	using namespace std;
	using namespace SceneEnums;

	//--------------------------------------------------------------------------------------
	// シーン
	//--------------------------------------------------------------------------------------
	class Scene : public BaseScene
	{
		vector<shared_ptr<GameObject>> m_gameObjectvec;
	public:
		Scene(UINT frameCount, PrimDevice* pSample);
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
		virtual void UpdateConstantBuffers()override;
		virtual void CommitConstantBuffers()override;

		virtual void Update(double elapsedTime)override;

		virtual void ShadowPass(ID3D12GraphicsCommandList* pCommandList)override;
		virtual void ScenePass(ID3D12GraphicsCommandList* pCommandList)override;
	};

}
//end namespace basecross

