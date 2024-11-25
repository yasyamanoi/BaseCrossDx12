/*!
@file Scene.h
@brief シーンクラス。必要であれば、BaseSceneを再定義
*/

#pragma once

#include "stdafx.h"

using namespace bsm;
namespace basecross {

	//--------------------------------------------------------------------------------------
	//	シェーダー
	//--------------------------------------------------------------------------------------
	DECLARE_DX12SHADER(VSPCSprite)
	DECLARE_DX12SHADER(PSPCSprite)
	//--------------------------------------------------------------------------------------
	//	シーン
	//--------------------------------------------------------------------------------------
	class Scene : public BaseScene
	{
		std::shared_ptr<PcSprite> m_pcSprite;
	public:
		Scene(UINT frameCount, PrimDevice* pPrimDevice);
		virtual ~Scene();
		//--------------------------------------------------------------------------------------
		// 更新処理
		//--------------------------------------------------------------------------------------
		virtual void Update(double elapsedTime)override;
		//--------------------------------------------------------------------------------------
		// デバッグ文字列の更新
		// 何もしない
		//--------------------------------------------------------------------------------------
		virtual void UpdateUI(std::wstring& uiText)override{}
	protected:
		//--------------------------------------------------------------------------------------
		// D3Dリソースの作成
		//--------------------------------------------------------------------------------------
		virtual void CreatePipelineStates(ID3D12Device* pDevice)override;
		virtual void CreateSamplers(ID3D12Device* pDevice)override {}
		//--------------------------------------------------------------------------------------
		// オブジェクト作成
		//--------------------------------------------------------------------------------------
		virtual void CreateSceneResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)override;
		//--------------------------------------------------------------------------------------
		// 描画処理（シャドウ）
		// 何もしない
		//--------------------------------------------------------------------------------------
		virtual void ShadowPass(ID3D12GraphicsCommandList* pCommandList)override{}
		//--------------------------------------------------------------------------------------
		// 描画処理（シーン）
		//--------------------------------------------------------------------------------------
		virtual void ScenePass(ID3D12GraphicsCommandList* pCommandList)override;
		//--------------------------------------------------------------------------------------
		//描画処理（ポストプロセス）
		//何もしない
		//--------------------------------------------------------------------------------------
		virtual void PostprocessPass(ID3D12GraphicsCommandList* pCommandList)override{}
	};
}
using namespace basecross;
// end namespace basecross
