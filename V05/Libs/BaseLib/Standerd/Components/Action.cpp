/*!
@file Action.cpp
@brief �A�N�V�����n�R���|�[�l���g
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	class ActionComponent : public Component ;
	//	�p�r: �A�N�V�����n�R���|�[�l���g�̐e�N���X
	//--------------------------------------------------------------------------------------
	//�\�z�Ɣj��
	ActionComponent::ActionComponent(const shared_ptr<GameObject>& GameObjectPtr) :
		Component(GameObjectPtr),
		m_Run(false),
		m_Arrived(false),
		m_TotalTime(0),
		m_NowTime(0)
	{}
	ActionComponent::~ActionComponent() {}

	//�A�N�Z�T
	void ActionComponent::SetRun(bool b) {
		m_Run = b;
	}
	void  ActionComponent::SetArrived(bool b) {
		m_Arrived = b;
	}


	bool ActionComponent::IsRun()const {
		return m_Run;
	}
	bool ActionComponent::GetRun()const {
		return m_Run;
	}

	bool ActionComponent::IsArrived()const {
		return m_Arrived;
	}
	bool ActionComponent::GetArrived()const {
		return m_Arrived;
	}

	float ActionComponent::GetTotalTime() const {
		return m_TotalTime;
	}
	void ActionComponent::SetTotalTime(float f) {
		m_TotalTime = f;
	}

	float ActionComponent::GetNowTime() const {
		return m_NowTime;
	}
	void ActionComponent::SetNowTime(float f) {
		m_NowTime = f;
	}

	//NowTime�����Z���āATotalTime�Ɣ�r����
	bool ActionComponent::AdditionalNowTime() {
		if (m_Run) {
			if (m_TotalTime > 0) {
				float ElapsedTime = App::GetElapsedTime();
				m_NowTime += ElapsedTime;
				if (m_NowTime >= m_TotalTime) {
					m_NowTime = m_TotalTime;
					m_Run = false;
					//��������
					m_Arrived = true;
				}
				return true;
			}
		}
		return false;
	}


	void ActionComponent::Stop() {
		m_Run = false;
	}
	void ActionComponent::ReStart() {
		if (!m_Arrived) {
			//�������ĂȂ���Γ��삳����
			m_Run = true;
		}
	}

	//--------------------------------------------------------------------------------------
	//	class ActionInterval : public  ActionComponent ;
	//	�p�r: �A�N�V�����̃C���^�[�o��
	//--------------------------------------------------------------------------------------
	//�\�z�Ɣj��
	ActionInterval::ActionInterval(const shared_ptr<GameObject>& GameObjectPtr) :
		ActionComponent(GameObjectPtr)
	{}
	ActionInterval::~ActionInterval() {}

	void ActionInterval::SetParams(float TotalTime) {
		SetTotalTime(TotalTime);
	}

	void ActionInterval::Run() {
		SetNowTime(0);
		SetRun(true);
		SetArrived(false);
		if (GetTotalTime() <= 0) {
			//�g�[�^���^�C����0�ȉ��Ȃ�A���łɓ������Ă��邱�ƂɂȂ�
			SetTotalTime(0);
			SetNowTime(0);
			SetRun(false);
			SetArrived(true);
		}
	}
	void ActionInterval::OnUpdate() {
		if (GetRun()) {
			if (!AdditionalNowTime()) {
				if (GetTotalTime() <= 0) {
					//�g�[�^���^�C����0�ȉ��Ȃ�A���łɓ������Ă��邱�ƂɂȂ�
					SetTotalTime(0);
					SetNowTime(0);
					SetRun(false);
					SetArrived(true);
				}
			}
		}
	}


	//--------------------------------------------------------------------------------------
	//	class ScaleComponent : public Component ;
	//	�p�r: �g��k���A�N�V�����̐e�N���X
	//--------------------------------------------------------------------------------------
	//�\�z�Ɣj��
	ScaleComponent::ScaleComponent(const shared_ptr<GameObject>& GameObjectPtr) :
		ActionComponent(GameObjectPtr),
		m_StartScale(1.0f, 1.0f, 1.0f),
		m_TargetScale(1.0f, 1.0f, 1.0f),
		m_Rate(Lerp::Linear)
	{}
	ScaleComponent::~ScaleComponent() {}


	//�A�N�Z�T
	Lerp::rate ScaleComponent::GetRate() const { return m_Rate; }
	void ScaleComponent::SetRate(const Lerp::rate r) { m_Rate = r; }

	const bsm::Vec3& ScaleComponent::GetStartScale() const { return m_StartScale; }
	void ScaleComponent::SetStartScale(const bsm::Vec3& StartPosition) { m_StartScale = StartPosition; }
	void ScaleComponent::SetStartScale(float x, float y, float z) { m_StartScale = bsm::Vec3(x, y, z); }

	const bsm::Vec3& ScaleComponent::GetTargetScale() const { return m_TargetScale; }
	void ScaleComponent::SetTargetScale(const bsm::Vec3& TargetScale) { m_TargetScale = TargetScale; }
	void ScaleComponent::SetTargetScale(float x, float y, float z) { m_TargetScale = bsm::Vec3(x, y, z); }

	//����
	void ScaleComponent::Run() {
		auto TransPtr = GetGameObject()->GetComponent<Transform>();
		m_StartScale = TransPtr->GetScale();
		SetNowTime(0);
		SetRun(true);
		SetArrived(false);
		if (GetTotalTime() <= 0) {
			//�g�[�^���^�C����0�ȉ��Ȃ�A���łɓ������Ă��邱�ƂɂȂ�
			TransPtr->SetScale(m_TargetScale);
			SetTotalTime(0);
			SetNowTime(0);
			SetRun(false);
			SetArrived(true);
		}
	}

	void ScaleComponent::OnUpdate() {
		if (GetRun()) {
			auto TransPtr = GetGameObject()->GetComponent<Transform>();
			if (AdditionalNowTime()) {
				bsm::Vec3 Scale = Lerp::CalculateLerp<bsm::Vec3>(
					m_StartScale,
					m_TargetScale,
					0,
					GetTotalTime(),
					GetNowTime(),
					m_Rate
					);
				TransPtr->SetScale(Scale);
			}
			else {
				if (GetTotalTime() <= 0) {
					//�g�[�^���^�C����0�ȉ��Ȃ�A���łɓ������Ă��邱�ƂɂȂ�
					TransPtr->SetScale(m_TargetScale);
					SetTotalTime(0);
					SetNowTime(0);
					SetRun(false);
					SetArrived(true);
				}
			}
		}
	}

	//���݃X�P�[�����v�Z���ĕԂ�
	//�^�C���̍X�V�͍s��Ȃ��̂�
	//Update��ɌĂԂׂ�
	bsm::Vec3 ScaleComponent::GetNowScale() const {
		if (GetTotalTime() <= 0) {
			return m_TargetScale;
		}
		bsm::Vec3 Scale = Lerp::CalculateLerp(
			m_StartScale,
			m_TargetScale,
			0,
			GetTotalTime(),
			GetNowTime(),
			m_Rate
		);
		return Scale;
	}



	//--------------------------------------------------------------------------------------
	//	class ScaleTo : public  ScaleComponent ;
	//	�p�r: �ړI�̊g�嗦�Ɋg��k��
	//--------------------------------------------------------------------------------------
	ScaleTo::ScaleTo(const shared_ptr<GameObject>& GameObjectPtr) :
		ScaleComponent(GameObjectPtr) {}
	ScaleTo::~ScaleTo() {}
	void ScaleTo::SetParams(float TotalTime, const bsm::Vec3& TargetScale, Lerp::rate Rate) {
		SetTargetScale(TargetScale);
		SetTotalTime(TotalTime);
		SetRate(Rate);
	}

	//--------------------------------------------------------------------------------------
	//	class ScaleBy : public  ScaleComponent;
	//	�p�r: �ړI�̑��΃X�P�[���ɂɊg��k��
	//--------------------------------------------------------------------------------------
	//�\�z�Ɣj��
	ScaleBy::ScaleBy(const shared_ptr<GameObject>& GameObjectPtr) :
		ScaleComponent(GameObjectPtr), 
		m_LocalScale(1.0f, 1.0f, 1.0f) {}
	ScaleBy::~ScaleBy() {}


	//�A�N�Z�T
	const bsm::Vec3& ScaleBy::GetLocalScale() const { return m_LocalScale; }
	void ScaleBy::SetLocalScale(const bsm::Vec3& LocalScale) { m_LocalScale = LocalScale; }
	void ScaleBy::SetLocalScale(float x, float y, float z) { m_LocalScale = bsm::Vec3(x, y, z); }

	void ScaleBy::SetParams(float TotalTime, const bsm::Vec3& LocalScale, Lerp::rate Rate) {
		SetLocalScale(LocalScale);
		SetTotalTime(TotalTime);
		SetRate(Rate);
	}


	//����
	void ScaleBy::Run() {
		auto TransPtr = GetGameObject()->GetComponent<Transform>();
		bsm::Vec3 Target = TransPtr->GetScale();
		Target.x *= m_LocalScale.x;
		Target.y *= m_LocalScale.y;
		Target.z *= m_LocalScale.z;
		SetTargetScale(Target);
		ScaleComponent::Run();
	}

	//--------------------------------------------------------------------------------------
	//	class RotateComponent : public ActionComponent ;
	//	�p�r: ��]�A�N�V�����̐e�N���X
	//--------------------------------------------------------------------------------------
	//�\�z�Ɣj��
	RotateComponent::RotateComponent(const shared_ptr<GameObject>& GameObjectPtr) :
		ActionComponent(GameObjectPtr),
		m_StartQuaternion(),
		m_TargetQuaternion()
	{
	}
	RotateComponent::~RotateComponent() {}


	//�A�N�Z�T
	const bsm::Quat& RotateComponent::GetStartQuaternion() const { return m_StartQuaternion; }
	void RotateComponent::SetStartQuaternion(const bsm::Quat& StartQuaternion) {
		m_StartQuaternion = StartQuaternion;
		m_StartQuaternion.normalize();
	}

	bsm::Vec3 RotateComponent::GetStartRotate() const {
		return m_StartQuaternion.toRotVec();
	}
	void RotateComponent::SetStartRotate(const bsm::Vec3& StartRotate) {
		m_StartQuaternion.rotationRollPitchYawFromVector(StartRotate);
		m_StartQuaternion.normalize();
	}
	void RotateComponent::SetStartRotate(float x, float y, float z) {
		m_StartQuaternion.rotationRollPitchYawFromVector(bsm::Vec3(x, y, z));
		m_StartQuaternion.normalize();
	}

	const bsm::Quat& RotateComponent::GetTargetQuaternion() const {
		return m_TargetQuaternion;
	}
	void RotateComponent::SetTargetQuaternion(const bsm::Quat& TargetQuaternion) {
		m_TargetQuaternion = TargetQuaternion;
		m_TargetQuaternion.normalize();
	}
	bsm::Vec3 RotateComponent::GetTargetRotate() const {
		return m_TargetQuaternion.toRotVec();
	}
	void RotateComponent::SetTargetRotate(const bsm::Vec3& TargetRotate) {
		m_TargetQuaternion.rotationRollPitchYawFromVector(TargetRotate);
		m_TargetQuaternion.normalize();
	}
	void RotateComponent::SetTargetRotate(float x, float y, float z) {
		m_TargetQuaternion.rotationRollPitchYawFromVector(bsm::Vec3(x, y, z));
		m_TargetQuaternion.normalize();
	}

	//����
	void RotateComponent::Run() {
		auto TransPtr = GetGameObject()->GetComponent<Transform>();
		//�N�I�[�^�j�I�����猻�݂̉�]�𓾂�
		m_StartQuaternion = TransPtr->GetQuaternion();
		SetNowTime(0);
		SetRun(true);
		SetArrived(false);
		if (GetTotalTime() <= 0) {
			//�g�[�^���^�C����0�ȉ��Ȃ�A���łɓ������Ă��邱�ƂɂȂ�
			TransPtr->SetQuaternion(m_TargetQuaternion);
			SetTotalTime(0);
			SetNowTime(0);
			SetRun(false);
			SetArrived(true);
		}
	}
	void RotateComponent::OnUpdate() {
		if (GetRun()) {
			auto TransPtr = GetGameObject()->GetComponent<Transform>();
			if (AdditionalNowTime()) {
				bsm::Quat Qt(XMQuaternionSlerp(
					m_StartQuaternion,
					m_TargetQuaternion,
					GetNowTime() / GetTotalTime()
				));
				Qt.normalize();
				TransPtr->SetQuaternion(Qt);
			}
			else {
				if (GetTotalTime() <= 0) {
					//�g�[�^���^�C����0�ȉ��Ȃ�A���łɓ������Ă��邱�ƂɂȂ�
					TransPtr->SetQuaternion(m_TargetQuaternion);
					SetTotalTime(0);
					SetNowTime(0);
					SetRun(false);
					SetArrived(true);
				}
			}
		}
	}

	//���݉�]���v�Z���ĕԂ�
	//�^�C���̍X�V�͍s��Ȃ��̂�
	//Update��ɌĂԂׂ�
	bsm::Vec3 RotateComponent::GetNowRotate() const {
		if (GetTotalTime() <= 0) {
			return m_TargetQuaternion.toRotVec();
		}
		bsm::Quat Qt(XMQuaternionSlerp(
			m_StartQuaternion,
			m_TargetQuaternion,
			GetNowTime() / GetTotalTime()
		));
		Qt.normalize();
		return Qt.toRotVec();
	}
	bsm::Quat RotateComponent::GetNowQuaternion() const {
		if (GetTotalTime() <= 0) {
			return m_TargetQuaternion;
		}
		bsm::Quat Qt(XMQuaternionSlerp(
			m_StartQuaternion,
			m_TargetQuaternion,
			GetNowTime() / GetTotalTime()
		));
		Qt.normalize();
		return Qt;
	}



	//--------------------------------------------------------------------------------------
	//	class RotateTo : public  MoveComponent ;
	//	�p�r: �ړI�̊p�x�ɉ�]
	//--------------------------------------------------------------------------------------
	//�\�z�Ɣj��
	RotateTo::RotateTo(const shared_ptr<GameObject>& GameObjectPtr) :
		RotateComponent(GameObjectPtr)
	{}
	RotateTo::~RotateTo() {}

	void RotateTo::SetParams(float TotalTime, const bsm::Vec3& TargetRotate) {
		SetTargetRotate(TargetRotate);
		SetTotalTime(TotalTime);
	}

	void RotateTo::SetParams(float TotalTime, const bsm::Quat& TargetQuaternion) {
		SetTargetQuaternion(TargetQuaternion);
		SetTotalTime(TotalTime);
	}



	//--------------------------------------------------------------------------------------
	//	class RotateBy : public  RotateComponent ;
	//	�p�r: �ړI�̑��Ίp�x�ɉ�]
	//--------------------------------------------------------------------------------------
	//�\�z�Ɣj��
	RotateBy::RotateBy(const shared_ptr<GameObject>& GameObjectPtr) :
		RotateComponent(GameObjectPtr),
		m_LocalQuaternion()
	{}
	RotateBy::~RotateBy() {}

	//�A�N�Z�T
	const bsm::Quat& RotateBy::GetLocalQuaternion() const { return m_LocalQuaternion; }
	void RotateBy::SetLocalQuaternion(const bsm::Quat& LocalQuaternion) {
		m_LocalQuaternion = LocalQuaternion;
		m_LocalQuaternion.normalize();
	}
	bsm::Vec3 RotateBy::GetLocalRotate() const {
		return m_LocalQuaternion.toRotVec();
	}
	void RotateBy::SetLocalRotate(const bsm::Vec3& LocalRotate) {
		m_LocalQuaternion.rotationRollPitchYawFromVector(LocalRotate);
		m_LocalQuaternion.normalize();
	}
	void RotateBy::SetLocalRotate(float x, float y, float z) {
		m_LocalQuaternion.rotationRollPitchYawFromVector(bsm::Vec3(x, y, z));
		m_LocalQuaternion.normalize();
	}

	void RotateBy::SetParams(float TotalTime, const bsm::Vec3& LocalRotate) {
		SetTotalTime(TotalTime);
		SetLocalRotate(LocalRotate);
	}

	void RotateBy::SetParams(float TotalTime, const bsm::Quat& LocalQuaternion) {
		SetTotalTime(TotalTime);
		SetLocalQuaternion(LocalQuaternion);
	}


	//����
	void RotateBy::Run() {
		auto TransPtr = GetGameObject()->GetComponent<Transform>();
		bsm::Quat QtNow = TransPtr->GetQuaternion();
		bsm::Quat Target = QtNow * m_LocalQuaternion;
		SetTargetQuaternion(Target);
		RotateComponent::Run();
	}

	//--------------------------------------------------------------------------------------
	//	class MoveComponent : public ActionComponent ;
	//	�p�r: �ړ��A�N�V�����̐e�N���X
	//--------------------------------------------------------------------------------------
	//�\�z�Ɣj��
	MoveComponent::MoveComponent(const shared_ptr<GameObject>& GameObjectPtr) :
		ActionComponent(GameObjectPtr),
		m_StartPosition(0, 0, 0),
		m_TargetPosition(0, 0, 0),
		m_RateX(Lerp::Linear),
		m_RateY(Lerp::Linear),
		m_RateZ(Lerp::Linear),
		m_Velocity(0, 0, 0)
	{}
	MoveComponent::~MoveComponent() {}
	//�A�N�Z�T

	Lerp::rate MoveComponent::GetRateX() const {
		return m_RateX;
	}
	Lerp::rate MoveComponent::GetRateY() const {
		return m_RateY;
	}
	Lerp::rate MoveComponent::GetRateZ() const {
		return m_RateZ;
	}
	void MoveComponent::SetRateX(const Lerp::rate r) {
		m_RateX = r;
	}
	void MoveComponent::SetRateY(const Lerp::rate r) {
		m_RateY = r;
	}
	void MoveComponent::SetRateZ(const Lerp::rate r) {
		m_RateZ = r;
	}
	void MoveComponent::SetRateAll(const Lerp::rate r) {
		m_RateX = r;
		m_RateY = r;
		m_RateZ = r;
	}
	void MoveComponent::SetRateAll(const Lerp::rate rX, const Lerp::rate rY, const Lerp::rate rZ) {
		m_RateX = rX;
		m_RateY = rY;
		m_RateZ = rZ;
	}

	bsm::Vec3 MoveComponent::GetVelocity()const {
		bsm::Vec3 Ret = m_Velocity;
		if (GetTotalTime() <= 0) {
			Ret.setAll(0.0f);
		}
		else {
			if (!GetRun() || GetArrived()) {
				Ret.setAll(0.0f);
			}
		}
		return Ret;
	}



	const bsm::Vec3& MoveComponent::GetStartPosition() const { return m_StartPosition; }
	void MoveComponent::SetStartPosition(const bsm::Vec3& StartPosition) { m_StartPosition = StartPosition; }
	void MoveComponent::SetStartPosition(float x, float y, float z) { m_StartPosition = bsm::Vec3(x, y, z); }

	const bsm::Vec3& MoveComponent::GetTargetPosition() const { return m_TargetPosition; }
	void MoveComponent::SetTargetPosition(const bsm::Vec3& TargetPosition) { m_TargetPosition = TargetPosition; }
	void MoveComponent::SetTargetPosition(float x, float y, float z) { m_TargetPosition = bsm::Vec3(x, y, z); }

	bsm::Vec3 MoveComponent::CalcVelocitySub(float NowTime) {
		bsm::Vec3 Pos;
		if (m_RateX == m_RateY && m_RateX == m_RateZ) {
			Pos = Lerp::CalculateLerp<bsm::Vec3>(
				m_StartPosition,
				m_TargetPosition,
				0,
				GetTotalTime(),
				NowTime,
				m_RateX
				);
		}
		else {
			Pos.x = Lerp::CalculateLerp<float>(
				m_StartPosition.x,
				m_TargetPosition.x,
				0,
				GetTotalTime(),
				NowTime,
				m_RateX
				);
			Pos.y = Lerp::CalculateLerp<float>(
				m_StartPosition.y,
				m_TargetPosition.y,
				0,
				GetTotalTime(),
				NowTime,
				m_RateY
				);
			Pos.z = Lerp::CalculateLerp<float>(
				m_StartPosition.z,
				m_TargetPosition.z,
				0,
				GetTotalTime(),
				NowTime,
				m_RateZ
				);
		}
		return Pos;
	}


	void MoveComponent::CalcVelocity() {
		if (GetTotalTime() <= 0) {
			m_Velocity = bsm::Vec3(0, 0, 0);
			return;
		}
		else {
			if (!GetRun() || GetArrived()) {
				m_Velocity = bsm::Vec3(0, 0, 0);
				return;
			}
			else {
				float NowTime = GetNowTime();
				if (NowTime <= 0) {
					m_Velocity = bsm::Vec3(0, 0, 0);
					return;
				}
				float ElapsedTime = App::GetElapsedTime();
				if (ElapsedTime <= 0) {
					m_Velocity = bsm::Vec3(0, 0, 0);
					return;
				}
				float BeforeTime = GetNowTime() - App::GetElapsedTime();
				if (BeforeTime <= 0) {
					BeforeTime = 0;
				}
				if ((NowTime - BeforeTime) <= 0) {
					m_Velocity = bsm::Vec3(0, 0, 0);
					return;
				}
				bsm::Vec3 BeforePos = CalcVelocitySub(BeforeTime);
				bsm::Vec3 NowPos = CalcVelocitySub(NowTime);
				m_Velocity = (NowPos - BeforePos) / ElapsedTime;
				return;
			}
		}
	}


	//����
	void MoveComponent::Run() {
		auto TransPtr = GetGameObject()->GetComponent<Transform>();
		m_StartPosition = TransPtr->GetWorldPosition();
		SetNowTime(0);
		SetRun(true);
		SetArrived(false);
		if (GetTotalTime() <= 0) {
			//�g�[�^���^�C����0�ȉ��Ȃ�A���łɓ������Ă��邱�ƂɂȂ�
			TransPtr->SetWorldPosition(m_TargetPosition);
			SetTotalTime(0);
			SetNowTime(0);
			SetRun(false);
			SetArrived(true);
		}
		CalcVelocity();
	}

	void MoveComponent::OnUpdate() {
		if (GetRun()) {
			auto PtrTrans = GetGameObject()->GetComponent<Transform>();
			if (AdditionalNowTime()) {
				bsm::Vec3 Pos;
				if (m_RateX == m_RateY && m_RateX == m_RateZ) {
					Pos = Lerp::CalculateLerp<bsm::Vec3>(
						m_StartPosition,
						m_TargetPosition,
						0,
						GetTotalTime(),
						GetNowTime(),
						m_RateX
						);
				}
				else {
					Pos.x = Lerp::CalculateLerp<float>(
						m_StartPosition.x,
						m_TargetPosition.x,
						0,
						GetTotalTime(),
						GetNowTime(),
						m_RateX
						);
					Pos.y = Lerp::CalculateLerp<float>(
						m_StartPosition.y,
						m_TargetPosition.y,
						0,
						GetTotalTime(),
						GetNowTime(),
						m_RateY
						);
					Pos.z = Lerp::CalculateLerp<float>(
						m_StartPosition.z,
						m_TargetPosition.z,
						0,
						GetTotalTime(),
						GetNowTime(),
						m_RateZ
						);
				}
				PtrTrans->SetWorldPosition(Pos);
			}
			else {
				if (GetTotalTime() <= 0) {
					//�g�[�^���^�C����0�ȉ��Ȃ�A���łɓ������Ă��邱�ƂɂȂ�
					PtrTrans->SetWorldPosition(m_TargetPosition);
					SetTotalTime(0);
					SetNowTime(0);
					SetRun(false);
					SetArrived(true);
				}
			}
			CalcVelocity();
		}
	}

	//���݈ʒu���v�Z���ĕԂ�
	//�^�C���̍X�V�͍s��Ȃ��̂�
	//Update��ɌĂԂׂ�
	bsm::Vec3 MoveComponent::GetNowPosition() const {
		if (GetTotalTime() <= 0) {
			return m_TargetPosition;
		}
		bsm::Vec3 Pos;
		if (m_RateX == m_RateY && m_RateX == m_RateZ) {
			Pos = Lerp::CalculateLerp<bsm::Vec3>(
				m_StartPosition,
				m_TargetPosition,
				0,
				GetTotalTime(),
				GetNowTime(),
				m_RateX
				);
		}
		else {
			Pos.x = Lerp::CalculateLerp<float>(
				m_StartPosition.x,
				m_TargetPosition.x,
				0,
				GetTotalTime(),
				GetNowTime(),
				m_RateX
				);
			Pos.y = Lerp::CalculateLerp<float>(
				m_StartPosition.y,
				m_TargetPosition.y,
				0,
				GetTotalTime(),
				GetNowTime(),
				m_RateY
				);
			Pos.z = Lerp::CalculateLerp<float>(
				m_StartPosition.z,
				m_TargetPosition.z,
				0,
				GetTotalTime(),
				GetNowTime(),
				m_RateZ
				);
		}
		return Pos;
	}



	//--------------------------------------------------------------------------------------
	//	class MoveTo : public  MoveComponent ;
	//	�p�r: �ړI�̈ʒu�Ɉړ�
	//--------------------------------------------------------------------------------------
	MoveTo::MoveTo(const shared_ptr<GameObject>& GameObjectPtr) :
		MoveComponent(GameObjectPtr) {}
	MoveTo::~MoveTo() {}

	void MoveTo::SetParams(float TotalTime, const bsm::Vec3& TargetPosition, Lerp::rate Rate) {
		SetTargetPosition(TargetPosition);
		SetTotalTime(TotalTime);
		SetRateAll(Rate);
	}

	void MoveTo::SetParams(float TotalTime, const bsm::Vec3& TargetPosition, Lerp::rate RateX, Lerp::rate RateY, Lerp::rate RateZ) {
		SetTargetPosition(TargetPosition);
		SetTotalTime(TotalTime);
		SetRateX(RateX);
		SetRateY(RateY);
		SetRateZ(RateZ);
	}




	//--------------------------------------------------------------------------------------
	//	class MoveBy : public  MoveComponent ;
	//	�p�r: �ړI�̑��Έʒu�Ɉړ�
	//--------------------------------------------------------------------------------------
	//�\�z�Ɣj��
	MoveBy::MoveBy(const shared_ptr<GameObject>& GameObjectPtr) :
		MoveComponent(GameObjectPtr),
		m_LocalVector(0, 0, 0)
	{}
	MoveBy::~MoveBy() {}

	//�A�N�Z�T
	const bsm::Vec3& MoveBy::GetLocalVector() const { return m_LocalVector; }
	void MoveBy::SetLocalVector(const bsm::Vec3& LocalVector) { m_LocalVector = LocalVector; }
	void MoveBy::SetLocalVector(float x, float y, float z) { m_LocalVector = bsm::Vec3(x, y, z); }

	void MoveBy::SetParams(float TotalTime, const bsm::Vec3& LocalVector, Lerp::rate Rate) {
		SetLocalVector(LocalVector);
		SetTotalTime(TotalTime);
		SetRateAll(Rate);
	}

	void MoveBy::SetParams(float TotalTime, const bsm::Vec3& LocalVector, Lerp::rate RateX, Lerp::rate RateY, Lerp::rate RateZ) {
		SetLocalVector(LocalVector);
		SetTotalTime(TotalTime);
		SetRateX(RateX);
		SetRateY(RateY);
		SetRateZ(RateZ);
	}

	//����
	void MoveBy::Run() {
		auto PtrTrans = GetGameObject()->GetComponent<Transform>();
		bsm::Vec3 Pos = PtrTrans->GetWorldPosition();
		bsm::Vec3 Target = Pos + m_LocalVector;
		SetTargetPosition(Target);
		MoveComponent::Run();
	}


	//--------------------------------------------------------------------------------------
	//	class Action : public Component ;
	//	�p�r: ������ԕύX�R���|�[�l���g
	//--------------------------------------------------------------------------------------
	//�\�z�Ɣj��
	Action::Action(const shared_ptr<GameObject>& GameObjectPtr) :
		Component(GameObjectPtr),
		m_Looped(false),
		m_ScaleArrived(false),
		m_RotateArrived(false),
		m_MoveArrived(false),
		m_ScaleActiveIndex(0),
		m_RotateActiveIndex(0),
		m_MoveActiveIndex(0)
	{}
	Action::~Action() {}

	//�A�N�Z�T
	bool Action::IsLooped()const { return m_Looped; }
	bool Action::GetLooped()const { return m_Looped; }
	void Action::SetLooped(bool b) { m_Looped = b; }

	bool Action::IsArrived()const {
		bool retScale = true;
		if (m_ScaleVec.size() > 0) {
			//�A�N�V���������邪�I�����ĂȂ�
			if (!m_ScaleArrived) {
				retScale = false;
			}
		}
		bool retRot = true;
		if (m_RotateVec.size() > 0) {
			//�A�N�V���������邪�I�����ĂȂ�
			if (!m_RotateArrived) {
				retRot = false;
			}
		}
		bool retMove = true;
		if (m_MoveVec.size() > 0) {
			//�A�N�V���������邪�I�����ĂȂ�
			if (!m_MoveArrived) {
				retMove = false;
			}
		}
		return
			retScale &&
			retRot &&
			retMove;
	}
	bool Action::GetArrived()const { return IsArrived(); }


	size_t Action::GetScaleActiveIndex() const {
		if (m_ScaleVec.size() <= 0) {
			throw BaseException(
				L"�X�P�[���A�N�V�������o�^����Ă��܂���",
				L"if (m_ScaleVec.size() <= 0)",
				L"Action::GetScaleActiveIndex()"
			);
		}
		return m_ScaleActiveIndex;
	}
	size_t Action::GetRotateActiveIndex() const {
		if (m_RotateVec.size() <= 0) {
			throw BaseException(
				L"��]�A�N�V�������o�^����Ă��܂���",
				L"if (m_RotateVec.size() <= 0)",
				L"Action::GetRotateActiveIndex()"
			);
		}
		return m_RotateActiveIndex;
	}
	size_t Action::GetMoveActiveIndex() const {
		if (m_MoveVec.size() <= 0) {
			throw BaseException(
				L"�ړ��A�N�V�������o�^����Ă��܂���",
				L"if (m_MoveVec.size() <= 0)",
				L"Action::GetMoveActiveIndex()"
			);
		}
		return m_MoveActiveIndex;
	}

	//���݂̑��x
	//Move�R���|�[�l���g���Ȃ��ꍇ��bsm::Vec3(0,0,0)��Ԃ�

	bsm::Vec3 Action::GetVelocity()const {
		if (m_MoveVec.size() <= 0) {
			return bsm::Vec3(0, 0, 0);
		}
		else {
			auto Ptr = dynamic_pointer_cast<MoveComponent>(m_MoveVec[m_MoveActiveIndex]);
			if (Ptr) {
				return Ptr->GetVelocity();
			}
			else {
				return bsm::Vec3(0, 0, 0);
			}
		}
	}



	//����
	shared_ptr<ScaleTo> Action::AddScaleTo(float TotalTime, const bsm::Vec3& TargetScale, Lerp::rate Rate) {
		auto Ptr = ObjectFactory::Create<ScaleTo>(GetGameObject());
		Ptr->SetParams(TotalTime, TargetScale, Rate);
		m_ScaleVec.push_back(Ptr);
		return Ptr;
	}
	shared_ptr<ScaleBy> Action::AddScaleBy(float TotalTime, const bsm::Vec3& LocalScale, Lerp::rate Rate) {
		auto Ptr = ObjectFactory::Create<ScaleBy>(GetGameObject());
		Ptr->SetParams(TotalTime, LocalScale, Rate);
		m_ScaleVec.push_back(Ptr);
		return Ptr;
	}

	shared_ptr<ActionInterval> Action::AddScaleInterval(float TotalTime) {
		auto Ptr = ObjectFactory::Create<ActionInterval>(GetGameObject());
		Ptr->SetTotalTime(TotalTime);
		m_ScaleVec.push_back(Ptr);
		return Ptr;
	}

	shared_ptr<ActionComponent> Action::GetScaleComponent(size_t TargetIndex)const {
		if (TargetIndex >= m_ScaleVec.size()) {
			throw BaseException(
				L"�C���f�b�N�X���͈͊O�ł�",
				L"if (TargetIndex >= m_ScaleVec.size())",
				L"Action::GetScaleComponent()"
			);
		}
		return m_ScaleVec[TargetIndex];
	}

	vector<shared_ptr<ActionComponent>>& Action::GetScaleVec() {
		return m_ScaleVec;
	}


	shared_ptr<RotateTo> Action::AddRotateTo(float TotalTime, const bsm::Vec3& TargetRotate) {
		auto Ptr = ObjectFactory::Create<RotateTo>(GetGameObject());
		Ptr->SetParams(TotalTime, TargetRotate);
		m_RotateVec.push_back(Ptr);
		return Ptr;
	}

	shared_ptr<RotateTo> Action::AddRotateTo(float TotalTime, const bsm::Quat& TargetQuaternion) {
		auto Ptr = ObjectFactory::Create<RotateTo>(GetGameObject());
		Ptr->SetParams(TotalTime, TargetQuaternion);
		m_RotateVec.push_back(Ptr);
		return Ptr;
	}


	shared_ptr<RotateBy> Action::AddRotateBy(float TotalTime, const bsm::Vec3& LocalRotate) {
		auto Ptr = ObjectFactory::Create<RotateBy>(GetGameObject());
		Ptr->SetParams(TotalTime, LocalRotate);
		m_RotateVec.push_back(Ptr);
		return Ptr;
	}

	shared_ptr<RotateBy> Action::AddRotateBy(float TotalTime, const bsm::Quat& LocalQuaternion) {
		auto Ptr = ObjectFactory::Create<RotateBy>(GetGameObject());
		Ptr->SetParams(TotalTime, LocalQuaternion);
		m_RotateVec.push_back(Ptr);
		return Ptr;
	}


	shared_ptr<ActionInterval> Action::AddRotateInterval(float TotalTime) {
		auto Ptr = ObjectFactory::Create<ActionInterval>(GetGameObject());
		Ptr->SetTotalTime(TotalTime);
		m_RotateVec.push_back(Ptr);
		return Ptr;
	}

	shared_ptr<ActionComponent> Action::GetRotateComponent(size_t TargetIndex)const {
		if (TargetIndex >= m_RotateVec.size()) {
			throw BaseException(
				L"�C���f�b�N�X���͈͊O�ł�",
				L"if (TargetIndex >= m_RotateVec.size())",
				L"Action::GetRotateComponent()"
			);
		}
		return m_RotateVec[TargetIndex];
	}

	vector<shared_ptr<ActionComponent>>& Action::GetRotateVec() {
		return m_RotateVec;
	}

	shared_ptr<MoveTo> Action::AddMoveTo(float TotalTime, const bsm::Vec3& TargetPosition, Lerp::rate Rate) {
		auto Ptr = ObjectFactory::Create<MoveTo>(GetGameObject());
		Ptr->SetParams(TotalTime, TargetPosition, Rate);
		m_MoveVec.push_back(Ptr);
		return Ptr;
	}

	shared_ptr<MoveTo> Action::AddMoveTo(float TotalTime, const bsm::Vec3& TargetPosition, Lerp::rate RateX, Lerp::rate RateY, Lerp::rate RateZ) {
		auto Ptr = ObjectFactory::Create<MoveTo>(GetGameObject());
		Ptr->SetParams(TotalTime, TargetPosition, RateX, RateY, RateZ);
		m_MoveVec.push_back(Ptr);
		return Ptr;
	}


	shared_ptr<MoveBy> Action::AddMoveBy(float TotalTime, const bsm::Vec3& LocalVector, Lerp::rate Rate) {
		auto Ptr = ObjectFactory::Create<MoveBy>(GetGameObject());
		Ptr->SetParams(TotalTime, LocalVector, Rate);
		m_MoveVec.push_back(Ptr);
		return Ptr;
	}

	shared_ptr<MoveBy> Action::AddMoveBy(float TotalTime, const bsm::Vec3& LocalVector, Lerp::rate RateX, Lerp::rate RateY, Lerp::rate RateZ) {
		auto Ptr = ObjectFactory::Create<MoveBy>(GetGameObject());
		Ptr->SetParams(TotalTime, LocalVector, RateX, RateY, RateZ);
		m_MoveVec.push_back(Ptr);
		return Ptr;
	}


	shared_ptr<ActionInterval> Action::AddMoveInterval(float TotalTime) {
		auto Ptr = ObjectFactory::Create<ActionInterval>(GetGameObject());
		Ptr->SetTotalTime(TotalTime);
		m_MoveVec.push_back(Ptr);
		return Ptr;
	}

	shared_ptr<ActionComponent> Action::GetMoveComponent(size_t TargetIndex)const {
		if (TargetIndex >= m_MoveVec.size()) {
			throw BaseException(
				L"�C���f�b�N�X���͈͊O�ł�",
				L"if (TargetIndex >= m_MoveVec.size())",
				L"Action::GetMoveComponent()"
			);
		}
		return m_MoveVec[TargetIndex];
	}

	vector<shared_ptr<ActionComponent>>& Action::GetMoveVec() {
		return m_MoveVec;
	}
	//���ׂẴA�N�V�������N���A����
	void Action::AllActionClear() {
		m_ScaleVec.clear();
		m_RotateVec.clear();
		m_MoveVec.clear();

		m_Looped = false;
		m_ScaleArrived = false;
		m_RotateArrived = false;
		m_MoveArrived = false;
		m_ScaleActiveIndex = 0;
		m_RotateActiveIndex = 0;
		m_MoveActiveIndex = 0;
	}



	void Action::RunSub(vector<shared_ptr<ActionComponent> >& TgtVector, size_t& TgtIndex) {
		if (TgtVector.size() > 0) {
			TgtIndex = 0;
			TgtVector[TgtIndex]->Run();
		}
	}
	void Action::Run() {
		RunSub(m_ScaleVec, m_ScaleActiveIndex);
		RunSub(m_RotateVec, m_RotateActiveIndex);
		RunSub(m_MoveVec, m_MoveActiveIndex);
	}

	void Action::StopSub(vector<shared_ptr<ActionComponent> >& TgtVector, size_t& TgtIndex) {
		if (TgtVector.size() > 0 && TgtIndex < TgtVector.size()) {
			TgtVector[TgtIndex]->Stop();
		}
	}
	void Action::Stop() {
		StopSub(m_ScaleVec, m_ScaleActiveIndex);
		StopSub(m_RotateVec, m_RotateActiveIndex);
		StopSub(m_MoveVec, m_MoveActiveIndex);
	}

	void Action::ReStartSub(vector<shared_ptr<ActionComponent> >& TgtVector, size_t& TgtIndex) {
		if (TgtVector.size() > 0 && TgtIndex < TgtVector.size()) {
			TgtVector[TgtIndex]->ReStart();
		}
	}

	void Action::ReStart() {
		ReStartSub(m_ScaleVec, m_ScaleActiveIndex);
		ReStartSub(m_RotateVec, m_RotateActiveIndex);
		ReStartSub(m_MoveVec, m_MoveActiveIndex);
	}

	bool Action::UpdateSub(vector<shared_ptr<ActionComponent> >& TgtVector, size_t& TgtIndex) {
		bool ret = false;
		if (TgtVector.size() > 0 && TgtIndex < TgtVector.size()) {
			if (TgtVector[TgtIndex]->IsUpdateActive()) {
				TgtVector[TgtIndex]->OnUpdate();
				if (TgtVector[TgtIndex]->IsArrived()) {
					//��������
					//�C���f�b�N�X�����Z
					TgtIndex++;
					if (TgtIndex >= TgtVector.size()) {
						//�͈͊O�Ȃ�
						if (m_Looped) {
							TgtIndex = 0;
							TgtVector[TgtIndex]->Run();
						}
						else {
							TgtIndex = TgtVector.size() - 1;
							//Run�͍s��Ȃ�
							//��������
							ret = true;
						}
					}
					else {
						TgtVector[TgtIndex]->Run();
					}
				}
			}
		}
		return ret;
	}
	//�X�V
	void Action::OnUpdate() {
		m_ScaleArrived = UpdateSub(m_ScaleVec, m_ScaleActiveIndex);
		m_RotateArrived = UpdateSub(m_RotateVec, m_RotateActiveIndex);
		m_MoveArrived = UpdateSub(m_MoveVec, m_MoveActiveIndex);
	}


}
// end namespace basecross