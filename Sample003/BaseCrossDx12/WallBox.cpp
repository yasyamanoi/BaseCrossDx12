/*!
@file WallBox.cpp
@brief 四角のオブジェクト　実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	using namespace std;
	using namespace bsm;

	//--------------------------------------------------------------------------------------
	// 四角のオブジェクト
	//--------------------------------------------------------------------------------------
	WallBox::WallBox(const shared_ptr<Stage>& stage, const TransParam& param) :
		GameObject(stage,param),
		m_totalTime(0.0)
	{
	}
	WallBox::~WallBox() {}

	void WallBox::OnCreate() {
		auto ptrGameStage = dynamic_pointer_cast<GameStage>(GetStage());
		auto ptrPxPhysics = ptrGameStage->GetPxPhysics();
		//Transformコンポーネントを取り出す
		auto ptrTrans = GetComponent<Transform>();
		auto& param = ptrTrans->GetTransParam();
		//PhysX関連
		physx::PxTransform pose = bsmUtil::ToPxTransform(param.position, param.quaternion);
		m_rigid_dynamic
			= ptrPxPhysics->createRigidDynamic(physx::PxTransform(
				pose
			)
		);
		physx::PxBoxGeometry scale(param.scale.x * 0.5f, param.scale.y * 0.5f, param.scale.z * 0.5f);
		auto box_shape
			= ptrPxPhysics->createShape(
				scale,
				*ptrPxPhysics->createMaterial(0.5f, 0.5f, 0.5f)
			);
		box_shape->setLocalPose(physx::PxTransform(physx::PxIdentity));
		m_rigid_dynamic->attachShape(*box_shape);
		ptrGameStage->GetPxScene()->addActor(*m_rigid_dynamic);
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


	void WallBox::OnUpdate(double elapsedTime) {
		//Transformコンポーネントを取り出す
		auto ptrTrans = GetComponent<Transform>();
		auto& param = ptrTrans->GetTransParam();
		physx::PxTransform pose = m_rigid_dynamic->getGlobalPose();
		param.position = bsmUtil::ToVec3(pose.p);
		param.quaternion = bsmUtil::ToQuat(pose.q);
	}

	void WallBox::OnShadowDraw() {
		auto ptrShadow = GetComponent<ShadowmapComp>();
		ptrShadow->OnShadowDraw();
	}
	void WallBox::OnSceneDraw() {
		auto ptrScene = GetComponent<SpSceneComp>();
		ptrScene->OnSceneDraw();
	}

	void WallBox::OnDestroy() {
		if (m_rigid_dynamic) {
			m_rigid_dynamic->release();
		}
	}



}
// end namespace basecross
