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
		Vec3 scale;
		Vec3 rotOrigin;
		Quat quaternion;
		Vec3 position;
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
		Mat4x4 world;
		/// �G�~�b�V�u�F
		Col4 emissive;
		/// �f�t���[�Y�F
		Col4 diffuse;
		SpriteConstant()
		{
			memset(this, 0, sizeof(SpriteConstant));
			emissive = Col4(0.0f);
			diffuse = Col4(1.0f, 1.0f, 1.0f, 1.0f);
		};
	};

	//--------------------------------------------------------------------------------------
	///	SimpleConstant�R���X�^���g�o�b�t�@�\����
	//--------------------------------------------------------------------------------------
	struct SimpleConstant
	{
		/// ���[���h�s��
		Mat4x4 world;
		/// �r���[�s��
		Mat4x4 view;
		/// �ˉe�s��
		Mat4x4 projection;
		/// �G�~�b�V�u�F
		Col4 emissive;
		/// �f�t���[�Y�F
		Col4 diffuse;
		/// �X�y�L�����[
		Col4 specular;
		/// �e�N�X�`��=x���A�N�e�B�u���ǂ���
		XMUINT4 activeFlg;
		/// ���C�C�g����
		Vec4 lightDir;
		/// ���C�g�ʒu
		Vec4 lightPos;
		/// eye�̈ʒu
		Vec4 eyePos;
		/// ���C�g�r���[�s��
		Mat4x4 lightView;
		/// ���C�g�ˉe�s��
		Mat4x4 lightProjection;
		/// Bone�z��
		Vec4 Bones[3 * 72];
		SimpleConstant() {
			memset(this, 0, sizeof(SimpleConstant));
			diffuse = Col4(1.0f, 1.0f, 1.0f, 1.0f);
		};
	};



	//--------------------------------------------------------------------------------------
	///	Basic�V�F�[�_�[�p�R���X�^���g�o�b�t�@
	//--------------------------------------------------------------------------------------
	struct BasicConstant
	{
		Col4 diffuseColor;
		Col4 emissiveColor;
		Col4 specularColorAndPower;

		Vec4 lightDirection[3];
		Vec4 lightDiffuseColor[3];
		Vec4 lightSpecularColor[3];

		Vec4 eyePosition;

		Col4 fogColor;
		Vec4 fogVector;

		Mat4x4 world;
		Vec4 worldInverseTranspose[3];
		Mat4x4 worldViewProj;
		//�ėp�t���O
		XMUINT4 activeFlg;
		//�ȉ��e
		Vec4 lightPos;
		Vec4 eyePos;
		Mat4x4 lightView;
		Mat4x4 lightProjection;

		Vec4 bones[3 * 72];
	};


}
// end namespace basecross

