/*!
@file Constants.h
@brief �R���X�^���g�o�b�t�@���܂Ƃ�
*/
#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	�I�u�W�F�N�g�ϊ��p
	//--------------------------------------------------------------------------------------
	struct TransParam {
		bsm::Vec3 scale;
		bsm::Vec3 rotOrigin;
		bsm::Quat quaternion;
		bsm::Vec3 position;
		TransParam() :
			scale(1.0f),
			rotOrigin(0.0f),
			quaternion(),
			position(0.0f)
		{
		}
	};

	//--------------------------------------------------------------------------------------
	///	�X�v���C�g�p�R���X�^���g�o�b�t�@�\����
	//--------------------------------------------------------------------------------------
	struct SpriteConstant
	{
		/// ���[���h�s��
		bsm::Mat4x4 world;
		/// �G�~�b�V�u�F
		bsm::Col4 emissive;
		/// �f�t���[�Y�F
		bsm::Col4 diffuse;
		SpriteConstant()
		{
			memset(this, 0, sizeof(SpriteConstant));
			emissive = bsm::Col4(0.0f);
			diffuse = bsm::Col4(1.0f, 1.0f, 1.0f, 1.0f);
		};
	};

	//--------------------------------------------------------------------------------------
	///	SimpleConstant�R���X�^���g�o�b�t�@�\����
	//--------------------------------------------------------------------------------------
	struct SimpleConstant
	{
		/// ���[���h�s��
		bsm::Mat4x4 world;
		/// �r���[�s��
		bsm::Mat4x4 view;
		/// �ˉe�s��
		bsm::Mat4x4 projection;
		/// �G�~�b�V�u�F
		bsm::Col4 emissive;
		/// �f�t���[�Y�F
		bsm::Col4 diffuse;
		/// �X�y�L�����[
		bsm::Col4 specular;
		/// �e�N�X�`��=x���A�N�e�B�u���ǂ���
		XMUINT4 activeFlg;
		/// ���C�C�g����
		bsm::Vec4 lightDir;
		/// ���C�g�ʒu
		bsm::Vec4 lightPos;
		/// eye�̈ʒu
		bsm::Vec4 eyePos;
		/// ���C�g�r���[�s��
		bsm::Mat4x4 lightView;
		/// ���C�g�ˉe�s��
		bsm::Mat4x4 lightProjection;
		/// Bone�z��
		bsm::Vec4 Bones[3 * 72];
		SimpleConstant() {
			memset(this, 0, sizeof(SimpleConstant));
			diffuse = bsm::Col4(1.0f, 1.0f, 1.0f, 1.0f);
		};
	};



	//--------------------------------------------------------------------------------------
	///	Basic�V�F�[�_�[�p�R���X�^���g�o�b�t�@
	//--------------------------------------------------------------------------------------
	struct BasicConstant
	{
		bsm::Col4 diffuseColor;
		bsm::Col4 emissiveColor;
		bsm::Col4 specularColorAndPower;

		bsm::Vec4 lightDirection[3];
		bsm::Vec4 lightDiffuseColor[3];
		bsm::Vec4 lightSpecularColor[3];

		bsm::Vec4 eyePosition;

		bsm::Col4 fogColor;
		bsm::Vec4 fogVector;

		bsm::Mat4x4 world;
		bsm::Vec4 worldInverseTranspose[3];
		bsm::Mat4x4 worldViewProj;
		//�ėp�t���O
		XMUINT4 activeFlg;
		//�ȉ��e
		bsm::Vec4 lightPos;
		bsm::Vec4 eyePos;
		bsm::Mat4x4 lightView;
		bsm::Mat4x4 lightProjection;

		bsm::Vec4 bones[3 * 72];
	};


}
// end namespace basecross

