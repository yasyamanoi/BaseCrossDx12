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
@brief ���[�e�B���e�B�N���X�A�֐��Q
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
		//�@void CreateShaderFlomCso(
		//		const wstring& fileName,	//cso�t�@�C��
		//		ID3DBlob** pResult	//Blob
		//	);
		//	�p�r: CSO�f�[�^����V�F�[�_�p��Blob���쐬����
		//--------------------------------------------------------------------------------------
		void CreateShaderFlomCso(const std::wstring& fileName, ID3DBlob** pResult) {
			try {
				if (fileName == L"") {
					throw BaseException(
						L"�t�@�C�����w�肳��Ă��܂���\n",
						L"if(fileName == L\"\")\n",
						L"Dx12ShaderResource::CreateShaderFlomCso()"
					);
				}
				DWORD RetCode;
				RetCode = GetFileAttributes(fileName.c_str());
				if (RetCode == -1) {
					throw BaseException(
						L"�t�@�C�������݂��܂���\n",
						fileName,
						L"\nDx12ShaderResource::CreateShaderFlomCso()"
					);
				}
				ThrowIfFailedEx(
					D3DReadFileToBlob(fileName.c_str(), pResult),
					L"Blpb�̍쐬�Ɏ��s���܂����B\n",
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
	//	�p�r: �V�F�[�_�֘A���\�[�X�̃C���^�[�t�F�C�X
	//--------------------------------------------------------------------------------------
	Dx12ShaderResource::Dx12ShaderResource() {}
	Dx12ShaderResource::~Dx12ShaderResource() {}


	//�V�F�[�_�A�N�Z�b�T
	ID3DBlob* Dx12ShaderResource::GetShaderBlob(const std::wstring& fileName, ComPtr<ID3DBlob>& shaderComPtr) {
		//�~���[�e�b�N�X
		std::mutex Mutex;
		//�����_�����p
		return Util::DemandCreate(shaderComPtr, Mutex, [&](ID3DBlob** pResult)
			{
				Dx12ShaderHelper::CreateShaderFlomCso(fileName, pResult);
			});
	}

	ComPtr<ID3DBlob>& Dx12ShaderResource::GetShaderBlobComPtr(const std::wstring& fileName, ComPtr<ID3DBlob>& shaderComPtr) {
		//�~���[�e�b�N�X
		std::mutex mutex;
		//�����_�����p
		Util::DemandCreate(shaderComPtr, mutex, [&](ID3DBlob** pResult)
			{
				Dx12ShaderHelper::CreateShaderFlomCso(fileName, pResult);
			});
		return shaderComPtr;
	}

}
// end namespace basecross
