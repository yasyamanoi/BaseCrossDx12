/*!
@file Steering.h
@brief ���Ǌ֘A
*/


#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	class Path;
	//	�p�r: �o�H�N���X
	//--------------------------------------------------------------------------------------
	class Path {
		//�o�H�̃|�C���g�̃��X�g
		std::list<Vec3> m_WayPoints;
		//���݃C�e���[�^�[
		std::list<Vec3>::iterator m_CurWaypoint;
		//���[�v���邩�ǂ���
		bool m_Looped;
	public:
		//--------------------------------------------------------------------------------------
		//	Path();
		//	�p�r: �R���X�g���N�^
		//	�߂�l: �Ȃ�
		//--------------------------------------------------------------------------------------
		Path() :
			m_Looped(false) {
		}
		//--------------------------------------------------------------------------------------
		//	virtual ~Path();
		//	�p�r: �f�X�g���N�^
		//	�߂�l: �Ȃ�
		//--------------------------------------------------------------------------------------
		virtual ~Path() {
			m_WayPoints.clear();
		}
		//--------------------------------------------------------------------------------------
		//�A�N�Z�b�T
		//--------------------------------------------------------------------------------------
		bool GetLooped()const { return m_Looped; }
		std::list<Vec3>* GetPathPtr() { return &m_WayPoints; }
		Vec3 GetCurWaypoint()const {
			if (m_CurWaypoint == m_WayPoints.end()) {
				return m_WayPoints.back();
			}
			else {
				return *m_CurWaypoint;
			}
		}
		void SetLooped(bool b) { m_Looped = b; }
		//--------------------------------------------------------------------------------------
		//	bool IsFinished();
		//	�p�r: �Ō�܂ōs���Ă邩�ǂ���
		//	�߂�l: �Ō�܂ōs���Ă����true
		//--------------------------------------------------------------------------------------
		bool IsFinished() {
			return !(m_CurWaypoint != m_WayPoints.end());
		}
		//--------------------------------------------------------------------------------------
		//	void Clear();
		//	�p�r: ���X�g���N���A����
		//	�߂�l: �Ȃ�
		//--------------------------------------------------------------------------------------
		void Clear() { m_WayPoints.clear(); }
		//--------------------------------------------------------------------------------------
		//	void SetList(
		//	const list<Vec3> new_path	//�V�������X�g
		//	);
		//	�p�r: ���X�g�������ւ���
		//	�߂�l: �Ȃ�
		//--------------------------------------------------------------------------------------
		void SetList(const std::list<Vec3>& new_path) {
			Clear();
			auto it = new_path.begin();
			while (it != new_path.end()) {
				m_WayPoints.push_back(*it);
				it++;
			}
			m_CurWaypoint = m_WayPoints.begin();
		}
		//--------------------------------------------------------------------------------------
		//	void SetNextWaypoint();
		//	�p�r: ���̃|�C���g�ɐi�߂�
		//	�߂�l: �Ȃ�
		//--------------------------------------------------------------------------------------
		void SetNextWaypoint() {
			if (m_WayPoints.size() <= 0) {
				throw BaseException(
					L"���X�g����ł��B",
					L"if(m_WayPoints.size() <= 0)",
					L"Path::SetNextWaypoint()"
				);
			}
			if (m_CurWaypoint == m_WayPoints.end()) {
				return;
			}
			if (++m_CurWaypoint == m_WayPoints.end()) {
				if (m_Looped) {
					m_CurWaypoint = m_WayPoints.begin();
				}
			}
		}
	};




	//--------------------------------------------------------------------------------------
	//	struct Steering;
	//	�p�r: ���Ǌ֘A���[�e�B���e�B
	//	��static�Ăяo��������
	//--------------------------------------------------------------------------------------
	struct Steering {
		Steering() {}
		~Steering() {}
		//--------------------------------------------------------------------------------------
		//	static  bool AccumulateForce(
		//	Vec3& Force,
		//	const Vec3& ForceToAdd,
		//	float MaxForce
		//	);
		//	�p�r: ���Ǘ͂����Z����
		//	�߂�l: ���Z������true
		//	��Force�Ɍv�Z���ʂ����Z����
		//--------------------------------------------------------------------------------------
		static bool AccumulateForce(Vec3& Force, const Vec3& ForceToAdd, float MaxForce);
		//--------------------------------------------------------------------------------------
		//	static Vec3 Seek(
		//	const Vec3& Velocity,	//���݂̑��x
		//	const Vec3& Target,		//�ڕW
		//	const Vec3& Pos,			//���݂̈ʒu
		//	float MaxSpeed				//�ō����x
		//	);
		//	�p�r: �T���s��
		//	�߂�l: �t�H�[�X
		//--------------------------------------------------------------------------------------
		static Vec3 Seek(const Vec3& Velocity, const Vec3& Target, const Vec3& Pos, float MaxSpeed);
		//--------------------------------------------------------------------------------------
		//	static Vec3 Flee(
		//	const Vec3& Velocity,	//���݂̑��x
		//	const Vec3& Target,		//�ڕW
		//	const Vec3& Pos,			//���݂̈ʒu
		//	float MaxSpeed,				//�ō����x
		//	float PanicDistance			//�����o�����������͈͂�1��
		//	);
		//	�p�r: �����s��
		//	�߂�l: �t�H�[�X
		//	���^�[�Q�b�g�� PanicDistance * PanicDistance�̋����͈͂ɓ������瓦���o��
		//--------------------------------------------------------------------------------------
		static Vec3 Flee(const Vec3& Velocity, const Vec3& Target,
			const Vec3& Pos, float MaxSpeed, float PanicDistance);
		//--------------------------------------------------------------------------------------
		//	static Vec3 Arrive(
		//	const Vec3& Velocity,	//���݂̑��x
		//	const Vec3& Target,		//�ڕW
		//	const Vec3& Pos,			//���݂̈ʒu
		//	float MaxSpeed				//�ō����x
		//	float Decl					//�����l�i1.0����3.0�̊Ԓ��x�j
		//	);
		//	�p�r: �����s��
		//	�߂�l: �t�H�[�X
		//--------------------------------------------------------------------------------------
		static Vec3 Arrive(const Vec3& Velocity, const Vec3& Target,
			const Vec3& Pos, float MaxSpeed, float Decl);
		//--------------------------------------------------------------------------------------
		//	static Vec3 Pursuit(
		//	const Vec3& Velocity,	//���݂̑��x
		//	const Vec3& Pos,			//���݂̈ʒu
		//	const Vec3& Rot,			//���݂̌���
		//	float MaxSpeed,				//�ō����x
		//	const Vec3& TargetVelocity,	//�ڕW�̌��݂̑��x
		//	const Vec3& Target,		//�ڕW
		//	const Vec3& TargetRot	//�ڕW�̌���
		//	);
		//	�p�r: �ǐՍs��
		//	�߂�l: �t�H�[�X
		//--------------------------------------------------------------------------------------
		static Vec3 Pursuit(const Vec3& Velocity, const Vec3& Pos, const Vec3& Rot, float MaxSpeed,
			const Vec3& TargetVelocity, const Vec3& Target, const Vec3& TargetRot);
		//--------------------------------------------------------------------------------------
		//	static Vec3 Wander(
		//	const Mat4x4 Matrix,		//���݂̍s��
		//	float WanderRadius,	//�p�j���锼�a
		//	float WanderDistance,	//�p�j���锼�a�܂ł̋���
		//	float WanderJitter,	//�����_���ψق̍ő�l
		//	Vec3& WanderTarget	//�p�j�̃^�[�Q�b�g�ϐ��i��]�l�j
		//	);
		//	�p�r: �p�j�s��
		//	�߂�l: �t�H�[�X
		//--------------------------------------------------------------------------------------
		static Vec3 Wander(const Mat4x4 Matrix,
			float WanderRadius, float WanderDistance, float WanderJitter, Vec3& WanderTarget);
		//--------------------------------------------------------------------------------------
		//	static Vec3 ObstacleAvoidance(
		//	const Mat4x4 Matrix,		//���݂̍s��
		//	const Vec3& Velocity,	//���݂̑��x
		//	float MaxSpeed,				//�ō����x
		//	float Width,				//������铹��
		//	float Height,				//������鍂��
		//	const vector<SPHERE>& SphereVec	//������ׂ����̂̔z��
		//	);
		//	�p�r: ��Q������s��
		//	�߂�l: �t�H�[�X
		//--------------------------------------------------------------------------------------
		static Vec3 ObstacleAvoidance(const Mat4x4 Matrix,
			const Vec3& Velocity, float MaxSpeed, float Width, float Height,
			const std::vector<SPHERE>& SphereVec);
		//--------------------------------------------------------------------------------------
		//	static Vec3 WallAvoidance(
		//	const Mat4x4 Matrix,		//���݂̍s��
		//	const Vec3& Velocity,	//���݂̑��x
		//	float MaxSpeed,				//�ō����x
		//	const vector<PLANE>& PlaneVec	//������ׂ����̂̔z��
		//	);
		//	�p�r: �ǉ���s��
		//	�߂�l: �t�H�[�X
		//--------------------------------------------------------------------------------------
		static Vec3 WallAvoidance(const Mat4x4 Matrix,
			const Vec3& Velocity, float MaxSpeed, const std::vector<PLANE>& PlaneVec);
		//--------------------------------------------------------------------------------------
		//	static Vec3 FollowPath(
		//	Path& rPath,				//�p�X
		//	float WaypointSeekDist,		//Seek�J�n�܂ł̋���
		//	const Vec3& Pos,			//���݂̈ʒu
		//	const Vec3& Velocity,	//���݂̑��x
		//	float MaxSpeed,				//�ō����x
		//	float Decl					//�����l�i1.0����3.0�̊Ԓ��x�j
		//	);
		//	�p�r: �o�H�s��
		//	�߂�l: �t�H�[�X
		//--------------------------------------------------------------------------------------
		static Vec3 FollowPath(Path& rPath,
			float WaypointSeekDist,
			const Vec3& Pos, const Vec3& Velocity,
			float MaxSpeed, float Decl);
		//--------------------------------------------------------------------------------------
		//	static Vec3 Separation(
		//	const shared_ptr<GameObjectGroup>& Group,	//�ݒ肷��O���[�v�̔z��
		//	const shared_ptr<GameObject>& MyObj				//�������g
		//	);
		//	�p�r: �����s��
		//	�߂�l: �t�H�[�X
		//--------------------------------------------------------------------------------------
		static Vec3 Separation(const std::shared_ptr<GameObjectGroup>& Group, const std::shared_ptr<GameObject>& MyObj);
		//--------------------------------------------------------------------------------------
		//	static Vec3 Alignment(
		//	const shared_ptr<GameObjectGroup>& Group,	//�ݒ肷��O���[�v�̔z��
		//	const shared_ptr<GameObject>& MyObj				//�������g
		//	);
		//	�p�r: ����s��
		//	�߂�l: �t�H�[�X
		//--------------------------------------------------------------------------------------
		static Vec3 Alignment(const std::shared_ptr<GameObjectGroup>& Group, const std::shared_ptr<GameObject>& MyObj);
		//--------------------------------------------------------------------------------------
		//	static Vec3 Cohesion(
		//	const shared_ptr<GameObjectGroup>& Group,	//�ݒ肷��O���[�v�̔z��
		//	const shared_ptr<GameObject>& MyObj			//�������g
		//	const Vec3& Velocity,	//���݂̑��x
		//	float MaxSpeed				//�ō����x
		//	);
		//	�p�r: �����s��
		//	�߂�l: �t�H�[�X
		//--------------------------------------------------------------------------------------
		static Vec3 Cohesion(const std::shared_ptr<GameObjectGroup>& Group, const std::shared_ptr<GameObject>& MyObj,
			const Vec3& Velocity, float MaxSpeed);
	};

}
//end namespace basecross
