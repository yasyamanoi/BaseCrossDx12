/*!
@file Component.h
@brief �R���|�[�l���g�e
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	class GameObject;
	class BaseStage;

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
			emissive = Col4(0.0f);
			diffuse = Col4(1.0f, 1.0f, 1.0f, 1.0f);
		};
	};


	//--------------------------------------------------------------------------------------
	///	Shadow�R���X�^���g�o�b�t�@�\����
	//--------------------------------------------------------------------------------------
	struct ShadowConstant
	{
		/// ���[���h�s��
		bsm::Mat4x4 world;
		/// �r���[�s��
		bsm::Mat4x4 view;
		/// �ˉe�s��
		Mat4x4 projection;
		/// bone�p
		bsm::Vec4 bones[3 * 72];
		ShadowConstant() {
			memset(this, 0, sizeof(ShadowConstant));
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
	struct BasicConstants
	{
		Vec4 diffuseColor;
		Vec4 emissiveColor;
		Vec4 specularColorAndPower;

		Vec4 lightDirection[3];
		Vec4 lightDiffuseColor[3];
		Vec4 lightSpecularColor[3];

		Vec4 eyePosition;

		Vec4 fogColor;
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


	//--------------------------------------------------------------------------------------
	///	�R���|�[�l���g�e�N���X
	//--------------------------------------------------------------------------------------
	class Component :public ObjectInterface {
		weak_ptr<GameObject> m_gameObject;
		bool m_updateActive;
		bool m_renderActive;
		wstring m_samplerKey;
	protected:
		//�t���[���p�����[�^�̃C���f�b�N�X(�R���X�^���g�o�b�t�@�Ŏg��)
		size_t m_frameParamIndex;
		explicit Component(const shared_ptr<GameObject>& gameObjectPtr);
		virtual ~Component() {}
		shared_ptr<BaseMesh> m_baseMesh;
		shared_ptr<BaseTexture> m_baseTexture;
		virtual void PopulateCommandList(BaseFrame* pBaseFrame) {}
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���b�V���̎擾
		@return	���b�V��
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<BaseMesh> GetMesh() const {
			return m_baseMesh;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���b�V���̐ݒ�
		@param[in]	mesh	���b�V��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetMesh(const shared_ptr<BaseMesh>& mesh) {
			m_baseMesh = mesh;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���b�V���̐ݒ�
		@param[in]	key	���b�V���̃��\�[�X�L�[
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetMesh(const wstring& key) {
			SetMesh(App::GetBaseScene()->GetMesh(key));
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���_�̕ύX
		@tparam	T	���_�̌^
		@param[in]	vertices	���_�̔z��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		void UpdateVertices( const vector<T>& vertices) {
			if (!m_baseMesh) {
				throw BaseException(
					L"���b�V�����L���ł͂���܂���",
					L"if (!m_baseMesh)",
					L"Component::UpdateVirtexBuffer()"
				);
			}
			auto comList = App::GetBaseDevice()->GetComandList();
			m_baseMesh->UpdateVirtexBuffer<T>(comList, vertices);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�e�N�X�`���̎擾
		@return	�e�N�X�`��
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<BaseTexture> GetTexture() const {
			return m_baseTexture;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�e�N�X�`���̐ݒ�
		@param[in]	baseTexture	�e�N�X�`��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetTexture(const shared_ptr<BaseTexture>& baseTexture) {
			m_baseTexture = baseTexture;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�e�N�X�`���̐ݒ�
		@param[in]	key	�e�N�X�`���̃��\�[�X�L�[
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetTexture(const wstring& key) {
			SetTexture(App::GetBaseScene()->GetTexture(key));
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�T���v���[�L�[�̎擾
		@return	�T���v���[�L�[
		*/
		//--------------------------------------------------------------------------------------
		wstring GetSamplerKey() const {
			return m_samplerKey;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�T���v���[�L�[�̐ݒ�
		@param[in]	key	�T���v���[�L�[
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetSamplerKey(const wstring& key) {
			m_samplerKey = key;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Q�[���I�u�W�F�N�g�𓾂�
		@return	�Q�[���I�u�W�F�N�g
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<GameObject> GetGameObject() const {
			auto ptr = m_gameObject.lock();
			if (!ptr) {
				throw BaseException(
					L"GameObject�͗L���ł͂���܂���",
					L"if (!shptr)",
					L"Component::GetGameObject()"
				);
			}
			else {
				return ptr;
			}
			return nullptr;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�e�[�W�𓾂�
		@return	�X�e�[�W
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<Stage> GetStage() const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	update���L�����ǂ���
		@return	�L���Ȃ�true
		*/
		//--------------------------------------------------------------------------------------
		bool IsUpdateActive() const {
			return m_updateActive;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	update���L�����ǂ����̐ݒ�
		@param[in]	b	true��false
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetUpdateActive(bool b) {
			m_updateActive = b;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	render���L�����ǂ���
		@return	�L���Ȃ�true
		*/
		//--------------------------------------------------------------------------------------
		bool IsRenderActive() const {
			return m_renderActive;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	render���L�����ǂ����̐ݒ�
		@param[in]	b	true��false
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetRenderActive(bool b) {
			m_renderActive = b;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�t���[���p�����[�^�̃C���f�b�N�X�𓾂�
		@return	�t���[���p�����[�^�̃C���f�b�N�X
		*/
		//--------------------------------------------------------------------------------------
		size_t GetFrameParamIndex() const {
			return m_frameParamIndex;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Q�[���I�u�W�F�N�g�̐ݒ�
		@param[in]	GameObjectPtr	�Q�[���I�u�W�F�N�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void AttachGameObject(const shared_ptr<GameObject>& GameObjectPtr);

		virtual void OnInitFrame(BaseFrame* pBaseFrame) {}
		virtual void WriteConstantBuffers(BaseFrame* pBaseFrame){}
		virtual void OnRender()override;
	};



}
//end basecross