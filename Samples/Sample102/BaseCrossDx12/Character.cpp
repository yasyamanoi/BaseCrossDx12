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

		//Transform�R���|�[�l���g�����o��
		auto ptrTrans = GetComponent<Transform>();
		auto& param = ptrTrans->GetTransParam();
		//PhysX�֘A
		PhysxCreateParam pxParam;
		physx::PxBoxGeometry scale(param.scale.x * 0.5f, param.scale.y * 0.5f, param.scale.z * 0.5f);
		pxParam.pGeometry = &scale;
		AddComponent<RigidbodyStatic>(pxParam);

		auto ptrShadow = AddComponent<Shadowmap>();
		ptrShadow->AddBaseMesh(L"DEFAULT_CUBE");
		auto ptrDraw = AddComponent<BcStaticDraw>();
		ptrDraw->AddBaseMesh(L"DEFAULT_CUBE");
		ptrDraw->AddBaseTexture(L"SKY_TX");
		ptrDraw->SetOwnShadowActive(true);
	}

	//--------------------------------------------------------------------------------------
	// �l�p�̃I�u�W�F�N�g
	//--------------------------------------------------------------------------------------
	WallBox::WallBox(const std::shared_ptr<Stage>& stage, const TransParam& param) :
		GameObject(stage),
		m_totalTime(0.0)
	{
		m_tempParam = param;
	}
	WallBox::~WallBox() {}

	void WallBox::OnCreate() {
		auto ptrGameStage = std::dynamic_pointer_cast<GameStage>(GetStage());
		//Transform�R���|�[�l���g�����o��
		auto ptrTrans = GetComponent<Transform>();
		auto& param = ptrTrans->GetTransParam();
		//PhysX�֘A
		PhysxCreateParam pxParam;
		physx::PxBoxGeometry scale(param.scale.x * 0.5f, param.scale.y * 0.5f, param.scale.z * 0.5f);
		pxParam.pGeometry = &scale;
		auto pRigDynamicComp = AddComponent<RigidbodyDynamic>(pxParam);
		auto pRigDynamic = pRigDynamicComp->GetRigidDynamic();

		auto ptrShadow = AddComponent<Shadowmap>();
		ptrShadow->AddBaseMesh(L"DEFAULT_CUBE");
		auto ptrDraw = AddComponent<BcStaticDraw>();
		ptrDraw->AddBaseMesh(L"DEFAULT_CUBE");
		ptrDraw->AddBaseTexture(L"WALL_TX");
		ptrDraw->SetOwnShadowActive(true);

		//�W�����v������
		pRigDynamic->addForce(physx::PxVec3(0, 10, 0), physx::PxForceMode::eIMPULSE);

	}

	void WallBox::OnUpdate(double elapsedTime) {
		//RigidbodyDynamic�R���|�[�l���g�����o��
//		auto ptrRigid = GetComponent<RigidbodyDynamic>();
//		ptrRigid->OnUpdate(elapsedTime);
	}



}
// end namespace basecross
