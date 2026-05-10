/*!
@file ParticleManager.cpp
@brief パーティクルマネージャクラス　実体
*/


#include "stdafx.h"

namespace basecross {

	IMPLEMENT_DX12SHADER(SpVSPCTParticle, App::GetShadersDir() + L"SpVSPCTParticle.cso")
	IMPLEMENT_DX12SHADER(SpPSPCTParticle, App::GetShadersDir() + L"SpPSPCTParticle.cso")


	ParticleManager::ParticleManager(const std::shared_ptr<Stage>& StagePtr) :
		GameObject(StagePtr) {

	}
	ParticleManager::~ParticleManager() {}

	//Particleの追加
	void InsertParticle(const ParticleParem& paaram) {

	}

}
// end namespace basecross
