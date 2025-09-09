/*!
@file Character.cpp
@brief �z�u�I�u�W�F�N�g ����
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// �{�b�N�X�I�u�W�F�N�g
	//--------------------------------------------------------------------------------------
	FixedBox::FixedBox(const std::shared_ptr<Stage>& stage, const TransParam& param) :
		GameObject(stage)
	{
		m_transParam = param;
	}
	FixedBox::~FixedBox() {}

	void FixedBox::OnCreate() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		//OBB�Փ�j�����t����
		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetFixed(true);
		//�^�O������
		AddTag(L"FixedBox");
		auto ptrShadow = AddComponent<Shadowmap>();
		ptrShadow->AddBaseMesh(L"DEFAULT_CUBE");
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->AddBaseMesh(L"DEFAULT_CUBE");
		ptrDraw->AddBaseTexture(L"SKY_TX");
		ptrDraw->SetOwnShadowActive(true);
	}

	//--------------------------------------------------------------------------------------
	// ���f���I�u�W�F�N�g
	//--------------------------------------------------------------------------------------
	FixedModel::FixedModel(const std::shared_ptr<Stage>& stage, const TransParam& param) :
		GameObject(stage),
		m_totalTime(0.0)
	{
		m_transParam = param;
	}
	FixedModel::~FixedModel() {}


	void FixedModel::OnCreate() {


		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
//		App::GetRelativeAssetsDir(), L"Chara_R\\Chara_R.fbx"

//		App::GetRelativeAssetsDir(), L"SeaLife_Rigged\\Green_Sea_Turtle_Maya_2018.fbx"

//		std::shared_ptr<AssimpLoader> ptrAssimpLoader = std::shared_ptr<AssimpLoader>(new AssimpLoader());


		m_baseMesh = BaseMesh::CreateBoneModelMesh(
			pCommandList,
			App::GetRelativeAssetsDir(), L"SeaLife_Rigged\\Green_Sea_Turtle_Maya_2018.fbx"
		);
		auto ptrShadow = AddComponent<Shadowmap>();
		ptrShadow->AddBaseMesh(m_baseMesh);
		auto ptrDraw = AddComponent<BcBoneDraw>();
		ptrDraw->AddBaseMesh(m_baseMesh);

	}

	void FixedModel::OnUpdate(double elapsedTime) {
		m_totalTime += elapsedTime;
		if (m_totalTime >= 6.0) {
			m_totalTime = 0.0;
		}
		auto ptrDraw = GetComponent<BcBoneDraw>();
		ptrDraw->UpdateAnimation(m_totalTime);

	}



}
// end namespace basecross
