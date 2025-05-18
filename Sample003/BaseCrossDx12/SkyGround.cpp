/*!
@file SkyGround.h
@brief 土台クラス　実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	using namespace std;
	using namespace bsm;

	//--------------------------------------------------------------------------------------
	// 土台のオブジェクト
	//--------------------------------------------------------------------------------------
	SkyGround::SkyGround(const shared_ptr<Stage>& stage, const TransParam& param) :
		GameObject(stage,param)
	{
	}
	SkyGround::~SkyGround() {}

	void SkyGround::OnCreate() {
		auto ptrGameStage = dynamic_pointer_cast<GameStage>(GetStage());
		auto ptrPxPhysics = ptrGameStage->GetPxPhysics();
		//Transformコンポーネントを取り出す
		auto ptrTrans = GetComponent<Transform>();
		auto& param = ptrTrans->GetTransParam();
		//PhysX関連
		auto pose = bsmUtil::ToPxTransform(param.position,Quat());
		physx::PxRigidStatic* rigid_static
			= ptrPxPhysics->createRigidStatic(
				pose
			);
		physx::PxBoxGeometry scale(param.scale.x * 0.5f, param.scale.y * 0.5f, param.scale.z * 0.5f);
		physx::PxShape* box_shape
			= ptrPxPhysics->createShape(
				scale,
				*ptrPxPhysics->createMaterial(0.5f, 0.5f, 0.5f)
			);
		box_shape->setLocalPose(physx::PxTransform(physx::PxIdentity));
		rigid_static->attachShape(*box_shape);
		ptrGameStage->GetPxScene()->addActor(*rigid_static);
		//BaseCross関連
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		m_mesh = BaseMesh::CreateCube(pCommandList, 1.0f);
		auto ptrShadow = AddComponent<ShadowmapComp>();
		ptrShadow->SetBaseMesh(m_mesh);
		auto texFile = App::GetRelativeAssetsDir() + L"sky.jpg";
		m_texture = BaseTexture::CreateTextureFlomFile(pCommandList, texFile);
		auto ptrScene = AddComponent<SpSceneComp>();
		ptrScene->SetBaseMesh(m_mesh);
		ptrScene->SetBaseTexture(m_texture);

	}

	void SkyGround::OnDestroy() {
	}


	void SkyGround::OnUpdateConstantBuffers() {
		auto ptrShadow = GetComponent<ShadowmapComp>();
		ptrShadow->OnUpdateConstantBuffers();
		auto ptrScene = GetComponent<SpSceneComp>();
		ptrScene->OnUpdateConstantBuffers();
	}

	void SkyGround::OnCommitConstantBuffers() {
		auto ptrShadow = GetComponent<ShadowmapComp>();
		ptrShadow->OnCommitConstantBuffers();
		auto ptrScene = GetComponent<SpSceneComp>();
		ptrScene->OnCommitConstantBuffers();
	}

	void SkyGround::OnShadowDraw() {
		auto ptrShadow = GetComponent<ShadowmapComp>();
		ptrShadow->OnShadowDraw();
	}
	void SkyGround::OnSceneDraw() {
		auto ptrScene = GetComponent<SpSceneComp>();
		ptrScene->OnSceneDraw();
	}

}
// end namespace basecross
