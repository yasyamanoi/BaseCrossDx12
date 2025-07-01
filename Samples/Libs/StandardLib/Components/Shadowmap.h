/*!
@file ShadowmapComp.h
@brief �V���h�E�}�b�v�R���|�[�l���g
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	Shadow�R���X�^���g�o�b�t�@�\����
	//--------------------------------------------------------------------------------------
	struct ShadowConstantBuffer
	{
		/// ���[���h�s��
		Mat4x4 world;
		/// �r���[�s��
		Mat4x4 view;
		/// �ˉe�s��
		Mat4x4 projection;
		/// bone�p
		Vec4 bones[3 * 72];
		ShadowConstantBuffer() {
			memset(this, 0, sizeof(ShadowConstantBuffer));
		};
	};

	//--------------------------------------------------------------------------------------
	///	Shadowmap�`��
	//--------------------------------------------------------------------------------------
	DECLARE_DX12SHADER(PNTShadowmap)

	//--------------------------------------------------------------------------------------
	///	�V���h�E�}�b�v�R���|�[�l���g
	//--------------------------------------------------------------------------------------
	class Shadowmap : public Component {
		static float m_lightHeight;
		static float m_lightNear;
		static float m_lightFar;
		static float m_viewWidth;
		static float m_viewHeight;
		static float m_posAdjustment;
	protected:
		ShadowConstantBuffer m_shadowConstantBuffer;
		size_t m_shadowConstantBufferIndex;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���C�g�̍����𓾂�
		@return	���C�g�̍���
		*/
		//--------------------------------------------------------------------------------------
		static float GetLightHeight() {
			return m_lightHeight;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���C�g��Near�l�𓾂�
		@return	���C�g��Near�l
		*/
		//--------------------------------------------------------------------------------------
		static float GetLightNear() {
			return m_lightNear;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���C�g��Far�l�𓾂�
		@return	���C�g��Far�l
		*/
		//--------------------------------------------------------------------------------------
		static float GetLightFar() {
			return m_lightFar;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�r���[�̕��𓾂�
		@return	�r���[�̕�
		*/
		//--------------------------------------------------------------------------------------
		static float GetViewWidth() {
			return m_viewWidth;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�r���[�̍����𓾂�
		@return	�r���[�̍���
		*/
		//--------------------------------------------------------------------------------------
		static float GetViewHeight() {
			return m_viewHeight;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�e�̈ʒu�����l�iY�����j�𓾂�
		@return	�e�̈ʒu�����l�iY�����j
		*/
		//--------------------------------------------------------------------------------------
		static float GetPosAdjustment() {
			return m_posAdjustment;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���C�g�̍�����ݒ肷��
		@param[in]	f	����
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		static void SetLightHeight(float f) {
			m_lightHeight = f;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���C�g��Near�l��ݒ肷��
		@param[in]	f	Near�l
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		static void SetLightNear(float f) {
			m_lightNear = f;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���C�g��Far�l��ݒ肷��
		@param[in]	f	Far�l
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		static void SetLightFar(float f) {
			m_lightFar = f;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�r���[�̕���ݒ肷��
		@param[in]	f	�r���[�̕�
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		static void SetViewWidth(float f) {
			m_viewWidth = f;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�r���[�̍�����ݒ肷��
		@param[in]	f	�r���[�̍���
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		static void SetViewHeight(float f) {
			m_viewHeight = f;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�r���[�̕��y�э����𓯂��l�ɐݒ肷��
		@param[in]	f	�r���[�̕��y�э����i���l�j
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		static void SetViewSize(float f) {
			m_viewWidth = m_viewHeight = f;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�e�̈ʒu�����l�iY�����j��ݒ肷��
		@param[in]	f	�e�̈ʒu�����l�iY�����j
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		static void SetPosAdjustment(float f) {
			m_posAdjustment = f;
		}
		Shadowmap(const std::shared_ptr<GameObject>& gameObjectPtr);
		virtual ~Shadowmap() {}
		virtual void OnUpdateConstantBuffers()override;
		virtual void OnCommitConstantBuffers()override;
		virtual void OnCreate()override;
		virtual void OnShadowDraw()override;
	};

}
// end namespace basecross

