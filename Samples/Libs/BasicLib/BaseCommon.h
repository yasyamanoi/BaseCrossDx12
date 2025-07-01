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
@brief ç\ë¢ëÃêÈåæìô
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


}
// end namespace basecross


