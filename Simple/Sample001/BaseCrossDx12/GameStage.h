/*!
@file GameStage.h
@brief ゲームステージクラス
*/


#pragma once


#pragma once
#include "stdafx.h"


namespace basecross {

	//--------------------------------------------------------------------------------------
	// 配置されるオブジェクトの親
	//--------------------------------------------------------------------------------------
	class GameStage : public Stage {
	protected:
		std::shared_ptr<PerspecCamera> m_myCamera;
		std::shared_ptr<LightSet> m_myLightSet;
	public:
		GameStage(ID3D12Device* pDevice):
			Stage(pDevice)
		{}
		virtual ~GameStage() {}
		//アクセサ
		std::shared_ptr<PerspecCamera> GetMyCamera() const {
			return m_myCamera;
		}
		std::shared_ptr<LightSet> GetLightSet() const {
			return m_myLightSet;
		}
		virtual void OnCreate()override;
	};



}
//end namespace basecross
