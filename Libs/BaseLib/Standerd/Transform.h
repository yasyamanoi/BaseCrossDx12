/*!
@file Transform.h
@brief 座標変換コンポーネント
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	class GameObject;

	//--------------------------------------------------------------------------------------
	///	座標変換コンポーネント
	//--------------------------------------------------------------------------------------
	class Transform : public Component {
		//1つ前の変数
		Vec3 m_beforeScale;
		Vec3 m_beforePivot;
		Quat m_beforeQuaternion;
		Vec3 m_beforePosition;
		//現在の変数
		Vec3 m_scale;
		Vec3 m_pivot;
		Quat m_quaternion;
		Vec3 m_position;
		bsm::Mat4x4 m_worldMatrix;
		bool m_dirtyFlg;
		//親オブジェクト
		weak_ptr<GameObject> m_parent;
	public:
		explicit Transform(const shared_ptr<GameObject>& gameObjectPtr);
		virtual ~Transform();
		Vec3 GetBeforeScale() const;
		Vec3 GetBeforePivot() const;
		Quat GetBeforeQuaternion() const;
		Vec3 GetBeforeRotation() const;
		Vec3 GetBeforePosition() const;
		Vec3 GetBeforeWorldPosition() const;
		bool IsSameBeforeWorldMatrix(const Mat4x4& mat) const;
		const Mat4x4 GetBeforeWorldMatrix() const;
		Vec3 GetScale() const;
		void SetScale(const Vec3& scale);
		void SetScale(float x, float y, float z);
		Vec3 GetPivot() const;
		void SetPivot(const Vec3& pivot);
		void SetPivot(float x, float y, float z);
		Quat GetQuaternion() const;
		void SetQuaternion(const Quat& qt);
		Vec3 GetRotation() const;
		void SetRotation(const Vec3& rot);
		void SetRotation(float x, float y, float z);
		Vec3 GetPosition() const;
		void AddPosition(const Vec3& addpos);
		void SetPosition(const Vec3& pos);
		void SetPosition(float x, float y, float z);
		void ResetPosition(const Vec3& pos);
		Vec3 GetWorldPosition();
		void SetWorldPosition(const Vec3& pos);
		void ResetWorldPosition(const Vec3& pos);
		bool IsSameWorldMatrix(const Mat4x4& mat);
		const Mat4x4& GetWorldMatrix();
		const Mat4x4& Get2DWorldMatrix();
		shared_ptr<GameObject> GetParent()const;
		void SetParent(const shared_ptr<GameObject>& obj);
		void ClearParent();
		Vec3 GetVelocity() const;
		float GetMoveSize() const;
		void SetToBefore();
		Vec3 GetForward();
		Vec3 GetUp();
		Vec3 GetRight();
		//操作
		virtual void OnInit()override {}
		virtual void OnUpdate()override;
		virtual void OnRender()override {}
		virtual void OnDestroy()override {}

	};


}
// end basecross
