/*!
@file Behavior.cpp
@brief �s���N���X����
*/

#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	///	�s���N���X�̐e�N���X
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
				L"GameObject�͗L���ł͂���܂���",
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
	///	�s�����[�e�B���e�B�N���X
	//--------------------------------------------------------------------------------------
	//�i�s�����������悤�ɂ���
	void UtilBehavior::RotToHead(float LerpFact) {
		if (LerpFact <= 0.0f) {
			//��ԌW����0�ȉ��Ȃ牽�����Ȃ�
			return;
		}
		//��]�̍X�V
		//Velocity�̒l�ŁA��]��ύX����
		//����Ői�s�����������悤�ɂȂ�
		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		Vec3 Velocity = PtrTransform->GetVelocity();
		if (Velocity.length() > 0.0f) {
			Vec3 Temp = Velocity;
			Temp.normalize();
			float ToAngle = atan2(Temp.x, Temp.z);
			Quat Qt;
			Qt.rotationRollPitchYawFromVector(Vec3(0, ToAngle, 0));
			Qt.normalize();
			//���݂̉�]���擾
			Quat NowQt = PtrTransform->GetQuaternion();
			//���݂ƖڕW����
			//���݂ƖڕW����
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
			//��ԌW����0�ȉ��Ȃ牽�����Ȃ�
			return;
		}
		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		//��]�̍X�V
		if (Velocity.length() > 0.0f) {
			Vec3 Temp = Velocity;
			Temp.normalize();
			float ToAngle = atan2(Temp.x, Temp.z);
			Quat Qt;
			Qt.rotationRollPitchYawFromVector(Vec3(0, ToAngle, 0));
			Qt.normalize();
			//���݂̉�]���擾
			Quat NowQt = PtrTransform->GetQuaternion();
			//���݂ƖڕW����
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
