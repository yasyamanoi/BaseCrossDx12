/*!
@file FrameResource.h
@brief フレームリソースクラス
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	Basicシェーダー用コンスタントバッファ
	//--------------------------------------------------------------------------------------
	struct BasicConstants
	{
		Vec4 diffuseColor;
		Vec4 emissiveColor;
		Vec4 specularColorAndPower;

		Vec4 lightDirection[3];
		Vec4 lightDiffuseColor[3];
		Vec4 lightSpecularColor[3];

		Vec4 eyePosition;

		Vec4 fogColor;
		Vec4 fogVector;

		Mat4x4 world;
		Vec4 worldInverseTranspose[3];
		Mat4x4 worldViewProj;
		//汎用フラグ
		XMUINT4 activeFlg;
		//以下影
		Vec4 lightPos;
		Vec4 eyePos;
		Mat4x4 lightView;
		Mat4x4 lightProjection;

		Vec4 bones[3 * 72];
	};

	struct FrameParam {
		ComPtr<ID3D12Resource> m_cbvUploadHeap;
		BasicConstants* m_pConstantBuffer;
		UINT m_constBuffIndex;
	};

	//--------------------------------------------------------------------------------------
	///	フレームリソースクラス(構造体)
	//--------------------------------------------------------------------------------------
	struct FrameResource {
		vector<FrameParam> m_frameParamVec;
		ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		UINT64 m_fenceValue;
		FrameResource();
		~FrameResource();
	};

}
// end basecross
