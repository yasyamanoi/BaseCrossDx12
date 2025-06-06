/*!
@file Gravity.h
@brief 重力コンポーネント
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/
#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	 重力コンポーネント
	//--------------------------------------------------------------------------------------
	class Gravity : public Component {
		bsm::Vec3 m_Gravity;
		bsm::Vec3 m_GravityVelocity;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		@param[in]	GameObjectPtr	ゲームオブジェクト
		@param[in]	gravity	重力加速度
		*/
		//--------------------------------------------------------------------------------------
		explicit Gravity(const shared_ptr<GameObject>& GameObjectPtr,
			const bsm::Vec3& gravity = bsm::Vec3(0, -9.8f, 0));
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~Gravity();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	重力加速度を得る
		@return	重力加速度
		*/
		//--------------------------------------------------------------------------------------
		bsm::Vec3 GetGravity() const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	重力加速度を設定する
		@param[in]	gravity	重力加速度
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetGravity(const bsm::Vec3& gravity);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	重力加速度を0にする
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetGravityZero();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	現在の重力加速度による速度を得る
		@return	重力加速度による速度
		*/
		//--------------------------------------------------------------------------------------
		bsm::Vec3 GetGravityVelocity() const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	重力加速度による速度をを設定する
		@param[in]	GravityVerocity	重力加速度による速度
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetGravityVerocity(const bsm::Vec3& GravityVerocity);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	重力加速度による速度を0にする
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetGravityVerocityZero();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ジャンプする
		@param[in]	JumpVerocity	ジャンプによるによる初速度
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void StartJump(const bsm::Vec3& JumpVerocity);
		//--------------------------------------------------------------------------------------
		/*!
		@brief 更新処理
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate()override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief 仮想関数デフォルトは何も行わない
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCreate()override {}
		virtual void OnDraw()override {}
		virtual void OnDestroy()override {}
	};

}
//end 