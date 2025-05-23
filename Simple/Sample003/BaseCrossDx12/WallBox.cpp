/*!
@file WallBox.cpp
@brief 四角のオブジェクト　実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// 四角のオブジェクト
	//--------------------------------------------------------------------------------------
	WallBox::WallBox(const std::shared_ptr<Stage>& stage, const TransParam& param) :
		GameObject(stage,param),
		m_totalTime(0.0)
	{
	}
	WallBox::~WallBox() {}

	void WallBox::OnCreate() {
		auto ptrGameStage = std::dynamic_pointer_cast<GameStage>(GetStage());
		//Transformコンポーネントを取り出す
		auto ptrTrans = GetComponent<Transform>();
		auto& param = ptrTrans->GetTransParam();
		//PhysX関連
		PhysxCreateParam pxParam;
		physx::PxBoxGeometry scale(param.scale.x * 0.5f, param.scale.y * 0.5f, param.scale.z * 0.5f);
		pxParam.pGeometry = &scale;
		auto pRigDynamicComp = AddComponent<RigidDynamicComp>(pxParam);
		auto pRigDynamic = pRigDynamicComp->GetRigidDynamic();
		//ジャンプさせる
		pRigDynamic->addForce(physx::PxVec3(0, 10, 0), physx::PxForceMode::eIMPULSE);


		//BaseCross関連
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		m_mesh = BaseMesh::CreateCube(pCommandList, 1.0f);
		auto ptrShadow = AddComponent<ShadowmapComp>();
		ptrShadow->SetBaseMesh(m_mesh);
		auto texFile = App::GetRelativeAssetsDir() + L"wall.jpg";
		m_texture = BaseTexture::CreateTextureFlomFile(pCommandList, texFile);
		auto ptrScene = AddComponent<SpSceneComp>();
		ptrScene->SetBaseMesh(m_mesh);
		ptrScene->SetBaseTexture(m_texture);
	}

	void WallBox::OnUpdate(double elapsedTime) {
		//RigidDynamicCompコンポーネントを取り出す
		auto ptrRigid = GetComponent<RigidDynamicComp>();
		ptrRigid->OnUpdate(elapsedTime);
	}

	void WallBox::OnUpdateConstantBuffers() {
		auto ptrShadow = GetComponent<ShadowmapComp>();
		ptrShadow->OnUpdateConstantBuffers();
		auto ptrScene = GetComponent<SpSceneComp>();
		ptrScene->OnUpdateConstantBuffers();
	}

	void WallBox::OnCommitConstantBuffers() {
		auto ptrShadow = GetComponent<ShadowmapComp>();
		ptrShadow->OnCommitConstantBuffers();
		auto ptrScene = GetComponent<SpSceneComp>();
		ptrScene->OnCommitConstantBuffers();
	}

	void WallBox::OnDestroy() {
		//RigidDynamicCompコンポーネントを取り出す
		auto ptrRigid = GetComponent<RigidDynamicComp>();
		ptrRigid->OnDestroy();
	}

	void WallBox::OnShadowDraw() {
		auto ptrShadow = GetComponent<ShadowmapComp>();
		ptrShadow->OnShadowDraw();
	}
	void WallBox::OnSceneDraw() {
		auto ptrScene = GetComponent<SpSceneComp>();
		ptrScene->OnSceneDraw();
	}



}
// end namespace basecross
