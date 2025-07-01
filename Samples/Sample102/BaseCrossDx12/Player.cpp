/*!
@file Player.cpp
@brief プレイヤー　実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// プレイヤー
	//--------------------------------------------------------------------------------------
	Player::Player(const std::shared_ptr<Stage>& stage, const TransParam& param) :
		GameObject(stage),
		m_Speed(6.0f)
	{
		m_tempParam = param;
	}

	Vec2 Player::GetInputState() const {
		Vec2 ret;
		//コントローラの取得
		auto cntlVec = App::GetInputDevice().GetControlerVec();
		ret.x = 0.0f;
		ret.y = 0.0f;
		WORD wButtons = 0;
		if (cntlVec[0].bConnected) {
			ret.x = cntlVec[0].fThumbLX;
			ret.y = cntlVec[0].fThumbLY;
		}
		return ret;
	}


	Vec3 Player::GetMoveVector() {
		m_angle = Vec3(0, 0, 0);
		//入力の取得
		auto inPut = GetInputState();
		float moveX = inPut.x;
		float moveZ = inPut.y;
		if (moveX != 0 || moveZ != 0) {
			float moveLength = 0;	//動いた時のスピード
			auto ptrTransform = GetComponent<Transform>();
			auto ptrCamera = GetStage()->GetCamera();
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
			m_angle = Vec3(cos(totalAngle), 0, sin(totalAngle));
			//正規化する
			m_angle.normalize();
			//移動サイズを設定。
			m_angle *= moveSize;
			//Y軸は変化させない
			m_angle.y = 0;
		}
		return m_angle;
	}

	void Player::MovePlayer() {
		float elapsedTime = (float)Scene::GetElapsedTime();
		auto angle = GetMoveVector();
		//RigidbodyDynamicコンポーネントを取り出す
		auto ptrRigid = GetComponent<RigidbodyDynamic>();
		auto pRigDynamic = ptrRigid->GetRigidDynamic();
		if (angle.length() > 0.0f) {
			Vec3 tmpVelo = angle * m_Speed;
			pRigDynamic->setLinearVelocity(bsmUtil::ToPxVec3(tmpVelo));
			//auto pos = GetComponent<Transform>()->GetPosition();
			//pos += angle * (float)elapsedTime * m_Speed;
			//GetComponent<Transform>()->SetPosition(pos);
		}
		else {
			pRigDynamic->setLinearVelocity(bsmUtil::ToPxVec3(Vec3(0)));
		}
	}




	void Player::OnCreate() {
		GetStage()->SetSharedGameObject(L"Player", GetThis<Player>());

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
		auto pRigDynamicComp = AddComponent<RigidbodyDynamic>(pxParam);
		auto pRigDynamic = pRigDynamicComp->GetRigidDynamic();


		auto ptrShadow = AddComponent<Shadowmap>();
		ptrShadow->AddBaseMesh(L"DEFAULT_SPHERE");

		auto ptrDraw = AddComponent<BcStaticDraw>();
		ptrDraw->AddBaseMesh(L"DEFAULT_SPHERE");
		ptrDraw->AddBaseTexture(L"TRACE_TX");
		//透明処理
		SetAlphaActive(true);
		//カメラを得る
		auto ptrCamera = std::dynamic_pointer_cast<MyCamera>(GetStage()->GetCamera());
		if (ptrCamera) {
			//MyCameraである
			//MyCameraに注目するオブジェクト（プレイヤー）の設定
			ptrCamera->SetTargetObject(GetThis<GameObject>());
			ptrCamera->SetTargetToAt(Vec3(0, 0.25f, 0));
		}

	}

	void Player::OnUpdate(double elapsedTime) {
		//コントローラチェックして入力があればコマンド呼び出し
		m_InputHandler.PushHandle(GetThis<Player>());
		MovePlayer();
	}

	void Player::OnUpdate2(double elapsedTime) {
		//回転の計算
		//if (m_angle.length() > 0.0f) {
		//	auto utilPtr = GetBehavior<UtilBehavior>();
		//	utilPtr->RotToHead(m_angle, 1.0f);
		//}
	}


	void Player::OnPushA() {
	}

}
// end namespace basecross
