/*!
@file Character.h
@brief �z�u�I�u�W�F�N�g
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// �{�b�N�X�I�u�W�F�N�g
	//--------------------------------------------------------------------------------------
	class FixedBox : public GameObject {
	public:
		FixedBox(const std::shared_ptr<Stage>& stage, const TransParam& param);
		virtual ~FixedBox();
		virtual void OnCreate()override;
		virtual void OnUpdate(double elapsedTime)override {}
	};

	//--------------------------------------------------------------------------------------
	// �l�p�̃I�u�W�F�N�g
	//--------------------------------------------------------------------------------------
	class WallBox : public  GameObject {
		double m_totalTime;
	protected:
	public:
		WallBox(const std::shared_ptr<Stage>& stage, const TransParam& param);
		virtual ~WallBox();
		virtual void OnCreate();
		virtual void OnUpdate(double elapsedTime);
	};

	//--------------------------------------------------------------------------------------
	//	�ǂ�������z�u�I�u�W�F�N�g
	//--------------------------------------------------------------------------------------

	class SeekObject : public GameObject {
		//�X�e�[�g�}�V�[��
		std::unique_ptr< StateMachine<SeekObject> >  m_StateMachine;
		Vec3 m_StartPos;
		float m_StateChangeSize;
		//�t�H�[�X
		Vec3 m_Force;
		//���x
		Vec3 m_Velocity;
	public:
		//�\�z�Ɣj��
		SeekObject(const std::shared_ptr<Stage>& StagePtr, const Vec3& startPos);
		virtual ~SeekObject();
		//������
		virtual void OnCreate() override;
		//�A�N�Z�T
		const std::unique_ptr<StateMachine<SeekObject>>& GetStateMachine() {
			return m_StateMachine;
		}
		float GetStateChangeSize() const {
			return m_StateChangeSize;
		}
		const Vec3& GetForce()const {
			return m_Force;
		}
		void SetForce(const Vec3& f) {
			m_Force = f;
		}
		void AddForce(const Vec3& f) {
			m_Force += f;
		}
		const Vec3& GetVelocity()const {
			return m_Velocity;
		}
		void SetVelocity(const Vec3& v) {
			m_Velocity = v;
		}
		void ApplyForce();
		Vec3 GetTargetPos()const;
		//����
		virtual void OnUpdate(double elapsedTime) override;
	};

	//--------------------------------------------------------------------------------------
	//	class SeekFarState : public ObjState<SeekObject>;
	//	�p�r: �v���C���[���牓���Ƃ��̈ړ�
	//--------------------------------------------------------------------------------------
	class SeekFarState : public ObjState<SeekObject>
	{
		SeekFarState() {}
	public:
		static std::shared_ptr<SeekFarState> Instance();
		virtual void Enter(const std::shared_ptr<SeekObject>& Obj)override;
		virtual void Execute(const std::shared_ptr<SeekObject>& Obj)override;
		virtual void Exit(const std::shared_ptr<SeekObject>& Obj)override;
	};

	//--------------------------------------------------------------------------------------
	//	class SeekNearState : public ObjState<SeekObject>;
	//	�p�r: �v���C���[����߂��Ƃ��̈ړ�
	//--------------------------------------------------------------------------------------
	class SeekNearState : public ObjState<SeekObject>
	{
		SeekNearState() {}
	public:
		static std::shared_ptr<SeekNearState> Instance();
		virtual void Enter(const std::shared_ptr<SeekObject>& Obj)override;
		virtual void Execute(const std::shared_ptr<SeekObject>& Obj)override;
		virtual void Exit(const std::shared_ptr<SeekObject>& Obj)override;
	};


}
// end namespace basecross
