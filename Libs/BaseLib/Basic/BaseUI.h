/*!
@file BaseUI.h
@brief 文字列表示クラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	文字列表示
	//--------------------------------------------------------------------------------------
	class BaseUI
	{
	public:
		BaseUI(UINT frameCount, ID3D12Device* pDevice, ID3D12CommandQueue* pCommandQueue);
		//文字列の更新
		void UpdateLabels(const std::wstring& uiText);
		//文字列の表示
		void Render(UINT frameIndex);
		//リソースの解放
		void ReleaseResources();
		//表示領域サイズの変更
		void Resize(Microsoft::WRL::ComPtr<ID3D12Resource>* ppRenderTargets, UINT width, UINT height);

	private:
		//フレーム数を得る
		UINT FrameCount() { return static_cast<UINT>(m_wrappedRenderTargets.size()); }
		//初期化
		void Initialize(ID3D12Device* pDevice, ID3D12CommandQueue* pCommandQueue);

		// 描画ターゲット情報
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
