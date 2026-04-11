/*!
@file SteeringBehavior.h
@brief ステアリング行動コンポーネント
*/
#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	 ステアリング行動コンポーネント（親）
	//--------------------------------------------------------------------------------------
	class SteeringBehavior : public Component {
	public:
		//計算結果が入るforce
		Vec3 m_force;
		//質量
		double m_mass;
		//最大フォース
		double m_max_force;
		//最大スピード
		double m_max_speed;
		//
		double m_turn_rate;
		//影響度
		float m_weight;
		//Steering処理による速度
		Vec3 m_velocity;
	protected:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	プロテクトコンストラクタ
		@param[in]	GameObjectPtr	ゲームオブジェクト
		*/
		//--------------------------------------------------------------------------------------
		explicit SteeringBehavior(const std::shared_ptr<GameObject>& GameObjectPtr);
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~SteeringBehavior();
		//--------------------------------------------------------------------------------------
		/*!
		@brief 更新処理
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate(double elapsedTime);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	フォースを加える
		@param[in,out]	RunningTot	加えられたフォース
		@param[in]	ForceToAdd	加えるフォース
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		bool AccumulateForce(Vec3& RunningTot,Vec3& ForceToAdd);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	フォースを計算してGameObjecの位置を変更する
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void ApplyForce();
		//--------------------------------------------------------------------------------------
		/*!
		@brief 仮想関数デフォルトは何も行わない
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCreate()override {}
		virtual void OnShadowDraw(ID3D12GraphicsCommandList* pCommandList)override {}
		virtual void OnSceneDraw(ID3D12GraphicsCommandList* pCommandList)override {}
		virtual void OnDestroy()override {}
	};

	//--------------------------------------------------------------------------------------
	///	 Seek
	//--------------------------------------------------------------------------------------
	class Seek : public SteeringBehavior {
	public:
		Seek(const std::shared_ptr<GameObject>& GameObjectPtr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	行動を実行する
		@param[in]	velocity	速度
		@param[in]	target	目的の場所
		@param[in]	pos	現在の自分の場所
		@return	掛けるべきフォース
		*/
		//--------------------------------------------------------------------------------------
		Vec3 Execute(const Vec3& velocity, const Vec3& target, const Vec3& pos);
	};

	//--------------------------------------------------------------------------------------
	///	 Arrive
	//--------------------------------------------------------------------------------------
	class Arrive : public SteeringBehavior {
		float m_Decl;
	public:
		Arrive(const std::shared_ptr<GameObject>& GameObjectPtr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	行動を実行する
		@param[in]	Force	現在のフォース
		@param[in]	Velocity	現在の速度
		@param[in]	TargetPos	到着する位置
		@return	掛けるべきフォース
		*/
		//--------------------------------------------------------------------------------------
		Vec3 Execute(const Vec3& Force, const Vec3& Velocity, const Vec3& TargetPos);
	};


	//--------------------------------------------------------------------------------------
	///	 Separation
	//--------------------------------------------------------------------------------------
	class Separation : public SteeringBehavior {
		std::weak_ptr<GameObjectGroup> m_group;

	public:
		Separation(const std::shared_ptr<GameObject>& GameObjectPtr);
		//アクセサ
		std::shared_ptr<GameObjectGroup> Separation::GetGameObjectGroup() const {
			auto shptr = m_group.lock();
			if (!shptr) {
				throw BaseException(
					L"グループは無効です",
					L"グループの存在を確認して下さい",
					L"Separation::GetGameObjectGroup()"
				);
			}
			return shptr;
		}
		void Separation::SetGameObjectGroup(const std::shared_ptr<GameObjectGroup>& Group) {
			m_group = Group;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	行動を実行する
		@param[in]	Force	現在のフォース
		@return	掛けるべきフォース
		*/
		//--------------------------------------------------------------------------------------
		Vec3 Execute(const Vec3& Force);

	};

}
//end namespace basecross

