/*!
@file GameStage.h
@brief ゲームステージクラス
*/


#pragma once


#pragma once
#include "stdafx.h"


namespace basecross {
	//--------------------------------------------------------------------------------------
	// ゲームステージ
	//--------------------------------------------------------------------------------------
	class GameStage : public Stage {
	protected:
	public:
		GameStage(ID3D12Device* pDevice) :
			Stage(pDevice)
		{
		}
		virtual ~GameStage() {}

		virtual void OnCreate()override;
	};




}
//end namespace basecross
