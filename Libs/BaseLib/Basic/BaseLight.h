/*!
@file BaseLight.h
@brief ���C�g�N���X
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once
#include "stdafx.h"


namespace basecross {

	//--------------------------------------------------------------------------------------
	//	���C�g
	//--------------------------------------------------------------------------------------
	struct BaseLight {
		Vec3 m_directional;	//���C�g�̌���
		Vec4 m_diffuseColor;	//�f�B�t�B�[�Y�F
		Vec4 m_specularColor;	//�X�y�L�����[�F
		Vec3 m_position;	//�ʒu�i�g�p���Ȃ��ꍇ����j
		Vec3 m_at;		//�w���Ă���ꏊ�i�g�p���Ȃ��ꍇ����j

		BaseLight() :
			m_directional(0, -1.0f, 0),
			m_diffuseColor(1.0f, 1.0f, 1.0f, 1.0f),
			m_specularColor(0.2f, 0.2f, 0.2f, 1.0f),
			m_position(0.0f),
			m_at(0.0f)
		{
		}
		BaseLight(const Vec3& dir, const Vec4& def, const Vec4& sp) :
			m_directional(dir),
			m_diffuseColor(def),
			m_specularColor(sp),
			m_position(0.0f),
			m_at(0.0f)
		{
		}
		BaseLight(const BaseLight& other) :
			m_directional(other.m_directional),
			m_diffuseColor(other.m_diffuseColor),
			m_specularColor(other.m_specularColor),
			m_position(other.m_position),
			m_at(other.m_at)
		{
		}
		BaseLight& operator=(const BaseLight& other) {
			if (this != &other) {
				m_directional = other.m_directional;
				m_diffuseColor = other.m_diffuseColor;
				m_specularColor = other.m_specularColor;
				m_position = other.m_position;
				m_at = other.m_at;
			}
			return *this;
		}
		~BaseLight() {}
		void SetPositionToDirectional(const Vec3& pos, const Vec3& at) {
			m_position = pos;
			m_at = at;
			Vec3 dir = at - pos;
			dir.normalize();
			m_directional = dir;
		}
	};

	//--------------------------------------------------------------------------------------
	//	���C�g�̃Z�b�g
	//--------------------------------------------------------------------------------------
	class LightSet {
		const size_t m_maxLights = 3;
		vector<BaseLight> m_lights;
		Vec4 m_ambient;
		size_t m_mainIndex;
		LightSet() :m_mainIndex(0) {}
	public:
		virtual ~LightSet() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief ���C�g�̎擾
		@param[in]	index	�C���f�b�N�X
		@return	�w��̃C���f�b�N�X�̃��C�g��Ԃ�
		*/
		//--------------------------------------------------------------------------------------
		BaseLight GetLight(size_t index) const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief ���C�g���̎擾
		@return	���C�g��
		*/
		//--------------------------------------------------------------------------------------
		size_t GetNumLights() const {
			return m_lights.size();
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief ���C�g�̐ݒ�
		@param[in]	index	�C���f�b�N�X
		@param[in]	light	�ݒ肷�郉�C�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetLight(size_t index, const BaseLight& light);
		//--------------------------------------------------------------------------------------
		/*!
		@brief ���C�g�̒ǉ�
		@param[in]	light	�ǉ����郉�C�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void AddLight(const BaseLight& light);
		//--------------------------------------------------------------------------------------
		/*!
		@brief �A���r�G���g�̎擾
		@return	�A���r�G���g
		*/
		//--------------------------------------------------------------------------------------
		Vec4 GetAmbient() const { return m_ambient; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief �A���r�G���g�̐ݒ�
		@param[in]	a	�ݒ肷��A���r�G���g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetAmbient(const Vec4& a) { m_ambient = a; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief ���C�����C�g�C���f�b�N�X�̎擾
		@return	���C�����C�g�C���f�b�N�X
		*/
		//--------------------------------------------------------------------------------------
		size_t GetMainIndex() const { return m_mainIndex; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief ���C�����C�g�C���f�b�N�X�̐ݒ�
		@param[in]	index	�ݒ肷��C���f�b�N�X
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetMainIndex(size_t index) { m_mainIndex = index; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief ���C�����C�g�̎擾
		@return	���C�����C�g
		*/
		//--------------------------------------------------------------------------------------
		BaseLight GetMainBaseLight() const {
			return GetLight(GetMainIndex());
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief �f�t�H���g�̃��C�e�B���O���������C�g�Z�b�g�̎擾
		@return	���C�g�Z�b�g�̃|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<LightSet>
			CreateDefaultLights();
	};



}
// end basecross