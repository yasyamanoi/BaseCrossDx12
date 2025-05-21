/*!
@file BaseTexture.h
@brief テクスチャクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	テクスチャ
	//--------------------------------------------------------------------------------------
	class BaseTexture {
		ComPtr<ID3D12Resource> m_texture; //テクスチャリソース
		ComPtr<ID3D12Resource> m_textureUploadHeap; //テクスチャのアップロードヒープ
		DirectX::ScratchImage m_image; //イメージ
		std::vector<D3D12_SUBRESOURCE_DATA> m_subresources; //サブリソース
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_maphandle; //マップハンドル
		UINT m_srvIndex; //シェーダーリソースビューのインデックス
		BaseTexture() {}
		//ファイルからテスクチャ作成(SRVも作成する)
		static std::shared_ptr<BaseTexture>
			CreateBaseTextureFromFilePrim(ID3D12GraphicsCommandList* pCommandList, const std::wstring& fileName, const CD3DX12_CPU_DESCRIPTOR_HANDLE& mapHandle);
	public:
		~BaseTexture() {}
		//テクスチャの取得
		ComPtr<ID3D12Resource> GetTexture() const {
			return m_texture;
		}
		//テクスチャのアップロードヒープの取得
		ComPtr<ID3D12Resource> GetTextureUploadHeap() const {
			return m_textureUploadHeap;
		}
		//マップハンドルの取得
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetMapHandle() const {
			return m_maphandle;
		}
		//シェーダーリソースビューインデックスの設定
		void SetSrvIndex(UINT srvIndex) {
			m_srvIndex = srvIndex;
		}
		//シェーダーリソースビューインデックスの取得
		UINT GetSrvIndex() const {
			return m_srvIndex;
		}
		//サブリソースのアップデートとシェーダリソースビューの作成
		void UpdateSRAndCreateSRV(ID3D12GraphicsCommandList* pCommandList);
		//ファイルからテスクチャ作成(SRVも作成する)
		static std::shared_ptr<BaseTexture> CreateTextureFlomFile(ID3D12GraphicsCommandList* pCommandList, const std::wstring& falsename);
	};
}
//end basecross
