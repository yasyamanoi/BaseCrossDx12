/*!
@file Scene.h
@brief �V�[���N���X�B�K�v�ł���΁ABaseScene���Ē�`
*/

#pragma once

#include "stdafx.h"

using namespace bsm;
namespace basecross {

	//--------------------------------------------------------------------------------------
	//	�V�F�[�_�[
	//--------------------------------------------------------------------------------------
	DECLARE_DX12SHADER(VSPCSprite)
	DECLARE_DX12SHADER(PSPCSprite)
	//--------------------------------------------------------------------------------------
	//	�V�[��
	//--------------------------------------------------------------------------------------
	class Scene : public BaseScene
	{
		std::shared_ptr<PcSprite> m_pcSprite;
	public:
		Scene(UINT frameCount, PrimDevice* pPrimDevice);
		virtual ~Scene();
		//--------------------------------------------------------------------------------------
		// �X�V����
		//--------------------------------------------------------------------------------------
		virtual void Update(double elapsedTime)override;
		//--------------------------------------------------------------------------------------
		// �f�o�b�O������̍X�V
		// �������Ȃ�
		//--------------------------------------------------------------------------------------
		virtual void UpdateUI(std::wstring& uiText)override{}
	protected:
		//--------------------------------------------------------------------------------------
		// D3D���\�[�X�̍쐬
		//--------------------------------------------------------------------------------------
		virtual void CreatePipelineStates(ID3D12Device* pDevice)override;
		virtual void CreateSamplers(ID3D12Device* pDevice)override {}
		//--------------------------------------------------------------------------------------
		// �I�u�W�F�N�g�쐬
		//--------------------------------------------------------------------------------------
		virtual void CreateSceneResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)override;
		//--------------------------------------------------------------------------------------
		// �`�揈���i�V���h�E�j
		// �������Ȃ�
		//--------------------------------------------------------------------------------------
		virtual void ShadowPass(ID3D12GraphicsCommandList* pCommandList)override{}
		//--------------------------------------------------------------------------------------
		// �`�揈���i�V�[���j
		//--------------------------------------------------------------------------------------
		virtual void ScenePass(ID3D12GraphicsCommandList* pCommandList)override;
		//--------------------------------------------------------------------------------------
		//�`�揈���i�|�X�g�v���Z�X�j
		//�������Ȃ�
		//--------------------------------------------------------------------------------------
		virtual void PostprocessPass(ID3D12GraphicsCommandList* pCommandList)override{}
	};
}
using namespace basecross;
// end namespace basecross