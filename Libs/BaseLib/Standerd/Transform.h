/*!
@file Transform.h
@brief 座標変換コンポーネント
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"


namespace basecross {

	using namespace std;
	using namespace bsm;

	//--------------------------------------------------------------------------------------
	///	座標変換コンポーネント
	//--------------------------------------------------------------------------------------
	class Transform : public Component {
	protected:
		TransParam m_beforeParam;
		TransParam m_param;
		Mat4x4 m_worldMatrix;
		//親オブジェクト
		weak_ptr<GameObject> m_parent;
	public:
		Transform(const shared_ptr<GameObject>& gameObjectPtr,const TransParam& param);
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
		void SetBeforeParam(const TransParam& param) {
			m_beforeParam = param;
		}
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
		shared_ptr<GameObject> GetParent()const;
		void SetParent(const shared_ptr<GameObject>& obj);
		void ClearParent();
		Vec3 GetMovePositiom() const;
		float GetMoveSize() const;
		void SetToBefore();
		Vec3 GetForward();
		Vec3 GetUp();
		Vec3 GetRight();
	};


}
// end namespace basecross
