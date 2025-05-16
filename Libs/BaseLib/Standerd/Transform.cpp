/*!
@file Transform.cpp
@brief 座標変換コンポーネント
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {

	Transform::Transform(const shared_ptr<GameObject>& gameObjectPtr, const TransParam& param):
		Component(gameObjectPtr),
		m_beforeParam(param),
		m_param(param),
		m_worldMatrix()
	{
	}

	Vec3 Transform::GetScale() const {
		return m_param.scale;
	}

	void Transform::SetScale(const Vec3& scale) {
		if (!scale.isNaN() && !scale.isInfinite()) {
			m_param.scale = scale;
		}
	}
	void Transform::SetScale(float x, float y, float z) {
		SetScale(Vec3(x, y, z));
	}

	Vec3 Transform::GetRotOrigin() const {
		return m_param.rotOrigin;
	}
	void Transform::SetRotOrigin(const Vec3& rotOrigin) {
		if (!rotOrigin.isNaN() && !rotOrigin.isInfinite()) {
			m_param.rotOrigin = rotOrigin;
		}
	}
	void Transform::SetRotOrigin(float x, float y, float z) {
		SetRotOrigin(Vec3(x, y, z));
	}

	Quat Transform::GetQuaternion() const {
		return m_param.quaternion;
	}
	void Transform::SetQuaternion(const bsm::Quat& quaternion) {
		if (!quaternion.isNaN() && !quaternion.isInfinite()) {
			m_param.quaternion = quaternion;
			m_param.quaternion.normalize();
		}
	}
	Vec3 Transform::GetRotation() const {
		Vec3 r = m_param.quaternion.toRotVec();
		r.normalize();
		return r;
	}

	void Transform::SetRotation(const Vec3& rot) {
		if (!rot.isNaN() && !rot.isInfinite()) {
			Quat qt;
			qt.rotationRollPitchYawFromVector(rot);
			SetQuaternion(qt);
		}
	}
	void Transform::SetRotation(float x, float y, float z) {
		SetRotation(Vec3(x, y, z));
	}

	void Transform::AddPosition(const Vec3& addpos) {
		if (!addpos.isNaN() && !addpos.isInfinite()) {
			m_param.position += addpos;
		}
	}


	Vec3 Transform::GetPosition() const {
		return m_param.position;
	}

	void Transform::SetPosition(const Vec3& position) {
		if (!position.isNaN() && !position.isInfinite()) {
			m_param.position = position;
		}
	}
	void Transform::SetPosition(float x, float y, float z) {
		SetPosition(Vec3(x, y, z));
	}

	void Transform::ResetPosition(const Vec3& position) {
		if (!position.isNaN() && !position.isInfinite()) {
			m_beforeParam.position = position;
			m_param.position = position;
		}
	}

	Vec3 Transform::GetWorldPosition() {
		return GetWorldMatrix().transInMatrix();
	}
	void Transform::SetWorldPosition(const Vec3& position) {
		auto SetPos = position;
		auto parPtr = GetParent();
		if (parPtr) {
			auto parWorldPos = parPtr->GetComponent<Transform>()->GetWorldMatrix().transInMatrix();
			SetPos -= parWorldPos;
			auto parQt = parPtr->GetComponent<Transform>()->GetWorldMatrix().quatInMatrix();
			parQt.inverse();
			Mat4x4 parQtMat(parQt);
			SetPos *= parQtMat;
		}
		SetPosition(SetPos);
	}
	void Transform::ResetWorldPosition(const Vec3& position) {
		auto setPos = position;
		auto parPtr = GetParent();
		if (parPtr) {
			auto parWorldPos = parPtr->GetComponent<Transform>()->GetWorldMatrix().transInMatrix();
			setPos -= parWorldPos;
			auto parQt = parPtr->GetComponent<Transform>()->GetWorldMatrix().quatInMatrix();
			parQt.inverse();
			Mat4x4 parQtMat(parQt);
			setPos *= parQtMat;
		}
		ResetPosition(setPos);
	}

	bool Transform::IsSameWorldMatrix(const bsm::Mat4x4& mat) {
		return mat.equalInt(GetWorldMatrix());
	}



	const Mat4x4& Transform::GetWorldMatrix() {
		auto parPtr = GetParent();
		//Mat4x4 WorldMat;
		m_worldMatrix.affineTransformation(
			m_param.scale,
			m_param.rotOrigin,
			m_param.quaternion,
			m_param.position
		);
		if (parPtr) {
			auto parWorld = parPtr->GetComponent<Transform>()->GetWorldMatrix();
			parWorld.scaleIdentity();
			m_worldMatrix *= parWorld;
		}
		return m_worldMatrix;
	}


	const Mat4x4& Transform::Get2DWorldMatrix() {
		auto parPtr = GetParent();
		m_param.scale.z = 1.0f;
		Vec4 temp_z(m_param.position.z);
		temp_z = XMVector4ClampLength(temp_z, 0.0f, 1.0f);
		m_param.position.z = temp_z.z;
		m_param.rotOrigin.z = 0;
		//Mat4x4 WorldMat;
		m_worldMatrix.affineTransformation(
			m_param.scale,
			m_param.rotOrigin,
			m_param.quaternion,
			m_param.position
		);
		if (parPtr) {
			auto parWorld = parPtr->GetComponent<Transform>()->Get2DWorldMatrix();
			parWorld.scaleIdentity();
			m_worldMatrix *= parWorld;
		}
		return m_worldMatrix;
	}




	shared_ptr<GameObject> Transform::GetParent()const {
		auto shPtr = m_parent.lock();
		if (shPtr) {
			return shPtr;
		}
		return nullptr;
	}
	void Transform::SetParent(const shared_ptr<GameObject>& Obj) {
		if (GetParent() == Obj) {
			return;
		}
		if (Obj) {
			ClearParent();
			m_parent = Obj;
			auto parWorld = Obj->GetComponent<Transform>()->GetWorldMatrix();
			parWorld.scaleIdentity();
			auto posSpan = GetPosition() - parWorld.transInMatrix();
			auto qtSpan = parWorld.quatInMatrix();
			qtSpan.inverse();
			Mat4x4 qarQtMat(qtSpan);
			posSpan *= qarQtMat;

			Mat4x4 Mat = GetWorldMatrix() * parWorld;
			Vec3 scale, pos;
			bsm::Quat qt;
			Mat.decompose(scale, qt, pos);
			SetScale(scale);
			SetQuaternion(qt);
			SetPosition(posSpan);
			SetToBefore();

		}
		else {
			//nullptrが渡された
			ClearParent();
		}
	}

	void Transform::ClearParent() {
		if (auto parPtr = GetParent()) {
			auto pos = GetWorldPosition();
			SetPosition(pos);
			SetToBefore();
		}
		m_parent.reset();
	}

	Vec3 Transform::GetMovePositiom() const {
		return m_param.position - m_beforeParam.position;
	}

	float Transform::GetMoveSize() const {
		Vec3 Move = m_param.position - m_beforeParam.position;
		return Move.length();
	}

	void Transform::SetToBefore() {
		m_beforeParam = m_param;
	}

	Vec3 Transform::GetForward() {
		Vec3 ret = GetWorldMatrix().rotZInMatrix();
		ret.normalize();
		return ret;
	}

	Vec3 Transform::GetUp() {
		Vec3 ret = GetWorldMatrix().rotYInMatrix();
		ret.normalize();
		return ret;
	}
	Vec3 Transform::GetRight() {
		Vec3 ret = GetWorldMatrix().rotXInMatrix();
		ret.normalize();
		return ret;
	}




}
// end namespace basecross
