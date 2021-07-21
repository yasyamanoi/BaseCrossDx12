/*!
@file BaseTexture.h
@brief テクスチャクラス
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	テクスチャクラス
	//--------------------------------------------------------------------------------------
	class BaseTexture {
		ComPtr<ID3D12Resource> m_texture;
		ComPtr<ID3D12Resource> m_textureUploadHeap;
		DirectX::ScratchImage m_image;
		std::vector<D3D12_SUBRESOURCE_DATA> m_subresources;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_maphandle;
		UINT m_srvIndex;
		BaseTexture() {}
	public:
		~BaseTexture() {}
		ComPtr<ID3D12Resource> GetTexture() const {
			return m_texture;
		}
		ComPtr<ID3D12Resource> GetTextureUploadHeap() const {
			return m_textureUploadHeap;
		}
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetMapHandle() const {
			return m_maphandle;
		}
		void SetSrvIndex(UINT srvIndex) {
			m_srvIndex = srvIndex;
		}
		UINT GetSrvIndex() const {
			return m_srvIndex;
		}
		//サブリソースのアップデートとシェーダリソースビューの作成
		void UpdateSRAndCreateSRV(const ComPtr<ID3D12GraphicsCommandList>& commandList);
		//ファイルからテスクチャ作成(SRVも作成する)
		static shared_ptr<BaseTexture>
			CreateBaseTextureFromFile(const ComPtr<ID3D12GraphicsCommandList>& commandList,const wstring& fileName, const CD3DX12_CPU_DESCRIPTOR_HANDLE& mapHandle);

	};



}
//end basecross
