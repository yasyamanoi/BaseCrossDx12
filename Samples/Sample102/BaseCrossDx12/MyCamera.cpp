/*!
@file MyCamera.cpp
@brief �}�C�J�����N���X�@����
*/


#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	MyCamera�J����
	//--------------------------------------------------------------------------------------
	MyCamera::MyCamera() :
		PerspecCamera(),
		m_ToTargetLerp(1.0f),
		m_TargetToAt(0, 0, 0),
		m_RadY(0.5f),
		m_RadXZ(0),
		m_CameraUpDownSpeed(0.5f),
		m_CameraUnderRot(0.1f),
		m_ArmLen(5.0f),
		m_MaxArm(20.0f),
		m_MinArm(2.0f),
		m_RotSpeed(1.0f),
		m_ZoomSpeed(0.1f),
		m_LRBaseMode(true),
		m_UDBaseMode(true)
	{
	}

	MyCamera::MyCamera(float ArmLen) :
		PerspecCamera(),
		m_ToTargetLerp(1.0f),
		m_TargetToAt(0, 0, 0),
		m_RadY(0.5f),
		m_RadXZ(0),
		m_CameraUpDownSpeed(0.5f),
		m_CameraUnderRot(0.1f),
		m_ArmLen(5.0f),
		m_MaxArm(20.0f),
		m_MinArm(2.0f),
		m_RotSpeed(1.0f),
		m_ZoomSpeed(0.1f),
		m_LRBaseMode(true),
		m_UDBaseMode(true)
	{
		m_ArmLen = ArmLen;
		auto eye = GetEye();
		eye.y = m_ArmLen;
		SetEye(eye);
	}

	MyCamera::~MyCamera() {}
	//�A�N�Z�T

	void MyCamera::SetEye(const Vec3& Eye) {
		PerspecCamera::SetEye(Eye);
		UpdateArmLengh();
	}
	void MyCamera::SetEye(float x, float y, float z) {
		PerspecCamera::SetEye(x, y, z);
		UpdateArmLengh();
	}


	std::shared_ptr<GameObject> MyCamera::GetTargetObject() const {
		if (!m_TargetObject.expired()) {
			return m_TargetObject.lock();
		}
		return nullptr;
	}

	void MyCamera::SetTargetObject(const std::shared_ptr<GameObject>& Obj) {
		m_TargetObject = Obj;
	}

	float MyCamera::GetToTargetLerp() const {
		return m_ToTargetLerp;
	}
	void MyCamera::SetToTargetLerp(float f) {
		m_ToTargetLerp = f;
	}

	float MyCamera::GetArmLengh() const {
		return m_ArmLen;
	}

	void MyCamera::UpdateArmLengh() {
		auto vec = GetEye() - GetAt();
		m_ArmLen = bsmUtil::length(vec);
		if (m_ArmLen >= m_MaxArm) {
			//m_MaxArm�ȏ㗣��Ȃ��悤�ɂ���
			m_ArmLen = m_MaxArm;
		}
		if (m_ArmLen <= m_MinArm) {
			//m_MinArm�ȉ��߂Â��Ȃ��悤�ɂ���
			m_ArmLen = m_MinArm;
		}
	}

	float MyCamera::GetMaxArm() const {
		return m_MaxArm;

	}
	void MyCamera::SetMaxArm(float f) {
		m_MaxArm = f;
	}
	float MyCamera::GetMinArm() const {
		return m_MinArm;
	}
	void MyCamera::SetMinArm(float f) {
		m_MinArm = f;
	}

	float MyCamera::GetRotSpeed() const {
		return m_RotSpeed;

	}
	void MyCamera::SetRotSpeed(float f) {
		m_RotSpeed = abs(f);
	}

	Vec3 MyCamera::GetTargetToAt() const {
		return m_TargetToAt;

	}
	void MyCamera::SetTargetToAt(const Vec3& v) {
		m_TargetToAt = v;
	}

	bool MyCamera::GetLRBaseMode() const {
		return m_LRBaseMode;

	}
	bool MyCamera::IsLRBaseMode() const {
		return m_LRBaseMode;

	}
	void MyCamera::SetLRBaseMode(bool b) {
		m_LRBaseMode = b;
	}
	bool MyCamera::GetUDBaseMode() const {
		return m_UDBaseMode;

	}
	bool MyCamera::IsUDBaseMode() const {
		return m_UDBaseMode;
	}
	void MyCamera::SetUDBaseMode(bool b) {
		m_UDBaseMode = b;

	}


	void MyCamera::SetAt(const Vec3& At) {
		PerspecCamera::SetAt(At);
		Vec3 armVec = GetEye() - GetAt();
		armVec.normalize();
		armVec *= m_ArmLen;
		Vec3 newEye = GetAt() + armVec;
		PerspecCamera::SetEye(newEye);
	}
	void MyCamera::SetAt(float x, float y, float z) {
		PerspecCamera::SetAt(x, y, z);
		Vec3 armVec = GetEye() - GetAt();
		armVec.normalize();
		armVec *= m_ArmLen;
		Vec3 newEye = GetAt() + armVec;
		PerspecCamera::SetEye(newEye);

	}


	void MyCamera::OnUpdate(double elapsedTime) {
		auto cntlVec = App::GetInputDevice().GetControlerVec();
		//		auto keyData = App::GetInputDevice().GetKeyState();
		//�O��̃^�[������̎���
		Vec3 newEye = GetEye();
		Vec3 newAt = GetAt();
		//�v�Z�Ɏg�����߂̘r�p�x�i�x�N�g���j
		Vec3 armVec = newEye - newAt;
		//���K�����Ă���
		armVec.normalize();
		float fThumbRY = 0.0f;
		float fThumbRX = 0.0f;
		WORD wButtons = 0;
		if (cntlVec[0].bConnected) {
			fThumbRY = cntlVec[0].fThumbRY;
			fThumbRX = cntlVec[0].fThumbRX;
			wButtons = cntlVec[0].wButtons;
		}

		//�㉺�p�x�̕ύX
		if (fThumbRY >= 0.1f) {
			if (IsUDBaseMode()) {
				m_RadY += m_CameraUpDownSpeed * (float)elapsedTime;
			}
			else {
				m_RadY -= m_CameraUpDownSpeed * (float)elapsedTime;
			}
		}
		else if (fThumbRY <= -0.1f) {
			if (IsUDBaseMode()) {
				m_RadY -= m_CameraUpDownSpeed * (float)elapsedTime;
			}
			else {
				m_RadY += m_CameraUpDownSpeed * (float)elapsedTime;
			}
		}
		if (m_RadY > XM_PI * 4 / 9.0f) {
			m_RadY = XM_PI * 4 / 9.0f;
		}
		else if (m_RadY <= m_CameraUnderRot) {
			//�J���������E���ɉ��������炻��ȏ㉺����Ȃ�
			m_RadY = m_CameraUnderRot;
		}
		armVec.y = sin(m_RadY);
		//������Y����]���쐬
		if (fThumbRX != 0) {
			//��]�X�s�[�h�𔽉f
			if (fThumbRX != 0) {
				if (IsLRBaseMode()) {
					m_RadXZ += -fThumbRX * (float)elapsedTime * m_RotSpeed;
				}
				else {
					m_RadXZ += fThumbRX * (float)elapsedTime * m_RotSpeed;
				}
			}
			if (abs(m_RadXZ) >= XM_2PI) {
				//1�T�������0��]�ɂ���
				m_RadXZ = 0;
			}
		}
		//�N�I�[�^�j�I����Y��]�i�܂�XZ�x�N�g���̒l�j���v�Z
		Quat qtXZ;
		qtXZ.rotationAxis(Vec3(0, 1.0f, 0),m_RadXZ);
		qtXZ.normalize();
		//�ړ���s�̍s��v�Z���邱�ƂŁAXZ�̒l���Z�o
		Mat4x4 Mat;
		Mat.strTransformation(
			Vec3(1.0f, 1.0f, 1.0f),
			Vec3(0.0f, 0.0f, -1.0f),
			qtXZ
		);

		Vec3 posXZ = Mat.transInMatrix();
		//XZ�̒l���킩�����̂Řr�p�x�ɑ��
		armVec.x = posXZ.x;
		armVec.z = posXZ.z;
		//�r�p�x�𐳋K��
		armVec.normalize();

		auto ptrTarget = GetTargetObject();
		if (ptrTarget) {
			//�ڎw�������ꏊ
			Vec3 toAt = ptrTarget->GetComponent<Transform>()->GetWorldMatrix().transInMatrix();
			toAt += m_TargetToAt;
			newAt = Lerp::CalculateLerp(GetAt(), toAt, 0, 1.0f, 1.0, Lerp::Linear);
		}
		//�A�[���̕ύX
		//D�p�b�h��
		if (wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
			//�J�����ʒu������
			m_ArmLen += m_ZoomSpeed;
			if (m_ArmLen >= m_MaxArm) {
				//m_MaxArm�ȏ㗣��Ȃ��悤�ɂ���
				m_ArmLen = m_MaxArm;
			}
		}
		//D�p�b�h��
		else if (wButtons & XINPUT_GAMEPAD_DPAD_UP) {
			//�J�����ʒu�����
			m_ArmLen -= m_ZoomSpeed;
			if (m_ArmLen <= m_MinArm) {
				//m_MinArm�ȉ��߂Â��Ȃ��悤�ɂ���
				m_ArmLen = m_MinArm;
			}
		}
		////�ڎw�������ꏊ�ɃA�[���̒l�Ƙr�x�N�g����Eye�𒲐�
		Vec3 toEye = newAt + armVec * m_ArmLen;
		newEye = Lerp::CalculateLerp(GetEye(), toEye, 0, 1.0f, m_ToTargetLerp, Lerp::Linear);

		SetAt(newAt);
		SetEye(newEye);
		UpdateArmLengh();
		PerspecCamera::OnUpdate(elapsedTime);
	}



}
//end 