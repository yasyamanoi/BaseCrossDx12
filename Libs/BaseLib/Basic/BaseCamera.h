/*!
@file BaseCamera.h
@brief �J�����N���X
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/
#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	�J�����N���X
	//--------------------------------------------------------------------------------------
	class BaseCamera : public ObjectInterface {
		bsm::Vec3 m_eye;
		bsm::Vec3 m_at;
		bsm::Vec3 m_up;
		float m_near;
		float m_far;
		bool m_RH;
	protected:
		//�r���[�s��
		bsm::Mat4x4 m_viewMatrix;
		//�ˉe�s��
		bsm::Mat4x4 m_projMatrix;
		//�v���e�N�g�R���X�g���N�^
		BaseCamera();
		//�v���e�N�g�f�X�g���N�^
		virtual ~BaseCamera() {}
	public:
		//RH�@���ǂ����𓾂�
		bool GetRH() const;
		//RH�@���ǂ����ݒ肷��
		void SetRH(bool b);
		//�J�����̈ʒu�𓾂�
		const bsm::Vec3& GetEye() const;
		//�J�����̈ʒu��ݒ肷��
		void SetEye(const bsm::Vec3& eye);
		//�J�����̈ʒu��ݒ肷��
		void SetEye(float x, float y, float z);
		//�J�����̎��_�𓾂�
		const bsm::Vec3& GetAt() const;
		//�J�����̎��_��ݒ肷��
		void SetAt(const bsm::Vec3& at);
		//�J�����̎��_��ݒ肷��
		void SetAt(float x, float y, float z);
		//�J�����̌X���𓾂�
		const bsm::Vec3& GetUp() const;
		//�J�����̌X����ݒ肷��
		void SetUp(const bsm::Vec3& up);
		//�J�����̌X����ݒ肷��
		void SetUp(float x, float y, float z);
		//Near�����𓾂�
		float GetNear() const;
		//Near������ݒ肷��
		void SetNear(float f);
		//Far�����𓾂�
		float GetFar() const;
		//Far������ݒ肷��
		void SetFar(float f);
		//�s����v�Z����
		virtual void CalculateMatrix() = 0;
		//�r���[�s��𓾂�
		virtual const bsm::Mat4x4& GetViewMatrix();
		//�ˉe�s��𓾂�
		virtual const bsm::Mat4x4& GetProjMatrix();
		//����������
		virtual void OnCreate()override;
		//�X�V����
		virtual void OnUpdate()override;
		//�`�揈���B�f�t�H���g�͉����s��Ȃ�
		virtual void OnDraw()override {}
		//�I�������B�f�t�H���g�͉����s��Ȃ�
		virtual void OnDestroy()override {}
	};

	//--------------------------------------------------------------------------------------
	//	���ߖ@�J����
	//--------------------------------------------------------------------------------------
	class PerspecCamera : public BaseCamera {
		float m_fovY;   //�ˉe�p�x
		float m_aspect;	//�A�X�y�N�g��
	public:
		PerspecCamera();
		virtual ~PerspecCamera() {}
		//����������
		virtual void OnCreate()override;
		//��p�𓾂�i���ߖ@���ݒ肳��Ă���Ƃ��p�j
		float GetFovY() const;
		//��p��ݒ肷��i���ߖ@���ݒ肳��Ă���Ƃ��p�j
		void SetFovY(float f);
		//�A�X�y�N�g��𓾂�i���ߖ@���ݒ肳��Ă���Ƃ��p�j
		float GetAspect() const;
		//�A�X�y�N�g���ݒ肷��i���ߖ@���ݒ肳��Ă���Ƃ��p�j
		void SetAspect(float f);
		//�s����v�Z����
		virtual void CalculateMatrix()override;
	};

	//--------------------------------------------------------------------------------------
	//	���ˉe�J����
	//--------------------------------------------------------------------------------------
	class OrthoCamera : public BaseCamera {
		float m_width;	//���ߖ@���g��Ȃ��ꍇ�i���ˉe�j�̕�
		float m_height;	//���ߖ@���g��Ȃ��ꍇ�i���ˉe�j�̍���
	public:
		OrthoCamera();
		virtual ~OrthoCamera() {}
		//����������
		virtual void OnCreate()override;
		//��ʂ̕��𓾂�i���ߖ@���ݒ肳��Ȃ��Ƃ��p�j
		float GetWidth() const;
		//��ʂ̕���ݒ肷��i���ߖ@���ݒ肳��Ȃ��Ƃ��p�j
		void SetWidth(float f);
		//��ʂ̍����𓾂�i���ߖ@���ݒ肳��Ȃ��Ƃ��p�j
		float GetHeight() const;
		//��ʂ̍�����ݒ肷��i���ߖ@���ݒ肳��Ȃ��Ƃ��p�j
		void SetHeight(float f);
		//�s����v�Z����
		virtual void CalculateMatrix()override;
	};
}
using namespace basecross;
// end namespace basecross