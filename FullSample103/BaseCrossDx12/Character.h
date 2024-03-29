/*!
@file Character.h
@brief キャラクタークラス
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	class FixedBox : public GameObject;
	//--------------------------------------------------------------------------------------
	class FixedBox : public GameObject {
		Vec3 m_Scale;
		Vec3 m_Rotation;
		Vec3 m_Position;
	public:
		//構築と破棄
		FixedBox(const shared_ptr<Stage>& StagePtr,
			const Vec3& Scale,
			const Vec3& Rotation,
			const Vec3& Position
		);
		virtual ~FixedBox();
		//初期化
		virtual void OnCreate() override;
		//操作
	};

	//--------------------------------------------------------------------------------------
	///	敵の親
	//--------------------------------------------------------------------------------------
	class EnemyBase : public GameObject {
		//基本の大きさ
		Vec3 m_ScaleBase;
		//スタート位置
		Vec3 m_StartPos;
		//フォース
		Vec3 m_Force;
		//速度
		Vec3 m_Velocity;
	protected:
		//構築と破棄
		//--------------------------------------------------------------------------------------
		/*!
		@brief	プロテクトコンストラクタ
		@param[in]	StagePtr	ステージ
		*/
		//--------------------------------------------------------------------------------------
		EnemyBase(const shared_ptr<Stage>& StagePtr, const Vec3& StartPos);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	プロテクトデストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~EnemyBase() {}
	public:
		//アクセサ
		const Vec3& GetForce()const {
			return m_Force;
		}
		void SetForce(const Vec3& f) {
			m_Force = f;
		}
		void AddForce(const Vec3& f) {
			m_Force += f;
		}
		const Vec3& GetVelocity()const {
			return m_Velocity;
		}
		void SetVelocity(const Vec3& v) {
			m_Velocity = v;
		}
		void ApplyForce();
		Vec3 GetTargetPos()const;
		//初期化
		virtual void OnCreate() override;
		//更新
		virtual void OnUpdate() override;
	};



	//--------------------------------------------------------------------------------------
	///	敵１
	//--------------------------------------------------------------------------------------
	class Enemy1 : public EnemyBase {
		//大きさ
		Vec3 m_Scale;
		//ステートマシーン
		unique_ptr<StateMachine<Enemy1>>  m_StateMachine;
		//NearとFarを切り替える値
		float m_StateChangeSize;
	public:
		//構築と破棄
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		@param[in]	StagePtr	ステージ
		*/
		//--------------------------------------------------------------------------------------
		Enemy1(const shared_ptr<Stage>& StagePtr, const Vec3& StartPos);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~Enemy1() {}
		//アクセサ
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ステートマシンを得る
		@return	ステートマシン
		*/
		//--------------------------------------------------------------------------------------
		unique_ptr< StateMachine<Enemy1>>& GetStateMachine() {
			return m_StateMachine;
		}
		float GetStateChangeSize() const {
			return m_StateChangeSize;
		}
		//初期化
		virtual void OnCreate() override;
		//更新
		virtual void OnUpdate() override;
	};


	//--------------------------------------------------------------------------------------
	///	Enemy1のFarステート
	//--------------------------------------------------------------------------------------
	class Enemy1FarState : public ObjState<Enemy1>
	{
		Enemy1FarState() {}
	public:
		//ステートのインスタンス取得
		DECLARE_SINGLETON_INSTANCE(Enemy1FarState)
		virtual void Enter(const shared_ptr<Enemy1>& Obj)override;
		virtual void Execute(const shared_ptr<Enemy1>& Obj)override;
		virtual void Exit(const shared_ptr<Enemy1>& Obj)override;
	};

	//--------------------------------------------------------------------------------------
	///	Enemy1のNearステート
	//--------------------------------------------------------------------------------------
	class Enemy1NearState : public ObjState<Enemy1>
	{
		Enemy1NearState() {}
	public:
		//ステートのインスタンス取得
		DECLARE_SINGLETON_INSTANCE(Enemy1NearState)
		virtual void Enter(const shared_ptr<Enemy1>& Obj)override;
		virtual void Execute(const shared_ptr<Enemy1>& Obj)override;
		virtual void Exit(const shared_ptr<Enemy1>& Obj)override;
	};


	//--------------------------------------------------------------------------------------
	///	敵2
	//--------------------------------------------------------------------------------------
	class Enemy2 : public EnemyBase {
		//ステートマシーン
		unique_ptr<StateMachine<Enemy2>>  m_StateMachine;
		//longとmediumを切り替える値
		const float m_LongMediumChange;
		//mediumとshortを切り替える値
		const float m_MediumShortChange;
	public:
		//構築と破棄
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		@param[in]	StagePtr	ステージ
		*/
		//--------------------------------------------------------------------------------------
		Enemy2(const shared_ptr<Stage>& StagePtr, const Vec3& StartPos);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~Enemy2() {}
		//アクセサ
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ステートマシンを得る
		@return	ステートマシン
		*/
		//--------------------------------------------------------------------------------------
		unique_ptr< StateMachine<Enemy2>>& GetStateMachine() {
			return m_StateMachine;
		}
		float GetLongMediumChange() const {
			return m_LongMediumChange;
		}
		float GetMediumShortChange() const {
			return m_MediumShortChange;

		}
		//初期化
		virtual void OnCreate() override;
		//更新
		virtual void OnUpdate() override;
		//衝突時
		virtual void OnCollisionEnter(shared_ptr<GameObject>& Other) override;
	};


	//--------------------------------------------------------------------------------------
	///	Enemy2のlongステート
	//--------------------------------------------------------------------------------------
	class Enemy2longState : public ObjState<Enemy2>
	{
		Enemy2longState() {}
	public:
		//ステートのインスタンス取得
		DECLARE_SINGLETON_INSTANCE(Enemy2longState)
		virtual void Enter(const shared_ptr<Enemy2>& Obj)override;
		virtual void Execute(const shared_ptr<Enemy2>& Obj)override;
		virtual void Exit(const shared_ptr<Enemy2>& Obj)override;
	};


	//--------------------------------------------------------------------------------------
	///	Enemy2のMediumステート
	//--------------------------------------------------------------------------------------
	class Enemy2MediumState : public ObjState<Enemy2>
	{
		Enemy2MediumState() {}
	public:
		//ステートのインスタンス取得
		DECLARE_SINGLETON_INSTANCE(Enemy2MediumState)
		virtual void Enter(const shared_ptr<Enemy2>& Obj)override;
		virtual void Execute(const shared_ptr<Enemy2>& Obj)override;
		virtual void Exit(const shared_ptr<Enemy2>& Obj)override;
	};

	//--------------------------------------------------------------------------------------
	///	Enemy2のShortステート
	//--------------------------------------------------------------------------------------
	class Enemy2ShortState : public ObjState<Enemy2>
	{
		Enemy2ShortState() {}
	public:
		//ステートのインスタンス取得
		DECLARE_SINGLETON_INSTANCE(Enemy2ShortState)
		virtual void Enter(const shared_ptr<Enemy2>& Obj)override;
		virtual void Execute(const shared_ptr<Enemy2>& Obj)override;
		virtual void Exit(const shared_ptr<Enemy2>& Obj)override;
	};

	//--------------------------------------------------------------------------------------
	///	敵３
	//--------------------------------------------------------------------------------------
	class Enemy3 : public EnemyBase {
		//ステートマシーン
		unique_ptr<StateMachine<Enemy3>>  m_StateMachine;
		//defaultとnearを切り替える値
		const float m_DefaultNearChange;
	public:
		//構築と破棄
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		@param[in]	StagePtr	ステージ
		*/
		//--------------------------------------------------------------------------------------
		Enemy3(const shared_ptr<Stage>& StagePtr, const Vec3& StartPos);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~Enemy3() {}
		//アクセサ
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ステートマシンを得る
		@return	ステートマシン
		*/
		//--------------------------------------------------------------------------------------
		unique_ptr< StateMachine<Enemy3>>& GetStateMachine() {
			return m_StateMachine;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	DefaultとNearの境値を得る
		@return	DefaultとNearの境値
		*/
		//--------------------------------------------------------------------------------------
		float GetDefaultNearChange() const {
			return m_DefaultNearChange;

		}
		//初期化
		virtual void OnCreate() override;
		//更新
		virtual void OnUpdate() override;
	};

	//--------------------------------------------------------------------------------------
	///	Enemy3のDefaultステート
	//--------------------------------------------------------------------------------------
	class Enemy3DefaultState : public ObjState<Enemy3>
	{
		Enemy3DefaultState() {}
	public:
		//ステートのインスタンス取得
		DECLARE_SINGLETON_INSTANCE(Enemy3DefaultState)
		virtual void Enter(const shared_ptr<Enemy3>& Obj)override;
		virtual void Execute(const shared_ptr<Enemy3>& Obj)override;
		virtual void Exit(const shared_ptr<Enemy3>& Obj)override;
	};

	//--------------------------------------------------------------------------------------
	///	Enemy3のNearステート
	//--------------------------------------------------------------------------------------
	class Enemy3NearState : public ObjState<Enemy3>
	{
		Enemy3NearState() {}
	public:
		//ステートのインスタンス取得
		DECLARE_SINGLETON_INSTANCE(Enemy3NearState)
		virtual void Enter(const shared_ptr<Enemy3>& Obj)override;
		virtual void Execute(const shared_ptr<Enemy3>& Obj)override;
		virtual void Exit(const shared_ptr<Enemy3>& Obj)override;
	};



}
//end namespace basecross
