/*!
@file GameStage.h
@brief �Q�[���X�e�[�W�N���X
*/


#pragma once


#pragma once
#include "stdafx.h"


namespace basecross {

	//--------------------------------------------------------------------------------------
	// �Q�[���X�e�[�W
	//--------------------------------------------------------------------------------------
	class GameStage : public Stage {
	protected:
	public:
		GameStage(ID3D12Device* pDevice):
			Stage(pDevice)
		{}
		virtual ~GameStage() {}

		//�ǂ�������I�u�W�F�N�g�̍쐬
		void CreateSeekObject();

		virtual void OnCreate()override;
	};



}
//end namespace basecross
