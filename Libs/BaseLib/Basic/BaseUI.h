/*!
@file BaseUI.h
@brief ������\���N���X
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	������\��
	//--------------------------------------------------------------------------------------
	class BaseUI
	{
	public:
		BaseUI(UINT frameCount, ID3D12Device* pDevice, ID3D12CommandQueue* pCommandQueue);
		//������̍X�V
		void UpdateLabels(const std::wstring& uiText);
		//������̕\��
		void Render(UINT frameIndex);
		//���\�[�X�̉��
		void ReleaseResources();
		//�\���̈�T�C�Y�̕ύX
		void Resize(Microsoft::WRL::ComPtr<ID3D12Resource>* ppRenderTargets, UINT width, UINT height);

	private:
		//�t���[�����𓾂�
		UINT FrameCount() { return static_cast<UINT>(m_wrappedRenderTargets.size()); }
		//������
		void Initialize(ID3D12Device* pDevice, ID3D12CommandQueue* pCommandQueue);

		// �`��^�[�Q�b�g���
		float m_width;
		float m_height;

		struct TextBlock
		{
			std::wstring text;
			D2D1_RECT_F layout;
			IDWriteTextFormat* pFormat;
		};

		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3d11DeviceContext;
		Microsoft::WRL::ComPtr<ID3D11On12Device> m_d3d11On12Device;
		Microsoft::WRL::ComPtr<IDWriteFactory> m_dwriteFactory;
		Microsoft::WRL::ComPtr<ID2D1Factory3> m_d2dFactory;
		Microsoft::WRL::ComPtr<ID2D1Device2> m_d2dDevice;
		Microsoft::WRL::ComPtr<ID2D1DeviceContext2> m_d2dDeviceContext;
		std::vector<Microsoft::WRL::ComPtr<ID3D11Resource>> m_wrappedRenderTargets;
		std::vector<Microsoft::WRL::ComPtr<ID2D1Bitmap1>> m_d2dRenderTargets;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_textBrush;
		Microsoft::WRL::ComPtr<IDWriteTextFormat> m_textFormat;
		std::vector<TextBlock> m_textBlocks;
	};

}
using namespace basecross;
//namespace basecross 