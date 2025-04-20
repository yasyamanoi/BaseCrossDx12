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
@file BaseCommon.h
@brief 構造体宣言等
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once

#include "stdafx.h"
namespace basecross {

	static const UINT NumNullSrvs = 2; // Null descriptors at the start of the heap.

	// Currently the rendering code can only handle a single point light.
	static const UINT NumLights = 1; // Keep this in sync with "ShadowsAndScenePass.hlsl".

	// Command list submissions from main thread.
	static const int CommandListCount = 3;
	static const int CommandListPre = 0;
	static const int CommandListMid = 1;
	static const int CommandListPost = 2;

	namespace SceneEnums
	{
		namespace RenderPass {
			enum Value { Scene = 0, Shadow, Postprocess, Count };
		}

		namespace DepthGenPass {
			enum Value { Scene = 0, Shadow, Count };
		}

		namespace RootSignature {
			enum { ShadowPass = 0, ScenePass, PostprocessPass, Count };
		};

		namespace VertexBuffer {
			enum Value { SceneGeometry = 0, ScreenQuad, Count };
		}

		namespace Timestamp {
			enum Value { ScenePass = 0, PostprocessPass, Count };
		}
	}

	struct PostprocessConstantBuffer
	{
		XMFLOAT4 lightPosition;
		XMFLOAT4 cameraPosition;
		XMFLOAT4X4 viewInverse;
		XMFLOAT4X4 projInverse;
		XMFLOAT4X4 viewProjInverseAtNearZ1;
		float fogDensity;
	};


	//--------------------------------------------------------------------------------------
	///	Shadowコンスタントバッファ構造体
	//--------------------------------------------------------------------------------------
	struct ShadowConstant
	{
		/// ワールド行列
		XMFLOAT4X4 world;
		/// ビュー行列
		XMFLOAT4X4 view;
		/// 射影行列
		XMFLOAT4X4 projection;
		/// bone用
		XMFLOAT4 bones[3 * 72];
		ShadowConstant() {
			memset(this, 0, sizeof(ShadowConstant));
		};
	};

	//--------------------------------------------------------------------------------------
	///	Baseシェーダー用コンスタントバッファ
	//--------------------------------------------------------------------------------------
	struct BaseConstantBuffer
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT4 emissiveColor;
		XMFLOAT4 specularColorAndPower;

		XMFLOAT4 lightDirection[3];
		XMFLOAT4 lightDiffuseColor[3];
		XMFLOAT4 lightSpecularColor[3];

		XMFLOAT4 eyePosition;

		XMFLOAT4 fogColor;
		XMFLOAT4 fogVector;

		XMFLOAT4X4 world;
		XMFLOAT4 worldInverseTranspose[3];
		XMFLOAT4X4 worldViewProj;
		//汎用フラグ
		XMUINT4 activeFlg;
		//以下影
		XMFLOAT4 lightPos;
		XMFLOAT4 eyePos;
		XMFLOAT4X4 lightView;
		XMFLOAT4X4 lightProjection;

		XMFLOAT4 bones[3 * 72];
	};


}
// end namespace basecross


