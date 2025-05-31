/*!
@file Transform.h
@brief 座標変換コンポーネント
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
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
		bsm::Mat4x4 m_worldMatrix;
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
		bsm::Vec3 GetBeforeScale() const;
		bsm::Vec3 GetBeforePivot() const;
		bsm::Quat GetBeforeQuaternion() const;
		bsm::Vec3 GetBeforeRotation() const;
		bsm::Vec3 GetBeforePosition() const;
		bsm::Vec3 GetBeforeWorldPosition() const;
		bool IsSameBeforeWorldMatrix(const bsm::Mat4x4& mat) const;
		const bsm::Mat4x4 GetBeforeWorldMatrix() const;
		bsm::Vec3 GetScale() const;
		void SetScale(const bsm::Vec3& scale);
		void SetScale(float x, float y, float z);
		bsm::Vec3 GetRotOrigin() const;
		void SetRotOrigin(const bsm::Vec3& rotOrigin);
		void SetRotOrigin(float x, float y, float z);
		bsm::Quat GetQuaternion() const;
		void SetQuaternion(const bsm::Quat& qt);
		bsm::Vec3 GetRotation() const;
		void SetRotation(const bsm::Vec3& rot);
		void SetRotation(float x, float y, float z);
		bsm::Vec3 GetPosition() const;
		void AddPosition(const bsm::Vec3& addpos);
		void SetPosition(const bsm::Vec3& pos);
		void SetPosition(float x, float y, float z);
		void ResetPosition(const bsm::Vec3& pos);
		bsm::Vec3 GetWorldPosition();
		void SetWorldPosition(const bsm::Vec3& pos);
		void ResetWorldPosition(const bsm::Vec3& pos);
		bool IsSameWorldMatrix(const bsm::Mat4x4& mat);
		const bsm::Mat4x4& GetWorldMatrix();
		const bsm::Mat4x4& Get2DWorldMatrix();



		std::shared_ptr<GameObject> GetParent()const;
		void SetParent(const std::shared_ptr<GameObject>& obj);
		void ClearParent();
		bsm::Vec3 GetVelocity() const;
		bsm::Vec3 GetMovePositiom() const;
		float GetMoveSize() const;
		void SetToBefore();
		bsm::Vec3 GetForward();
		bsm::Vec3 GetUp();
		bsm::Vec3 GetRight();
	};


}
// end namespace basecross
