//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

/*!
@file BaseHelper.cpp
@brief ユーティリティクラス、関数群
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {
	//--------------------------------------------------------------------------------------
	///	Dx12ShaderHelper
	//--------------------------------------------------------------------------------------

	namespace Dx12ShaderHelper {
		//--------------------------------------------------------------------------------------
		//　void CreateShaderFlomCso(
		//		const wstring& fileName,	//csoファイル
		//		ID3DBlob** pResult	//Blob
		//	);
		//	用途: CSOデータからシェーダ用のBlobを作成する
		//--------------------------------------------------------------------------------------
		void CreateShaderFlomCso(const std::wstring& fileName, ID3DBlob** pResult) {
			try {
				if (fileName == L"") {
					throw BaseException(
						L"ファイルが指定されていません\n",
						L"if(fileName == L\"\")\n",
						L"Dx12ShaderResource::CreateShaderFlomCso()"
					);
				}
				DWORD RetCode;
				RetCode = GetFileAttributes(fileName.c_str());
				if (RetCode == -1) {
					throw BaseException(
						L"ファイルが存在しません\n",
						fileName,
						L"\nDx12ShaderResource::CreateShaderFlomCso()"
					);
				}
				ThrowIfFailedEx(
					D3DReadFileToBlob(fileName.c_str(), pResult),
					L"Blpbの作成に失敗しました。\n",
					fileName,
					L"\nDx12ShaderResource::CreateShaderFlomCso()"
				);

			}
			catch (...) {
				throw;
			}
		}
	}


	//--------------------------------------------------------------------------------------
	//	class Dx12ShaderResource;
	//	用途: シェーダ関連リソースのインターフェイス
	//--------------------------------------------------------------------------------------
	Dx12ShaderResource::Dx12ShaderResource() {}
	Dx12ShaderResource::~Dx12ShaderResource() {}


	//シェーダアクセッサ
	ID3DBlob* Dx12ShaderResource::GetShaderBlob(const std::wstring& fileName, ComPtr<ID3DBlob>& shaderComPtr) {
		//ミューテックス
		std::mutex Mutex;
		//ラムダ式利用
		return Util::DemandCreate(shaderComPtr, Mutex, [&](ID3DBlob** pResult)
			{
				Dx12ShaderHelper::CreateShaderFlomCso(fileName, pResult);
			});
	}

	ComPtr<ID3DBlob>& Dx12ShaderResource::GetShaderBlobComPtr(const std::wstring& fileName, ComPtr<ID3DBlob>& shaderComPtr) {
		//ミューテックス
		std::mutex mutex;
		//ラムダ式利用
		Util::DemandCreate(shaderComPtr, mutex, [&](ID3DBlob** pResult)
			{
				Dx12ShaderHelper::CreateShaderFlomCso(fileName, pResult);
			});
		return shaderComPtr;
	}

}
// end namespace basecross
