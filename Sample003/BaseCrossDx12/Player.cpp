/*!
@file Player.cpp
@brief プレイヤー　実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	using namespace std;
	using namespace basecross::bsm;


	//--------------------------------------------------------------------------------------
	// プレイヤー
	//--------------------------------------------------------------------------------------
	Player::Player(const shared_ptr<Stage>& stage, const TransParam& param) :
		GameObject(stage, param),
		m_Speed(15.0f)
	{
	}
	Player::~Player() {}

	void Player::OnCreate() {
		auto ptrGameStage = dynamic_pointer_cast<GameStage>(GetStage());
		//Transformコンポーネントを取り出す
		auto ptrTrans = GetComponent<Transform>();
		auto& param = ptrTrans->GetTransParam();
		//PhysX関連
		PhysxCreateParam pxParam;
		physx::PxSphereGeometry scale(param.scale.x * 0.5f);
		pxParam.pGeometry = &scale;
		pxParam.staticFriction = 1.0f;
		pxParam.dynamicFriction = 1.0f;
		pxParam.restitution = 1.0f;
		auto pRigDynamicComp = AddComponent<RigidDynamicComp>(pxParam);
		auto pRigDynamic = pRigDynamicComp->GetRigidDynamic();
		//BaseCross関連
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		m_mesh = BaseMesh::CreateSphere(pCommandList, 1.0f,18);
		auto ptrShadow = AddComponent<ShadowmapComp>();
		ptrShadow->SetBaseMesh(m_mesh);
		auto texFile = App::GetRelativeAssetsDir() + L"trace.png";
		m_texture = BaseTexture::CreateTextureFlomFile(pCommandList, texFile);
		auto ptrScene = AddComponent<SpSceneComp>();
		ptrScene->SetBaseMesh(m_mesh);
		ptrScene->SetBaseTexture(m_texture);
		//カメラ関連
		//カメラを得る
		auto ptrCamera = ptrGameStage->GetMyCamera();
		if (ptrCamera) {
			//MyCameraである
			//MyCameraに注目するオブジェクト（プレイヤー）の設定
			ptrCamera->SetTargetObject(GetThis<GameObject>());
			ptrCamera->SetTargetToAt(Vec3(0, 0.25f, 0));
		}
	}

	Vec2 Player::GetInputState() const {
		Vec2 ret;
		//コントローラの取得
		//入力の取得
		InputDevice inputDevice;
		inputDevice.ResetControlerState();
		auto cntlVec = inputDevice.GetControlerVec();
		ret.x = 0.0f;
		ret.y = 0.0f;
		WORD wButtons = 0;
		if (cntlVec[0].bConnected) {
			ret.x = cntlVec[0].fThumbLX;
			ret.y = cntlVec[0].fThumbLY;
		}
		return ret;
	}


	Vec3 Player::GetMoveVector() const {
		auto ptrGameStage = dynamic_pointer_cast<GameStage>(GetStage());
		Vec3 angle(0, 0, 0);
		//入力の取得
		auto inPut = GetInputState();
		float moveX = inPut.x;
		float moveZ = inPut.y;
		if (moveX != 0 || moveZ != 0) {
			float moveLength = 0;	//動いた時のスピード
			auto ptrTransform = GetComponent<Transform>();
			auto ptrCamera = ptrGameStage->GetMyCamera();
			//進行方向の向きを計算
			auto front = ptrTransform->GetPosition() - ptrCamera->GetEye();
			front.y = 0;
			front.normalize();
			//進行方向向きからの角度を算出
			float frontAngle = atan2(front.z, front.x);
			//コントローラの向き計算
			Vec2 moveVec(moveX, moveZ);
			float moveSize = moveVec.length();
			//コントローラの向きから角度を計算
			float cntlAngle = atan2(-moveX, moveZ);
			//トータルの角度を算出
			float totalAngle = frontAngle + cntlAngle;
			//角度からベクトルを作成
			angle = Vec3(cos(totalAngle), 0, sin(totalAngle));
			//正規化する
			angle.normalize();
			//移動サイズを設定。
			angle *= moveSize;
			//Y軸は変化させない
			angle.y = 0;
		}
		return angle;
	}


	void Player::MovePlayer(double elapsedTime) {
		auto angle = GetMoveVector();
		//RigidDynamicCompコンポーネントを取り出す
		auto ptrRigid = GetComponent<RigidDynamicComp>();
		if (angle.length() > 0.0f) {
			auto pRigDynamic = ptrRigid->GetRigidDynamic();
			Vec3 tmp = angle * (float)elapsedTime * m_Speed;
			pRigDynamic->addForce(bsmUtil::ToPxVec3(tmp), physx::PxForceMode::eIMPULSE);
			//auto pos = GetComponent<Transform>()->GetPosition();
			//pos += angle * (float)elapsedTime * m_Speed;
			//GetComponent<Transform>()->SetPosition(pos);
		}
		ptrRigid->OnUpdate(elapsedTime);
		//回転の計算
		//if (angle.length() > 0.0f) {
		//	auto utilPtr = GetBehavior<UtilBehavior>();
		//	utilPtr->RotToHead(angle, 1.0f);
		//}
	}



	void Player::OnUpdate(double elapsedTime) {
		////RigidDynamicCompコンポーネントを取り出す
		//auto ptrRigid = GetComponent<RigidDynamicComp>();
		//ptrRigid->OnUpdate(elapsedTime);
		MovePlayer(elapsedTime);

	}

	void Player::OnUpdateConstantBuffers() {
		auto ptrShadow = GetComponent<ShadowmapComp>();
		ptrShadow->OnUpdateConstantBuffers();
		auto ptrScene = GetComponent<SpSceneComp>();
		ptrScene->OnUpdateConstantBuffers();
	}

	void Player::OnCommitConstantBuffers() {
		auto ptrShadow = GetComponent<ShadowmapComp>();
		ptrShadow->OnCommitConstantBuffers();
		auto ptrScene = GetComponent<SpSceneComp>();
		ptrScene->OnCommitConstantBuffers();
	}

	void Player::OnDestroy() {
		//RigidDynamicCompコンポーネントを取り出す
		auto ptrRigid = GetComponent<RigidDynamicComp>();
		ptrRigid->OnDestroy();
	}

	void Player::OnShadowDraw() {
		auto ptrShadow = GetComponent<ShadowmapComp>();
		ptrShadow->OnShadowDraw();
	}
	void Player::OnSceneDraw() {
		auto ptrScene = GetComponent<SpSceneComp>();
		ptrScene->OnSceneDraw();
	}



}
// end namespace basecross
