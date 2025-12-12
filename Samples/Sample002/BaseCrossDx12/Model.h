/*!
@file Model.h
@brief モデルクラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	DECLARE_DX12SHADER(BcVSPNTBonePL)
	DECLARE_DX12SHADER(BcPSPNTPL)


	//--------------------------------------------------------------------------------------
	//	モデルクラス
	//--------------------------------------------------------------------------------------
	class Model : public ObjectInterface {
		std::shared_ptr<BaseMesh> m_baseMesh;
		std::vector<Mat4x4>  m_BoneTransforms;
		double m_totalTime;


		BasicConstant m_constantBuffer;
		size_t m_constantBufferIndex;


	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ、デストラクタ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		Model() : m_totalTime(0.0){}
		virtual ~Model() {}
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



		virtual bool UpdateAnimation(double animeTime);

		virtual void OnUpdateConstantBuffers();

		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンスタントバッファのコミット
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void CommitConstantBuffers();

		virtual void ScenePass(ID3D12GraphicsCommandList* pCommandList);






	};


}
// end namespace basecross
