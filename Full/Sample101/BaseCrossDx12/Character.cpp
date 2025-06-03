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
		GameObject(stage, param)
	{
	}
	FixedBox::~FixedBox() {}

	void FixedBox::OnCreate() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		//OBB衝突j判定を付ける
		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetFixed(true);
		//タグをつける
		AddTag(L"FixedBox");
		auto ptrShadow = AddComponent<Shadowmap>();
		ptrShadow->AddBaseMesh(L"DEFAULT_CUBE");
		auto ptrDraw = AddComponent<BcStaticDraw>();
		ptrDraw->AddBaseMesh(L"DEFAULT_CUBE");
		ptrDraw->AddBaseTexture(L"SKY_TX");
		ptrDraw->SetOwnShadowActive(true);
	}

	//--------------------------------------------------------------------------------------
	// 四角のオブジェクト
	//--------------------------------------------------------------------------------------
	WallBox::WallBox(const std::shared_ptr<Stage>& stage, const TransParam& param) :
		GameObject(stage, param),
		m_totalTime(0.0)
	{
	}
	WallBox::~WallBox() {}

	void WallBox::OnCreate() {
		//OBB衝突j判定を付ける
		auto ptrColl = AddComponent<CollisionObb>();
	//	ptrColl->SetFixed(true);
		//重力をつける
		auto ptrGra = AddComponent<Gravity>();

		auto ptrShadow = AddComponent<Shadowmap>();
		ptrShadow->AddBaseMesh(L"DEFAULT_CUBE");
		auto ptrDraw = AddComponent<BcStaticDraw>();
		ptrDraw->AddBaseMesh(L"DEFAULT_CUBE");
		ptrDraw->AddBaseTexture(L"WALL_TX");
		ptrDraw->SetOwnShadowActive(true);
	}

	void WallBox::OnUpdate(double elapsedTime) {
		//Transformコンポーネントを取り出す
		auto ptrTrans = GetComponent<Transform>();
		auto& param = ptrTrans->GetTransParam();

		m_totalTime += elapsedTime;
		if (m_totalTime >= XM_2PI) {
			m_totalTime = 0.0;
		}
		Quat spanQt(Vec3(1.0f, 1.0f, 0.0f), (float)(elapsedTime * 4));
		Quat quaternion(param.quaternion);
		quaternion *= spanQt;
//		param.quaternion = quaternion;
		param.position.x = (float)sin(m_totalTime) * 2.0f;
	}


}
// end namespace basecross
