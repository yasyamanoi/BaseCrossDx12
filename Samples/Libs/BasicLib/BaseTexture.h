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
		//読み込み済みのイメージ（m_image）からテクスチャリソースとSRVを作成する（ファイル版・メモリ版・ピクセル版で共通）
		static std::shared_ptr<BaseTexture>
			CreateFromLoadedImagePrim(ID3D12GraphicsCommandList* pCommandList, const std::shared_ptr<BaseTexture>& ptrTexture,
				const DirectX::TexMetadata& metadata, const std::wstring& name, const CD3DX12_CPU_DESCRIPTOR_HANDLE& mapHandle);
		//SRVのハンドルを新しく払い出す
		static CD3DX12_CPU_DESCRIPTOR_HANDLE NewSrvHandle(UINT& srvIndex);
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
		//--------------------------------------------------------------------------------------
		/*!
		@brief	メモリ上の画像データ（PNG、JPG、DDS、TGA、HDRなどのファイルの中身）からテクスチャを作成する(SRVも作成する)。
		glbに埋め込まれた画像など、ファイルになっていないデータに使う
		@param[in]	pCommandList	コマンドリスト
		@param[in]	pData	画像データ
		@param[in]	dataSize	画像データのバイト数
		@param[in]	formatHint	画像の種類（"png"、"jpg"、"dds"、"tga"、"hdr"など。ddsとtgaとhdr以外はWICで読む）
		@param[in]	name	エラー表示用の名前
		@return	テクスチャ
		*/
		//--------------------------------------------------------------------------------------
		static std::shared_ptr<BaseTexture> CreateTextureFromMemory(ID3D12GraphicsCommandList* pCommandList,
			const void* pData, size_t dataSize, const std::wstring& formatHint, const std::wstring& name);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ピクセル配列からテクスチャを作成する(SRVも作成する)
		@param[in]	pCommandList	コマンドリスト
		@param[in]	width	幅
		@param[in]	height	高さ
		@param[in]	format	ピクセルの形式（B8G8R8A8_UNORMなど）
		@param[in]	pPixels	ピクセル配列
		@param[in]	rowPitch	1行のバイト数
		@return	テクスチャ
		*/
		//--------------------------------------------------------------------------------------
		static std::shared_ptr<BaseTexture> CreateTextureFromPixels(ID3D12GraphicsCommandList* pCommandList,
			UINT width, UINT height, DXGI_FORMAT format, const void* pPixels, size_t rowPitch);
	};
}
//end basecross
