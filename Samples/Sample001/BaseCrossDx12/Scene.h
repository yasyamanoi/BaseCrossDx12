/*!
@file Scene.h
@brief �V�[���N���X
*/
#pragma once
#include "stdafx.h"
#include "Project.h"

namespace basecross {


	DECLARE_DX12SHADER(SpVSPCStatic)
	DECLARE_DX12SHADER(SpPSPCStatic)

	//--------------------------------------------------------------------------------------
	// �V�[��
	//--------------------------------------------------------------------------------------
	class Scene : public BaseScene
	{
		SimpleConstant m_constantBuffer;
		size_t m_constantBufferIndex;
		std::shared_ptr<BaseMesh> m_mesh;
		double m_totalTime;
		TransParam m_param;
		std::shared_ptr<Camera> m_camera;
		std::shared_ptr<LightSet> m_lightSet;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R���X�g���N�^
		@param[in]	frameCount	�t���[����
		@param[in]	pPrimDevice	�f�o�C�X
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		Scene(UINT frameCount, PrimDevice* pPrimDevice);
		virtual ~Scene();
	protected:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R���e���c�̍쐬
		@param[in]	pDevice	�f�o�C�X
		@param[in]	pCommandList	�R�}���h���X�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void CreateAssetResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�V����
		@param[in]	elapsedTime	�O�̃^�[������̎���
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void Update(double elapsedTime)override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R���X�^���g�o�b�t�@�̍X�V
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void UpdateConstantBuffers()override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R���X�^���g�o�b�t�@�̃R�~�b�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void CommitConstantBuffers()override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	UI�̍X�V
		@param[in]	uiLayer	UI���C���[
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void UpdateUI(std::unique_ptr<UILayer>& uiLayer)override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�V���h�E�}�b�v�̕`��
		@param[in]	�R�}���h���X�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void ShadowPass(ID3D12GraphicsCommandList* pCommandList)override {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�V�[���̕`��
		@param[in]	�R�}���h���X�g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void ScenePass(ID3D12GraphicsCommandList* pCommandList);
	};
}
//end namespace basecross
