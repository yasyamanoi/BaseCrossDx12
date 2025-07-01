/*!
@file Steering.h
@brief 操舵関連
*/


#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	class Path;
	//	用途: 経路クラス
	//--------------------------------------------------------------------------------------
	class Path {
		//経路のポイントのリスト
		std::list<Vec3> m_WayPoints;
		//現在イテレーター
		std::list<Vec3>::iterator m_CurWaypoint;
		//ループするかどうか
		bool m_Looped;
	public:
		//--------------------------------------------------------------------------------------
		//	Path();
		//	用途: コンストラクタ
		//	戻り値: なし
		//--------------------------------------------------------------------------------------
		Path() :
			m_Looped(false) {
		}
		//--------------------------------------------------------------------------------------
		//	virtual ~Path();
		//	用途: デストラクタ
		//	戻り値: なし
		//--------------------------------------------------------------------------------------
		virtual ~Path() {
			m_WayPoints.clear();
		}
		//--------------------------------------------------------------------------------------
		//アクセッサ
		//--------------------------------------------------------------------------------------
		bool GetLooped()const { return m_Looped; }
		std::list<Vec3>* GetPathPtr() { return &m_WayPoints; }
		Vec3 GetCurWaypoint()const {
			if (m_CurWaypoint == m_WayPoints.end()) {
				return m_WayPoints.back();
			}
			else {
				return *m_CurWaypoint;
			}
		}
		void SetLooped(bool b) { m_Looped = b; }
		//--------------------------------------------------------------------------------------
		//	bool IsFinished();
		//	用途: 最後まで行ってるかどうか
		//	戻り値: 最後まで行っていればtrue
		//--------------------------------------------------------------------------------------
		bool IsFinished() {
			return !(m_CurWaypoint != m_WayPoints.end());
		}
		//--------------------------------------------------------------------------------------
		//	void Clear();
		//	用途: リストをクリアする
		//	戻り値: なし
		//--------------------------------------------------------------------------------------
		void Clear() { m_WayPoints.clear(); }
		//--------------------------------------------------------------------------------------
		//	void SetList(
		//	const list<Vec3> new_path	//新しいリスト
		//	);
		//	用途: リストを差し替える
		//	戻り値: なし
		//--------------------------------------------------------------------------------------
		void SetList(const std::list<Vec3>& new_path) {
			Clear();
			auto it = new_path.begin();
			while (it != new_path.end()) {
				m_WayPoints.push_back(*it);
				it++;
			}
			m_CurWaypoint = m_WayPoints.begin();
		}
		//--------------------------------------------------------------------------------------
		//	void SetNextWaypoint();
		//	用途: 次のポイントに進める
		//	戻り値: なし
		//--------------------------------------------------------------------------------------
		void SetNextWaypoint() {
			if (m_WayPoints.size() <= 0) {
				throw BaseException(
					L"リストが空です。",
					L"if(m_WayPoints.size() <= 0)",
					L"Path::SetNextWaypoint()"
				);
			}
			if (m_CurWaypoint == m_WayPoints.end()) {
				return;
			}
			if (++m_CurWaypoint == m_WayPoints.end()) {
				if (m_Looped) {
					m_CurWaypoint = m_WayPoints.begin();
				}
			}
		}
	};




	//--------------------------------------------------------------------------------------
	//	struct Steering;
	//	用途: 操舵関連ユーティリティ
	//	＊static呼び出しをする
	//--------------------------------------------------------------------------------------
	struct Steering {
		Steering() {}
		~Steering() {}
		//--------------------------------------------------------------------------------------
		//	static  bool AccumulateForce(
		//	Vec3& Force,
		//	const Vec3& ForceToAdd,
		//	float MaxForce
		//	);
		//	用途: 操舵力を加算する
		//	戻り値: 加算されればtrue
		//	＊Forceに計算結果を加算する
		//--------------------------------------------------------------------------------------
		static bool AccumulateForce(Vec3& Force, const Vec3& ForceToAdd, float MaxForce);
		//--------------------------------------------------------------------------------------
		//	static Vec3 Seek(
		//	const Vec3& Velocity,	//現在の速度
		//	const Vec3& Target,		//目標
		//	const Vec3& Pos,			//現在の位置
		//	float MaxSpeed				//最高速度
		//	);
		//	用途: 探索行動
		//	戻り値: フォース
		//--------------------------------------------------------------------------------------
		static Vec3 Seek(const Vec3& Velocity, const Vec3& Target, const Vec3& Pos, float MaxSpeed);
		//--------------------------------------------------------------------------------------
		//	static Vec3 Flee(
		//	const Vec3& Velocity,	//現在の速度
		//	const Vec3& Target,		//目標
		//	const Vec3& Pos,			//現在の位置
		//	float MaxSpeed,				//最高速度
		//	float PanicDistance			//逃げ出す平方距離範囲の1辺
		//	);
		//	用途: 逃走行動
		//	戻り値: フォース
		//	＊ターゲットが PanicDistance * PanicDistanceの距離範囲に入ったら逃げ出す
		//--------------------------------------------------------------------------------------
		static Vec3 Flee(const Vec3& Velocity, const Vec3& Target,
			const Vec3& Pos, float MaxSpeed, float PanicDistance);
		//--------------------------------------------------------------------------------------
		//	static Vec3 Arrive(
		//	const Vec3& Velocity,	//現在の速度
		//	const Vec3& Target,		//目標
		//	const Vec3& Pos,			//現在の位置
		//	float MaxSpeed				//最高速度
		//	float Decl					//減速値（1.0から3.0の間程度）
		//	);
		//	用途: 到着行動
		//	戻り値: フォース
		//--------------------------------------------------------------------------------------
		static Vec3 Arrive(const Vec3& Velocity, const Vec3& Target,
			const Vec3& Pos, float MaxSpeed, float Decl);
		//--------------------------------------------------------------------------------------
		//	static Vec3 Pursuit(
		//	const Vec3& Velocity,	//現在の速度
		//	const Vec3& Pos,			//現在の位置
		//	const Vec3& Rot,			//現在の向き
		//	float MaxSpeed,				//最高速度
		//	const Vec3& TargetVelocity,	//目標の現在の速度
		//	const Vec3& Target,		//目標
		//	const Vec3& TargetRot	//目標の向き
		//	);
		//	用途: 追跡行動
		//	戻り値: フォース
		//--------------------------------------------------------------------------------------
		static Vec3 Pursuit(const Vec3& Velocity, const Vec3& Pos, const Vec3& Rot, float MaxSpeed,
			const Vec3& TargetVelocity, const Vec3& Target, const Vec3& TargetRot);
		//--------------------------------------------------------------------------------------
		//	static Vec3 Wander(
		//	const Mat4x4 Matrix,		//現在の行列
		//	float WanderRadius,	//徘徊する半径
		//	float WanderDistance,	//徘徊する半径までの距離
		//	float WanderJitter,	//ランダム変異の最大値
		//	Vec3& WanderTarget	//徘徊のターゲット変数（回転値）
		//	);
		//	用途: 徘徊行動
		//	戻り値: フォース
		//--------------------------------------------------------------------------------------
		static Vec3 Wander(const Mat4x4 Matrix,
			float WanderRadius, float WanderDistance, float WanderJitter, Vec3& WanderTarget);
		//--------------------------------------------------------------------------------------
		//	static Vec3 ObstacleAvoidance(
		//	const Mat4x4 Matrix,		//現在の行列
		//	const Vec3& Velocity,	//現在の速度
		//	float MaxSpeed,				//最高速度
		//	float Width,				//回避する道幅
		//	float Height,				//回避する高さ
		//	const vector<SPHERE>& SphereVec	//回避すべき球体の配列
		//	);
		//	用途: 障害物回避行動
		//	戻り値: フォース
		//--------------------------------------------------------------------------------------
		static Vec3 ObstacleAvoidance(const Mat4x4 Matrix,
			const Vec3& Velocity, float MaxSpeed, float Width, float Height,
			const std::vector<SPHERE>& SphereVec);
		//--------------------------------------------------------------------------------------
		//	static Vec3 WallAvoidance(
		//	const Mat4x4 Matrix,		//現在の行列
		//	const Vec3& Velocity,	//現在の速度
		//	float MaxSpeed,				//最高速度
		//	const vector<PLANE>& PlaneVec	//回避すべき球体の配列
		//	);
		//	用途: 壁回避行動
		//	戻り値: フォース
		//--------------------------------------------------------------------------------------
		static Vec3 WallAvoidance(const Mat4x4 Matrix,
			const Vec3& Velocity, float MaxSpeed, const std::vector<PLANE>& PlaneVec);
		//--------------------------------------------------------------------------------------
		//	static Vec3 FollowPath(
		//	Path& rPath,				//パス
		//	float WaypointSeekDist,		//Seek開始までの距離
		//	const Vec3& Pos,			//現在の位置
		//	const Vec3& Velocity,	//現在の速度
		//	float MaxSpeed,				//最高速度
		//	float Decl					//減速値（1.0から3.0の間程度）
		//	);
		//	用途: 経路行動
		//	戻り値: フォース
		//--------------------------------------------------------------------------------------
		static Vec3 FollowPath(Path& rPath,
			float WaypointSeekDist,
			const Vec3& Pos, const Vec3& Velocity,
			float MaxSpeed, float Decl);
		//--------------------------------------------------------------------------------------
		//	static Vec3 Separation(
		//	const shared_ptr<GameObjectGroup>& Group,	//設定するグループの配列
		//	const shared_ptr<GameObject>& MyObj				//自分自身
		//	);
		//	用途: 分離行動
		//	戻り値: フォース
		//--------------------------------------------------------------------------------------
		static Vec3 Separation(const std::shared_ptr<GameObjectGroup>& Group, const std::shared_ptr<GameObject>& MyObj);
		//--------------------------------------------------------------------------------------
		//	static Vec3 Alignment(
		//	const shared_ptr<GameObjectGroup>& Group,	//設定するグループの配列
		//	const shared_ptr<GameObject>& MyObj				//自分自身
		//	);
		//	用途: 整列行動
		//	戻り値: フォース
		//--------------------------------------------------------------------------------------
		static Vec3 Alignment(const std::shared_ptr<GameObjectGroup>& Group, const std::shared_ptr<GameObject>& MyObj);
		//--------------------------------------------------------------------------------------
		//	static Vec3 Cohesion(
		//	const shared_ptr<GameObjectGroup>& Group,	//設定するグループの配列
		//	const shared_ptr<GameObject>& MyObj			//自分自身
		//	const Vec3& Velocity,	//現在の速度
		//	float MaxSpeed				//最高速度
		//	);
		//	用途: 結合行動
		//	戻り値: フォース
		//--------------------------------------------------------------------------------------
		static Vec3 Cohesion(const std::shared_ptr<GameObjectGroup>& Group, const std::shared_ptr<GameObject>& MyObj,
			const Vec3& Velocity, float MaxSpeed);
	};

}
//end namespace basecross
