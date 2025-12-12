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
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		//OBB衝突j判定を付ける
		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetFixed(true);
		//タグをつける
		AddTag(L"FixedBox");
		auto ptrShadow = AddComponent<Shadowmap>();
		ptrShadow->AddBaseMesh(L"DEFAULT_CUBE");
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->AddBaseMesh(L"DEFAULT_CUBE");
		ptrDraw->AddBaseTexture(L"SKY_TX");
		ptrDraw->SetOwnShadowActive(true);
	}


	//--------------------------------------------------------------------------------------
	// モデルオブジェクト
	//--------------------------------------------------------------------------------------
	BoneModel::BoneModel(const std::shared_ptr<Stage>& stage, const TransParam& param) :
		GameObject(stage),
		m_totalTime(0.0)
	{
		m_transParam = param;
	}
	BoneModel::~BoneModel() {}


	void BoneModel::OnCreate() {


		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		//		App::GetRelativeAssetsDir(), L"Chara_R\\Chara_R.fbx"

		//		App::GetRelativeAssetsDir(), L"SeaLife_Rigged\\Green_Sea_Turtle_Maya_2018.fbx"

		//		std::shared_ptr<AssimpLoader> ptrAssimpLoader = std::shared_ptr<AssimpLoader>(new AssimpLoader());


		m_baseMesh = BaseMesh::CreateSingleBoneModelMesh(
			pCommandList,
			App::GetRelativeAssetsDir(), L"SeaLife_Rigged\\Green_Sea_Turtle_Maya_2018.fbx"
		);

		//m_baseMesh = BaseMesh::CreateSingleBoneModelMesh(
		//	pCommandList,
		//	App::GetRelativeAssetsDir(), L"MurtiMesh\\FBX 2013\\Object_WalkAnimation.fbx"
		//);


		//App::GetRelativeAssetsDir(), L"MurtiMesh\\FBX 2013\\ObjectOnly.fbx"


		//ObjectOnly
		//Object_WalkAnimation
		auto ptrShadow = AddComponent<Shadowmap>();
		ptrShadow->AddBaseMesh(m_baseMesh);
		auto ptrDraw = AddComponent<BcPNTBoneDraw>();
		ptrDraw->AddBaseMesh(m_baseMesh);

	}

	void BoneModel::OnUpdate(double elapsedTime) {
		m_totalTime += elapsedTime;
		if (m_totalTime >= 2.0) {
			m_totalTime = 0.0;
		}
		auto ptrDraw = GetComponent<BcPNTBoneDraw>();
		ptrDraw->UpdateAnimation(m_totalTime);

	}



}
// end namespace basecross
