/*!
@file InputDevice.h
@brief 入力デバイスクラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	/// XBoxコントローラーのステータス。XINPUT_STATE派生
	//--------------------------------------------------------------------------------------
	struct CONTROLER_STATE : public XINPUT_STATE
	{

		WORD wButtons;		///< ボタン状態
		BYTE bLeftTrigger;	///< 左トリガ
		BYTE bRightTrigger;	///< 右トリガ

		float fThumbRX;	///< 右スティックXのflort変換
		float fThumbRY;	///< 右スティックYのflort変換
		float fThumbLX;	///< 左スティックXのflort変換
		float fThumbLY;	///< 左スティックYのflort変換
		WORD wNowUpdateButtons;	///< 状態が変わったときにフラグが立つボタン
		WORD wPressedButtons;	///< 押された瞬間を所持するボタン
		WORD wReleasedButtons;	///< 離された瞬間を所持するボタン
		WORD wLastButtons;	///< 一つ前のボタン
		bool bConnected;	///< 接続しているかどうか
	};

	//--------------------------------------------------------------------------------------
	///	入力機器
	//--------------------------------------------------------------------------------------
	class InputDevice {
		const DWORD MAX_CONTROLLERS = 4; // コントローラ数
		std::vector<CONTROLER_STATE> m_State; // コントローラステートの配列
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief コンストラクタ
		*/
		//--------------------------------------------------------------------------------------
		InputDevice() :
			m_State(MAX_CONTROLLERS)
		{
			for (DWORD i = 0; i < MAX_CONTROLLERS; i++) {
				::ZeroMemory(&m_State[i], sizeof(CONTROLER_STATE));
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		~InputDevice() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief コントローラーの状態をm_Stateに取得する
		*/
		//--------------------------------------------------------------------------------------
		void ResetControlerState() {
			DWORD dwResult;
			for (DWORD i = 0; i < MAX_CONTROLLERS; i++)
			{
				//一つ前の状態を保存
				m_State[i].wLastButtons = m_State[i].Gamepad.wButtons;
				// XInputから現在のステータスを得る
				XINPUT_STATE State;
				dwResult = XInputGetState(i, &State);
				if (dwResult == ERROR_SUCCESS) {
					m_State[i].dwPacketNumber = State.dwPacketNumber;
					m_State[i].Gamepad = State.Gamepad;
					m_State[i].bConnected = true;
					//ボタンの設定
					//コピー
					m_State[i].wButtons = m_State[i].Gamepad.wButtons;
					//更新ボタン
					m_State[i].wNowUpdateButtons = m_State[i].Gamepad.wButtons;
					m_State[i].bLeftTrigger = m_State[i].Gamepad.bLeftTrigger;
					m_State[i].bRightTrigger = m_State[i].Gamepad.bRightTrigger;
					//前回より変化しているボタンのみフラグを立てる
					//XOR演算により実装
					m_State[i].wNowUpdateButtons ^= m_State[i].wLastButtons;
					//押された瞬間をセット
					m_State[i].wPressedButtons = m_State[i].wNowUpdateButtons & m_State[i].wButtons;
					//離された瞬間をセット
					m_State[i].wReleasedButtons = m_State[i].wNowUpdateButtons & (m_State[i].wButtons ^ 0xffff);
					//レバーのデッドゾーンの設定
					if ((m_State[i].Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
						m_State[i].Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
						(m_State[i].Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
							m_State[i].Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
					{
						m_State[i].Gamepad.sThumbLX = 0;
						m_State[i].Gamepad.sThumbLY = 0;
					}
					if (m_State[i].Gamepad.sThumbLX >= 0) {
						m_State[i].fThumbLX = (float)m_State[i].Gamepad.sThumbLX / (float)SHRT_MAX;
					}
					else {
						m_State[i].fThumbLX = (float)m_State[i].Gamepad.sThumbLX / (float)(SHRT_MAX + 1);
					}
					if (m_State[i].Gamepad.sThumbLY >= 0) {
						m_State[i].fThumbLY = (float)m_State[i].Gamepad.sThumbLY / (float)SHRT_MAX;
					}
					else {
						m_State[i].fThumbLY = (float)m_State[i].Gamepad.sThumbLY / (float)(SHRT_MAX + 1);
					}
					if ((m_State[i].Gamepad.sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
						m_State[i].Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
						(m_State[i].Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
							m_State[i].Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
					{
						m_State[i].Gamepad.sThumbRX = 0;
						m_State[i].Gamepad.sThumbRY = 0;
					}
					if (m_State[i].Gamepad.sThumbRX >= 0) {
						m_State[i].fThumbRX = (float)m_State[i].Gamepad.sThumbRX / (float)SHRT_MAX;
					}
					else {
						m_State[i].fThumbRX = (float)m_State[i].Gamepad.sThumbRX / (float)(SHRT_MAX + 1);
					}
					if (m_State[i].Gamepad.sThumbRY >= 0) {
						m_State[i].fThumbRY = (float)m_State[i].Gamepad.sThumbRY / (float)SHRT_MAX;
					}
					else {
						m_State[i].fThumbRY = (float)m_State[i].Gamepad.sThumbRY / (float)(SHRT_MAX + 1);
					}
				}
				else {
					m_State[i].bConnected = false;
				}
			}
		}
		//	アクセサ
		//--------------------------------------------------------------------------------------
		/*!
		@brief コントローラステートの取得
		@return	コントローラステート
		*/
		//--------------------------------------------------------------------------------------
		const std::vector<CONTROLER_STATE>& GetControlerVec()const {
			return m_State; 
		}
	};


}
// end namespace basecross
