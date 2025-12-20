/*!
@file Triangle.h
@brief 三角形クラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	DECLARE_DX12SHADER(SpVSPCStatic)
	DECLARE_DX12SHADER(SpPSPCStatic)

	//--------------------------------------------------------------------------------------
	//	三角形クラス
	//--------------------------------------------------------------------------------------
	class Triangle : public ObjectInterface {
		SimpleConstant m_constantBuffer;
		size_t m_constantBufferIndex;
		std::shared_ptr<BaseMesh> m_mesh;
		double m_totalTime;
		TransParam m_param;
		void CreateConstantBuffer();
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ、デストラクタ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		Triangle() : m_totalTime(0.0) {}
		virtual ~Triangle() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	作成処理
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCreate()override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief 更新処理
		@param[in]	elapsedTime	ターン時間
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate(double elapsedTime)override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンスタントバッファの更新
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdateConstantBuffers();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンスタントバッファのコミット
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCommitConstantBuffers();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	シーンの描画
		@param[in]	pCommandList コマンドリスト
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnSceneDraw(ID3D12GraphicsCommandList* pCommandList)override;



	};



}
//namespace basecross
