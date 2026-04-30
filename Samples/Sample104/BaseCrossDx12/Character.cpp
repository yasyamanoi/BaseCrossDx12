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




	//--------------------------------------------------------------------------------------
	///	タイトルスプライト
	//--------------------------------------------------------------------------------------
	TitleSprite::TitleSprite(const std::shared_ptr<Stage>& StagePtr) :
		GameObject(StagePtr)
	{}

	TitleSprite::~TitleSprite() {}
	void TitleSprite::OnCreate() {
		float HelfSize = 0.5f;
		//頂点配列(縦横5個ずつ表示)
		std::vector<VertexPositionColorTexture> vertices = {
			{ VertexPositionColorTexture(Vec3(-HelfSize, HelfSize, 0),Col4(1.0f,1.0f,1.0f,1.0f), Vec2(0.0f, 0.0f)) },
			{ VertexPositionColorTexture(Vec3(HelfSize, HelfSize, 0), Col4(1.0f, 1.0f, 1.0f, 1.0f), Vec2(1.0f, 0.0f)) },
			{ VertexPositionColorTexture(Vec3(-HelfSize, -HelfSize, 0), Col4(1.0f, 1.0f, 1.0f, 1.0f), Vec2(0.0f, 1.0f)) },
			{ VertexPositionColorTexture(Vec3(HelfSize, -HelfSize, 0), Col4(1.0f, 1.0f, 1.0f, 1.0f), Vec2(1.0f, 1.0f)) },
		};
		//インデックス配列
		std::vector<uint32_t> indices = { 0, 1, 2, 1, 3, 2 };
		auto ptrTransform = GetComponent<Transform>();
		ptrTransform->SetScale(512.0f,112.0f, 1.0f);
		ptrTransform->SetRotation(0, 0, 0);
		ptrTransform->SetPosition(0.0f, 0.0f, 0.0f);
		SetAlphaActive(true);
		//頂点とインデックスを指定してスプライト作成
		auto PtrDraw = AddComponent<SpPCTSpriteDraw>(vertices, indices);
	}


}
// end namespace basecross
