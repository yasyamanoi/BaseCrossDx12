/*!
@file GameStage.h
@brief ゲームステージクラス
*/


#pragma once


#pragma once
#include "stdafx.h"


namespace basecross {

	using namespace std;
	using namespace bsm;

	//--------------------------------------------------------------------------------------
	// 配置されるオブジェクトの親
	//--------------------------------------------------------------------------------------
	class GameStage : public Stage {
	protected:
		shared_ptr<PerspecCamera> m_myCamera;
		shared_ptr<LightSet> m_myLightSet;
	public:
		GameStage(ID3D12Device* pDevice):
			Stage(pDevice)
		{}
		virtual ~GameStage() {}
		//アクセサ
		shared_ptr<PerspecCamera> GetMyCamera() const {
			return m_myCamera;
		}
		shared_ptr<LightSet> GetLightSet() const {
			return m_myLightSet;
		}
		virtual void OnCreate()override;
	};



}
//end namespace basecross
