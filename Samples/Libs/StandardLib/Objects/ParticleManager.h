/*!
@file ParticleManager.h
@brief パーティクルマネージャクラス
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	DECLARE_DX12SHADER(SpVSPCTParticle)
	DECLARE_DX12SHADER(SpPSPCTParticle)

	//--------------------------------------------------------------------------------------
	//	パーティクルマネージャ
	//--------------------------------------------------------------------------------------
	class ParticleManager : public GameObject {
		//Particleのmap
		std::map< std::shared_ptr<Particle>, std::wstring> m_particleMap;
	public:
		explicit ParticleManager(const std::shared_ptr<Stage>& StagePtr);
		virtual ~ParticleManager();
		//Particleの追加
		void InsertParticle(const ParticleParem& paaram);
		//操作
		virtual void OnCreate() override {}
		virtual void OnUpdate(double elapsedTime) override {}
		virtual void OnUpdateConstantBuffers()override {}
		virtual void OnCommitConstantBuffers()override {}
		virtual void OnSceneDraw(ID3D12GraphicsCommandList* pCommandList)override {}
		virtual void OnDestroy()override {}

	};

}
// end namespace basecross
