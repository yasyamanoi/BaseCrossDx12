/*!
@file Character.h
@brief 配置オブジェクト
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// ボックスオブジェクト
	//--------------------------------------------------------------------------------------
	class FixedBox : public GameObject {
	public:
		FixedBox(const std::shared_ptr<Stage>& stage, const TransParam& param);
		virtual ~FixedBox();
		//構築時
		virtual void OnCreate()override;
		//更新時
		virtual void OnUpdate(double elapsedTime)override {}
	};

	//--------------------------------------------------------------------------------------
	///	タイトルスプライト
	//--------------------------------------------------------------------------------------
	class TitleSprite : public GameObject {
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief コンストラクタ
		@param[in]	StagePtr	ステージ
		*/
		//--------------------------------------------------------------------------------------
		TitleSprite(const std::shared_ptr<Stage>& StagePtr);
		//破棄
		virtual ~TitleSprite();
		//初期化
		virtual void OnCreate() override;
		//更新
		virtual void OnUpdate(double elapsedTime)override {}
	};


}
// end namespace basecross
