/*!
@file Scene.h
@brief シーンクラス
*/
#pragma once
#include "stdafx.h"
#include "Project.h"

namespace basecross {


	DECLARE_DX12SHADER(SpVSPCStatic)
	DECLARE_DX12SHADER(SpPSPCStatic)

	//--------------------------------------------------------------------------------------
	// シーン
	//--------------------------------------------------------------------------------------
	class Scene : public BaseScene
	{
		SimpleConstant m_constantBuffer;
		size_t m_constantBufferIndex;
		std::shared_ptr<BaseMesh> m_mesh;
		double m_totalTime;
		TransParam m_param;
		std::shared_ptr<Camera> m_camera;
		std::shared_ptr<LightSet> m_lightSet;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		@param[in]	frameCount	フレーム数
		@param[in]	pPrimDevice	デバイス
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		Scene(UINT frameCount, PrimDevice* pPrimDevice);
		virtual ~Scene();
	protected:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンテンツの作成
		@param[in]	pDevice	デバイス
		@param[in]	pCommandList	コマンドリスト
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void CreateAssetResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	更新処理
		@param[in]	elapsedTime	前のターンからの時間
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void Update(double elapsedTime)override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンスタントバッファの更新
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void UpdateConstantBuffers()override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンスタントバッファのコミット
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void CommitConstantBuffers()override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	UIの更新
		@param[in]	uiLayer	UIレイヤー
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void UpdateUI(std::unique_ptr<UILayer>& uiLayer)override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	シャドウマップの描画
		@param[in]	コマンドリスト
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void ShadowPass(ID3D12GraphicsCommandList* pCommandList)override {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	シーンの描画
		@param[in]	コマンドリスト
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void ScenePass(ID3D12GraphicsCommandList* pCommandList);
	};
}
//end namespace basecross
