/*!
@file Character.cpp
@brief 配置オブジェクト 実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// ボックスオブジェクト
	//--------------------------------------------------------------------------------------
	FixedBox::FixedBox(const std::shared_ptr<Stage>& stage, const TransParam& param) :
		GameObject(stage)
	{
		m_transParam = param;
	}
	FixedBox::~FixedBox() {}

	void FixedBox::OnCreate() {
		//OBB衝突j判定を付ける
		auto ptrColl = AddComponent<CollisionObb>();
		//この衝突判定は、動かない、という特徴をつける
		ptrColl->SetFixed(true);
		//影（シャドウマップ）をつける
		auto ptrShadow = AddComponent<Shadowmap>();
		//L"DEFAULT_CUBE"というメッシュをつける
		ptrShadow->AddBaseMesh(L"DEFAULT_CUBE");
		//描画コンポーネントに、BcPNTStaticDraw、を指定する
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		//ここにもL"DEFAULT_CUBE"というメッシュをつける
		ptrDraw->AddBaseMesh(L"DEFAULT_CUBE");
		//L"SKY_TX"というテクスチャをつける
		ptrDraw->AddBaseTexture(L"SKY_TX");
		//自分自身に影が移りこむようにする
		ptrDraw->SetOwnShadowActive(true);
	}
}
// end namespace basecross
