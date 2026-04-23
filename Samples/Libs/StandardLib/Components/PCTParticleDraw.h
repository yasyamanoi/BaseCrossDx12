/*!
@file SpParticleDraw.h
@brief Simpleエフェクト描画
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	DECLARE_DX12SHADER(SpVSPCTParticle)
	DECLARE_DX12SHADER(SpPSPCTParticle)


	//--------------------------------------------------------------------------------------
	///	PCTParticleDraw描画コンポーネント
	//--------------------------------------------------------------------------------------
	class PCTParticleDraw : public Component {
		/// エミッシブ色
		Col4 m_emissive;
	protected:
		SimpleConstant m_simpleConstant;
		size_t m_constantIndex;
		//オリジナルメッシュ
		std::shared_ptr<BaseMesh> m_mesh;
	public:
		explicit PCTParticleDraw(const std::shared_ptr<GameObject>& gameObjectPtr, size_t MaxInstance, bool AddType) :
			Component(gameObjectPtr),
			m_emissive(0.0f)
		{
		}
		virtual ~PCTParticleDraw() {}

		Col4 GetEmissive()const {
			return m_emissive;
		}
		void SetEmissive(const Col4& col) {
			m_emissive = col;
		}
		virtual void OnCreate()override;
		//操作
		virtual void OnUpdate(double elapsedTime)override {}
		virtual void OnUpdateConstantBuffers()override;
		virtual void OnCommitConstantBuffers()override;
		virtual void OnSceneDraw(ID3D12GraphicsCommandList* pCommandList)override;
		virtual void OnDestroy()override {}
	};


}
// end namespace basecross
