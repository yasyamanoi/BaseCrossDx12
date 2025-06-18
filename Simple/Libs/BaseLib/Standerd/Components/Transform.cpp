/*!
@file Transform.cpp
@brief 座標変換コンポーネント
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {

	Transform::Transform(const std::shared_ptr<GameObject>& gameObjectPtr, const TransParam& param):
		Component(gameObjectPtr),
		m_beforeParam(param),
		m_param(param),
		m_worldMatrix()
	{
	}

	//Get&Set	Before
	bsm::Vec3 Transform::GetBeforeScale() const {
		return m_beforeParam.scale;
	}

	bsm::Vec3 Transform::GetBeforePivot() const {
		return m_beforeParam.rotOrigin;
	}

	bsm::Quat Transform::GetBeforeQuaternion() const {
		return m_beforeParam.quaternion;
	}

	bsm::Vec3 Transform::GetBeforeRotation() const {
		return m_beforeParam.quaternion.toRotVec();
	}

	bsm::Vec3 Transform::GetBeforePosition() const {
		return m_beforeParam.position;
	}

	bsm::Vec3 Transform::GetBeforeWorldPosition() const {
		return GetBeforeWorldMatrix().transInMatrix();
	}

	bool Transform::IsSameBeforeWorldMatrix(const bsm::Mat4x4& mat) const {
		return mat.equalInt(GetBeforeWorldMatrix());
	}


	const bsm::Mat4x4 Transform::GetBeforeWorldMatrix() const {
		auto ParPtr = GetParent();
		bsm::Mat4x4 BefWorld;
		BefWorld.affineTransformation(
			m_beforeParam.scale,
			m_beforeParam.rotOrigin,
			m_beforeParam.quaternion,
			m_beforeParam.position
		);
		if (ParPtr) {
			auto ParBeforeWorld = ParPtr->GetComponent<Transform>()->GetBeforeWorldMatrix();
			ParBeforeWorld.scaleIdentity();
			BefWorld *= ParBeforeWorld;
		}
		return BefWorld;
	}

	//Get&Set
	bsm::Vec3 Transform::GetScale() const {
		return m_param.scale;
	}

	void Transform::SetScale(const bsm::Vec3& scale) {
		if (!scale.isNaN() && !scale.isInfinite()) {
			m_param.scale = scale;
		}
	}
	void Transform::SetScale(float x, float y, float z) {
		SetScale(bsm::Vec3(x, y, z));
	}

	bsm::Vec3 Transform::GetRotOrigin() const {
		return m_param.rotOrigin;
	}
	void Transform::SetRotOrigin(const bsm::Vec3& rotOrigin) {
		if (!rotOrigin.isNaN() && !rotOrigin.isInfinite()) {
			m_param.rotOrigin = rotOrigin;
		}
	}
	void Transform::SetRotOrigin(float x, float y, float z) {
		SetRotOrigin(bsm::Vec3(x, y, z));
	}

	bsm::Quat Transform::GetQuaternion() const {
		return m_param.quaternion;
	}
	void Transform::SetQuaternion(const bsm::Quat& quaternion) {
		if (!quaternion.isNaN() && !quaternion.isInfinite()) {
			m_param.quaternion = quaternion;
			m_param.quaternion.normalize();
		}
	}
	bsm::Vec3 Transform::GetRotation() const {
		bsm::Vec3 r = m_param.quaternion.toRotVec();
		r.normalize();
		return r;
	}

	void Transform::SetRotation(const bsm::Vec3& rot) {
		if (!rot.isNaN() && !rot.isInfinite()) {
			bsm::Quat qt;
			qt.rotationRollPitchYawFromVector(rot);
			SetQuaternion(qt);
		}
	}
	void Transform::SetRotation(float x, float y, float z) {
		SetRotation(bsm::Vec3(x, y, z));
	}

	void Transform::AddPosition(const bsm::Vec3& addpos) {
		if (!addpos.isNaN() && !addpos.isInfinite()) {
			m_param.position += addpos;
		}
	}


	bsm::Vec3 Transform::GetPosition() const {
		return m_param.position;
	}

	void Transform::SetPosition(const bsm::Vec3& position) {
		if (!position.isNaN() && !position.isInfinite()) {
			m_param.position = position;
		}
	}
	void Transform::SetPosition(float x, float y, float z) {
		SetPosition(bsm::Vec3(x, y, z));
	}

	void Transform::ResetPosition(const bsm::Vec3& position) {
		if (!position.isNaN() && !position.isInfinite()) {
			m_beforeParam.position = position;
			m_param.position = position;
		}
	}

	bsm::Vec3 Transform::GetWorldPosition() {
		return GetWorldMatrix().transInMatrix();
	}
	void Transform::SetWorldPosition(const bsm::Vec3& position) {
		auto SetPos = position;
		auto parPtr = GetParent();
		if (parPtr) {
			auto parWorldPos = parPtr->GetComponent<Transform>()->GetWorldMatrix().transInMatrix();
			SetPos -= parWorldPos;
			auto parQt = parPtr->GetComponent<Transform>()->GetWorldMatrix().quatInMatrix();
			parQt.inverse();
			bsm::Mat4x4 parQtMat(parQt);
			SetPos *= parQtMat;
		}
		SetPosition(SetPos);
	}
	void Transform::ResetWorldPosition(const bsm::Vec3& position) {
		auto setPos = position;
		auto parPtr = GetParent();
		if (parPtr) {
			auto parWorldPos = parPtr->GetComponent<Transform>()->GetWorldMatrix().transInMatrix();
			setPos -= parWorldPos;
			auto parQt = parPtr->GetComponent<Transform>()->GetWorldMatrix().quatInMatrix();
			parQt.inverse();
			bsm::Mat4x4 parQtMat(parQt);
			setPos *= parQtMat;
		}
		ResetPosition(setPos);
	}

	bool Transform::IsSameWorldMatrix(const bsm::Mat4x4& mat) {
		return mat.equalInt(GetWorldMatrix());
	}



	const bsm::Mat4x4& Transform::GetWorldMatrix() {
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


	const bsm::Mat4x4& Transform::Get2DWorldMatrix() {
		auto parPtr = GetParent();
		m_param.scale.z = 1.0f;
		bsm::Vec4 temp_z(m_param.position.z);
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




	std::shared_ptr<GameObject> Transform::GetParent()const {
		auto shPtr = m_parent.lock();
		if (shPtr) {
			return shPtr;
		}
		return nullptr;
	}
	void Transform::SetParent(const std::shared_ptr<GameObject>& Obj) {
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
			bsm::Mat4x4 qarQtMat(qtSpan);
			posSpan *= qarQtMat;

			bsm::Mat4x4 Mat = GetWorldMatrix() * parWorld;
			bsm::Vec3 scale, pos;
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

	bsm::Vec3 Transform::GetVelocity() const {
		//前回のターンからの時間
		auto ElapsedTime = (float)Scene::GetElapsedTime();
		bsm::Vec3 Velocity = m_param.position - m_beforeParam.position;
		Velocity /= ElapsedTime;
		return Velocity;
	}


	bsm::Vec3 Transform::GetMovePositiom() const {
		return m_param.position - m_beforeParam.position;
	}

	float Transform::GetMoveSize() const {
		bsm::Vec3 Move = m_param.position - m_beforeParam.position;
		return Move.length();
	}

	void Transform::SetToBefore() {
		m_beforeParam = m_param;
	}

	bsm::Vec3 Transform::GetForward() {
		bsm::Vec3 ret = GetWorldMatrix().rotZInMatrix();
		ret.normalize();
		return ret;
	}

	bsm::Vec3 Transform::GetUp() {
		bsm::Vec3 ret = GetWorldMatrix().rotYInMatrix();
		ret.normalize();
		return ret;
	}
	bsm::Vec3 Transform::GetRight() {
		bsm::Vec3 ret = GetWorldMatrix().rotXInMatrix();
		ret.normalize();
		return ret;
	}




}
// end namespace basecross
