/*!
@file BaseTexture.h
@brief �e�N�X�`���N���X
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	�e�N�X�`��
	//--------------------------------------------------------------------------------------
	class BaseTexture {
		ComPtr<ID3D12Resource> m_texture; //�e�N�X�`�����\�[�X
		ComPtr<ID3D12Resource> m_textureUploadHeap; //�e�N�X�`���̃A�b�v���[�h�q�[�v
		DirectX::ScratchImage m_image; //�C���[�W
		std::vector<D3D12_SUBRESOURCE_DATA> m_subresources; //�T�u���\�[�X
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_maphandle; //�}�b�v�n���h��
		UINT m_srvIndex; //�V�F�[�_�[���\�[�X�r���[�̃C���f�b�N�X
		BaseTexture() {}
		//�t�@�C������e�X�N�`���쐬(SRV���쐬����)
		static std::shared_ptr<BaseTexture>
			CreateBaseTextureFromFilePrim(ID3D12GraphicsCommandList* pCommandList, const std::wstring& fileName, const CD3DX12_CPU_DESCRIPTOR_HANDLE& mapHandle);
	public:
		~BaseTexture() {}
		//�e�N�X�`���̎擾
		ComPtr<ID3D12Resource> GetTexture() const {
			return m_texture;
		}
		//�e�N�X�`���̃A�b�v���[�h�q�[�v�̎擾
		ComPtr<ID3D12Resource> GetTextureUploadHeap() const {
			return m_textureUploadHeap;
		}
		//�}�b�v�n���h���̎擾
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetMapHandle() const {
			return m_maphandle;
		}
		//�V�F�[�_�[���\�[�X�r���[�C���f�b�N�X�̐ݒ�
		void SetSrvIndex(UINT srvIndex) {
			m_srvIndex = srvIndex;
		}
		//�V�F�[�_�[���\�[�X�r���[�C���f�b�N�X�̎擾
		UINT GetSrvIndex() const {
			return m_srvIndex;
		}
		//�T�u���\�[�X�̃A�b�v�f�[�g�ƃV�F�[�_���\�[�X�r���[�̍쐬
		void UpdateSRAndCreateSRV(ID3D12GraphicsCommandList* pCommandList);
		//�t�@�C������e�X�N�`���쐬(SRV���쐬����)
		static std::shared_ptr<BaseTexture> CreateTextureFlomFile(ID3D12GraphicsCommandList* pCommandList, const std::wstring& falsename);
	};
}
//end basecross
