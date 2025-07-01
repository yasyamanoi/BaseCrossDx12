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
		m_tempParam = param;
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
		auto ptrDraw = AddComponent<BcStaticDraw>();
		ptrDraw->AddBaseMesh(L"DEFAULT_CUBE");
		ptrDraw->AddBaseTexture(L"SKY_TX");
		ptrDraw->SetOwnShadowActive(true);
	}

	//--------------------------------------------------------------------------------------
	// ���f���I�u�W�F�N�g
	//--------------------------------------------------------------------------------------
	FixedModel::FixedModel(const std::shared_ptr<Stage>& stage, const TransParam& param) :
		GameObject(stage)
	{
		m_tempParam = param;
	}
	FixedModel::~FixedModel() {}


	void FixedModel::OnCreate() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		m_baseMesh = BaseMesh::CreateBoneModelMesh(
			pCommandList,
			App::GetRelativeAssetsDir(), L"SeaLife_Rigged\\Green_Sea_Turtle_Maya_2018.fbx"
		);
		auto ptrShadow = AddComponent<Shadowmap>();
		ptrShadow->AddBaseMesh(m_baseMesh);
		auto ptrDraw = AddComponent<BcBoneDraw>();
		ptrDraw->AddBaseMesh(m_baseMesh);



	}


}
// end namespace basecross
