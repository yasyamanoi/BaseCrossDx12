/*!
@file ProjectUtil.h
@brief �v���W�F�N��`�̃��[�e�B���e�B�N���X
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	�R���g���[���̃{�^���̃n���h��
	//--------------------------------------------------------------------------------------
	template<typename T>
	struct InputHandler {
		void PushHandle(const std::shared_ptr<T>& Obj) {
			//�R���g���[���̎擾
			auto cntlVec = App::GetInputDevice().GetControlerVec();
			if (cntlVec[0].bConnected) {
				//A�{�^��
				if (cntlVec[0].wPressedButtons & XINPUT_GAMEPAD_A) {
					Obj->OnPushA();
				}
				//B�{�^��
				if (cntlVec[0].wPressedButtons & XINPUT_GAMEPAD_B) {
					Obj->OnPushB();
				}
			}
		}
	};
}

//end namespace basecross

