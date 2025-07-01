/*!
@file Player.cpp
@brief �v���C���[�@����
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// �v���C���[
	//--------------------------------------------------------------------------------------
	Player::Player(const std::shared_ptr<Stage>& stage, const TransParam& param) :
		GameObject(stage),
		m_Speed(6.0f)
	{
		m_tempParam = param;
	}

	Vec2 Player::GetInputState() const {
		Vec2 ret;
		//�R���g���[���̎擾
		auto cntlVec = App::GetInputDevice().GetControlerVec();
		ret.x = 0.0f;
		ret.y = 0.0f;
		WORD wButtons = 0;
		if (cntlVec[0].bConnected) {
			ret.x = cntlVec[0].fThumbLX;
			ret.y = cntlVec[0].fThumbLY;
		}
		return ret;
	}


	Vec3 Player::GetMoveVector() {
		m_angle = Vec3(0, 0, 0);
		//���͂̎擾
		auto inPut = GetInputState();
		float moveX = inPut.x;
		float moveZ = inPut.y;
		if (moveX != 0 || moveZ != 0) {
			float moveLength = 0;	//���������̃X�s�[�h
			auto ptrTransform = GetComponent<Transform>();
			auto ptrCamera = GetStage()->GetCamera();
			//�i�s�����̌������v�Z
			auto front = ptrTransform->GetPosition() - ptrCamera->GetEye();
			front.y = 0;
			front.normalize();
			//�i�s������������̊p�x���Z�o
			float frontAngle = atan2(front.z, front.x);
			//�R���g���[���̌����v�Z
			Vec2 moveVec(moveX, moveZ);
			float moveSize = moveVec.length();
			//�R���g���[���̌�������p�x���v�Z
			float cntlAngle = atan2(-moveX, moveZ);
			//�g�[�^���̊p�x���Z�o
			float totalAngle = frontAngle + cntlAngle;
			//�p�x����x�N�g�����쐬
			m_angle = Vec3(cos(totalAngle), 0, sin(totalAngle));
			//���K������
			m_angle.normalize();
			//�ړ��T�C�Y��ݒ�B
			m_angle *= moveSize;
			//Y���͕ω������Ȃ�
			m_angle.y = 0;
		}
		return m_angle;
	}

	void Player::MovePlayer() {
		float elapsedTime = (float)Scene::GetElapsedTime();
		auto angle = GetMoveVector();
		//RigidbodyDynamic�R���|�[�l���g�����o��
		auto ptrRigid = GetComponent<RigidbodyDynamic>();
		auto pRigDynamic = ptrRigid->GetRigidDynamic();
		if (angle.length() > 0.0f) {
			Vec3 tmpVelo = angle * m_Speed;
			pRigDynamic->setLinearVelocity(bsmUtil::ToPxVec3(tmpVelo));
			//auto pos = GetComponent<Transform>()->GetPosition();
			//pos += angle * (float)elapsedTime * m_Speed;
			//GetComponent<Transform>()->SetPosition(pos);
		}
		else {
			pRigDynamic->setLinearVelocity(bsmUtil::ToPxVec3(Vec3(0)));
		}
	}




	void Player::OnCreate() {
		GetStage()->SetSharedGameObject(L"Player", GetThis<Player>());

		//Transform�R���|�[�l���g�����o��
		auto ptrTrans = GetComponent<Transform>();
		auto& param = ptrTrans->GetTransParam();
		//PhysX�֘A
		PhysxCreateParam pxParam;
		physx::PxSphereGeometry scale(param.scale.x * 0.5f);
		pxParam.pGeometry = &scale;
		pxParam.staticFriction = 1.0f;
		pxParam.dynamicFriction = 1.0f;
		pxParam.restitution = 1.0f;
		auto pRigDynamicComp = AddComponent<RigidbodyDynamic>(pxParam);
		auto pRigDynamic = pRigDynamicComp->GetRigidDynamic();


		auto ptrShadow = AddComponent<Shadowmap>();
		ptrShadow->AddBaseMesh(L"DEFAULT_SPHERE");

		auto ptrDraw = AddComponent<BcStaticDraw>();
		ptrDraw->AddBaseMesh(L"DEFAULT_SPHERE");
		ptrDraw->AddBaseTexture(L"TRACE_TX");
		//��������
		SetAlphaActive(true);
		//�J�����𓾂�
		auto ptrCamera = std::dynamic_pointer_cast<MyCamera>(GetStage()->GetCamera());
		if (ptrCamera) {
			//MyCamera�ł���
			//MyCamera�ɒ��ڂ���I�u�W�F�N�g�i�v���C���[�j�̐ݒ�
			ptrCamera->SetTargetObject(GetThis<GameObject>());
			ptrCamera->SetTargetToAt(Vec3(0, 0.25f, 0));
		}

	}

	void Player::OnUpdate(double elapsedTime) {
		//�R���g���[���`�F�b�N���ē��͂�����΃R�}���h�Ăяo��
		m_InputHandler.PushHandle(GetThis<Player>());
		MovePlayer();
	}

	void Player::OnUpdate2(double elapsedTime) {
		//��]�̌v�Z
		//if (m_angle.length() > 0.0f) {
		//	auto utilPtr = GetBehavior<UtilBehavior>();
		//	utilPtr->RotToHead(m_angle, 1.0f);
		//}
	}


	void Player::OnPushA() {
	}

}
// end namespace basecross
