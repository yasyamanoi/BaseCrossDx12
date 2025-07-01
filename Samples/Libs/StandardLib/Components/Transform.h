/*!
@file Transform.h
@brief 座標変換コンポーネント
*/


#pragma once
#include "stdafx.h"


namespace basecross {

	//--------------------------------------------------------------------------------------
	///	座標変換コンポーネント
	//--------------------------------------------------------------------------------------
	class Transform : public Component {
	protected:
		TransParam m_beforeParam;
		TransParam m_param;
		Mat4x4 m_worldMatrix;
		//親オブジェクト
		std::weak_ptr<GameObject> m_parent;
	public:
		Transform(const std::shared_ptr<GameObject>& gameObjectPtr,const TransParam& param);
		virtual ~Transform(){}
		TransParam& GetTransParam(){
			return m_param;
		}
		void SetTransParam(const TransParam& param) {
			m_param = param;
		}
		TransParam& GetBeforeParam() {
			return m_beforeParam;
		}
		void SetBeforeParam(const TransParam& param){
			m_beforeParam = param;
		}
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
		Vec3 GetRotOrigin() const;
		void SetRotOrigin(const Vec3& rotOrigin);
		void SetRotOrigin(float x, float y, float z);
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



		std::shared_ptr<GameObject> GetParent()const;
		void SetParent(const std::shared_ptr<GameObject>& obj);
		void ClearParent();
		Vec3 GetVelocity() const;
		Vec3 GetMovePositiom() const;
		float GetMoveSize() const;
		void SetToBefore();
		Vec3 GetForward();
		Vec3 GetUp();
		Vec3 GetRight();
	};


}
// end namespace basecross
