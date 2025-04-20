/*!
@file Scene.h
@brief シーンクラス
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	using namespace std;
	using namespace SceneEnums;

	//--------------------------------------------------------------------------------------
	// シーン
	//--------------------------------------------------------------------------------------
	class Scene : public BaseScene
	{
	public:
		Scene(UINT frameCount, PrimDevice* pSample);
		virtual ~Scene();
	protected:

		virtual void CreateAssetResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)override;
		virtual void UpdateConstantBuffers()override;
		virtual void CommitConstantBuffers()override;

		virtual void Update(double elapsedTime)override;

		virtual void ShadowPass(ID3D12GraphicsCommandList* pCommandList)override;
		virtual void ScenePass(ID3D12GraphicsCommandList* pCommandList)override;
	};

}
//end namespace basecross

