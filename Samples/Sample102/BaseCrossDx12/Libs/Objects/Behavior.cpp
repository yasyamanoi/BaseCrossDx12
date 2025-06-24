/*!
@file Behavior.cpp
@brief 行動クラス実体
*/

#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	///	行動クラスの親クラス
	//--------------------------------------------------------------------------------------
	Behavior::Behavior(const std::shared_ptr<GameObject>& GameObjectPtr) :
		m_GameObject(GameObjectPtr)
	{
	}
	Behavior::~Behavior() {}
	std::shared_ptr<GameObject> Behavior::GetGameObject() const {
		auto shptr = m_GameObject.lock();
		if (!shptr) {
			throw BaseException(
				L"GameObjectは有効ではありません",
				L"if (!shptr)",
				L"Behavior::GetGameObject()"
			);
		}
		else {
			return shptr;
		}
	}
	std::shared_ptr<Stage> Behavior::GetStage() const {
		return GetGameObject()->GetStage();
	}

	//--------------------------------------------------------------------------------------
	///	行動ユーティリティクラス
	//--------------------------------------------------------------------------------------
	//進行方向を向くようにする
	void UtilBehavior::RotToHead(float LerpFact) {
		if (LerpFact <= 0.0f) {
			//補間係数が0以下なら何もしない
			return;
		}
		//回転の更新
		//Velocityの値で、回転を変更する
		//これで進行方向を向くようになる
		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		Vec3 Velocity = PtrTransform->GetVelocity();
		if (Velocity.length() > 0.0f) {
			Vec3 Temp = Velocity;
			Temp.normalize();
			float ToAngle = atan2(Temp.x, Temp.z);
			Quat Qt;
			Qt.rotationRollPitchYawFromVector(Vec3(0, ToAngle, 0));
			Qt.normalize();
			//現在の回転を取得
			Quat NowQt = PtrTransform->GetQuaternion();
			//現在と目標を補間
			//現在と目標を補間
			if (LerpFact >= 1.0f) {
				NowQt = Qt;
			}
			else {
				NowQt = XMQuaternionSlerp(NowQt, Qt, LerpFact);
			}
			PtrTransform->SetQuaternion(NowQt);
		}
	}

	void UtilBehavior::RotToHead(const Vec3& Velocity, float LerpFact) {
		if (LerpFact <= 0.0f) {
			//補間係数が0以下なら何もしない
			return;
		}
		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		//回転の更新
		if (Velocity.length() > 0.0f) {
			Vec3 Temp = Velocity;
			Temp.normalize();
			float ToAngle = atan2(Temp.x, Temp.z);
			Quat Qt;
			Qt.rotationRollPitchYawFromVector(Vec3(0, ToAngle, 0));
			Qt.normalize();
			//現在の回転を取得
			Quat NowQt = PtrTransform->GetQuaternion();
			//現在と目標を補間
			if (LerpFact >= 1.0f) {
				NowQt = Qt;
			}
			else {
				NowQt = XMQuaternionSlerp(NowQt, Qt, LerpFact);
			}
			PtrTransform->SetQuaternion(NowQt);
		}
	}
}
//end namespace basecross
