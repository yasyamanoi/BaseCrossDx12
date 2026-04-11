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
		//OBB衝突j判定を付ける
		auto ptrColl = AddComponent<CollisionObb>();
		//この衝突判定は、動かない、という特徴をつける
		ptrColl->SetFixed(true);
		//影（シャドウマップ）をつける
		auto ptrShadow = AddComponent<Shadowmap>();
		//L"DEFAULT_CUBE"というメッシュをつける
		ptrShadow->AddBaseMesh(L"DEFAULT_CUBE");
		//描画コンポーネントに、BcPNTStaticDraw、を指定する
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		//ここにもL"DEFAULT_CUBE"というメッシュをつける
		ptrDraw->AddBaseMesh(L"DEFAULT_CUBE");
		//L"SKY_TX"というテクスチャをつける
		ptrDraw->AddBaseTexture(L"SKY_TX");
		//自分自身に影が移りこむようにする
		ptrDraw->SetOwnShadowActive(true);
	}

	//--------------------------------------------------------------------------------------
	// 四角のオブジェクト
	//--------------------------------------------------------------------------------------
	WallBox::WallBox(const std::shared_ptr<Stage>& stage, const TransParam& param) :
		GameObject(stage),
		m_totalTime(0.0)
	{
		m_transParam = param;
	}
	WallBox::~WallBox() {}

	void WallBox::OnCreate() {
		//OBB衝突j判定を付ける
		auto ptrColl = AddComponent<CollisionObb>();
		//重力をつける
		auto ptrGra = AddComponent<Gravity>();

		auto ptrShadow = AddComponent<Shadowmap>();
		ptrShadow->AddBaseMesh(L"DEFAULT_CUBE");
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
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
		param.position.x = (float)sin(m_totalTime) * 2.0f;
	}

	//--------------------------------------------------------------------------------------
	//	追いかける配置オブジェクト
	//--------------------------------------------------------------------------------------
	//構築と破棄
	SeekObject::SeekObject(const std::shared_ptr<Stage>& StagePtr, const Vec3& startPos) :
		GameObject(StagePtr),
		m_StartPos(startPos),
		m_StateChangeSize(5.0f),
		m_Force(0),
		m_Velocity(0),
		m_Weight(1.0f),
		m_MaxSpeed(10.0f),
		m_MaxForce(30.0f),
		m_Decl(3.0f)
	{
		m_transParam.position = startPos;
	}
	SeekObject::~SeekObject() {}

	//初期化
	void SeekObject::OnCreate() {
		auto ptrTransform = GetComponent<Transform>();
		ptrTransform->SetPosition(m_StartPos);
		ptrTransform->SetScale(0.125f, 0.25f, 0.25f);
		ptrTransform->SetRotation(0.0f, 0.0f, 0.0f);


		//Obbの衝突判定をつける
		auto ptrColl = AddComponent<CollisionObb>();
		//重力をつける
		auto ptrGra = AddComponent<Gravity>();


		//分離行動をつける
		//オブジェクトのグループを得る
		auto group = GetStage()->GetSharedObjectGroup(L"SeekGroup");
		//グループに自分自身を追加
		group->IntoGroup(GetThis<SeekObject>());
		//分離行動をつける
		auto ptrSep = AddComponent<Separation>();
		//分離行動にグループを追加する
		ptrSep->SetGameObjectGroup(group);

		//追跡行動をつける
		auto ptrSeek = AddComponent<Seek>();
		//到着行動をつける
		auto ptrArr = AddComponent<Arrive>();

		//影をつける
		auto ptrShadow = AddComponent<Shadowmap>();
		ptrShadow->AddBaseMesh(L"DEFAULT_CUBE");

		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetFogEnabled(true);
		ptrDraw->AddBaseMesh(L"DEFAULT_CUBE");
		ptrDraw->AddBaseTexture(L"TRACE_TX");
		//透明処理をする
		SetAlphaActive(true);

		//ステートマシンの構築
		m_StateMachine.reset(new StateMachine<SeekObject>(GetThis<SeekObject>()));
		//最初のステートをSeekFarStateに設定
		m_StateMachine->ChangeState(SeekFarState::Instance());
	}


	//操作
	void SeekObject::OnUpdate(double elapsedTime) {
		m_Force = Vec3(0);
		//ステートマシンのUpdateを行う
		//この中でステートの切り替えが行われる
		m_StateMachine->Update();
		RotToHead(1.0f);
	}


	Vec3 SeekObject::GetTargetPos()const {
		auto ptrTarget = GetStage()->GetSharedGameObject(L"Player");
		return ptrTarget->GetComponent<Transform>()->GetWorldPosition();
	}


	void SeekObject::ApplyForce() {
		float elapsedTime = (float)Scene::GetElapsedTime();
		m_Velocity += m_Force * elapsedTime;
		auto ptrTrans = GetComponent<Transform>();
		auto pos = ptrTrans->GetPosition();
		pos += m_Velocity * elapsedTime;
		ptrTrans->SetPosition(pos);
	}

	void SeekObject::RotToHead(float LerpFact) {
		if (LerpFact <= 0.0f) {
			//補間係数が0以下なら何もしない
			return;
		}
		//回転の更新
		//Velocityの値で、回転を変更する
		//これで進行方向を向くようになる
		auto PtrTransform = GetComponent<Transform>();
		Vec3 Velocity = PtrTransform->GetVelocity();
		if (Velocity.length() > 0.0f) {
			Vec3 Temp = Velocity;
			Temp.normalize();
			float ToAngle = atan2(Temp.x, Temp.z);
			Quat Qt;
			Qt.rotationRollPitchYawFromVector(Vec3(0, ToAngle, 0));
			Qt.normalize();
			//現在の回転を取得
			Quat NowQt = PtrTransform->GetQuaternion();
			//現在と目標を補間
			//現在と目標を補間
			if (LerpFact >= 1.0f) {
				NowQt = Qt;
			}
			else {
				NowQt = XMQuaternionSlerp(NowQt, Qt, LerpFact);
			}
			PtrTransform->SetQuaternion(NowQt);
		}

	}




	//--------------------------------------------------------------------------------------
	//	プレイヤーから遠いときの移動
	//--------------------------------------------------------------------------------------
	std::shared_ptr<SeekFarState> SeekFarState::Instance() {
		static std::shared_ptr<SeekFarState> instance(new SeekFarState);
		return instance;
	}
	void SeekFarState::Enter(const std::shared_ptr<SeekObject>& Obj) {
	}
	void SeekFarState::Execute(const std::shared_ptr<SeekObject>& Obj) {
		auto trans = Obj->GetComponent<Transform>();


		auto seekComp = Obj->GetComponent<Seek>();
		auto separationComp = Obj->GetComponent<Separation>();

		//フォースの初期化
		Vec3 force(0.0f);
		//フォースの計算
		force = seekComp->Execute(Obj->GetVelocity(), Obj->GetTargetPos(), trans->GetWorldPosition())
			* Obj->GetWeight();
		force += separationComp->Execute(force);
		//そのforceを設定する
		Obj->SetForce(force);
		//その結果をもとにオブジェクトの位置に反映する
		Obj->ApplyForce();
		//結果として、ターゲットとの間の距離がObj->GetStateChangeSize()
		//より近ければ、ステートSeekNearStateに変換
		float f = bsm::bsmUtil::length(Obj->GetComponent<Transform>()->GetPosition() - Obj->GetTargetPos());
		if (f < Obj->GetStateChangeSize()) {
			Obj->GetStateMachine()->ChangeState(SeekNearState::Instance());
		}
	}

	void SeekFarState::Exit(const std::shared_ptr<SeekObject>& Obj) {
	}

	//--------------------------------------------------------------------------------------
	//	プレイヤーから近いときの移動
	//--------------------------------------------------------------------------------------
	std::shared_ptr<SeekNearState> SeekNearState::Instance() {
		static std::shared_ptr<SeekNearState> instance(new SeekNearState);
		return instance;
	}
	void SeekNearState::Enter(const std::shared_ptr<SeekObject>& Obj) {
	}
	void SeekNearState::Execute(const std::shared_ptr<SeekObject>& Obj) {
		auto trans = Obj->GetComponent<Transform>();
		auto arriveComp = Obj->GetComponent<Arrive>();
		auto separationComp = Obj->GetComponent<Separation>();

		//フォースの初期化
		Vec3 force(0.0f);
		//フォースの計算
		force = arriveComp->Execute(force,Obj->GetVelocity(), Obj->GetTargetPos());
		force += separationComp->Execute(force);
		//そのforceを設定する
		Obj->SetForce(force);
		//その結果をもとにオブジェクトの位置に反映する
		Obj->ApplyForce();
		//結果として、ターゲットとの間の距離がObj->GetStateChangeSize()
		//より遠ければ、ステートSeekFarStateに変換
		float f = bsm::bsmUtil::length(Obj->GetComponent<Transform>()->GetPosition() - Obj->GetTargetPos());
		if (f >= Obj->GetStateChangeSize()) {
			Obj->GetStateMachine()->ChangeState(SeekFarState::Instance());
		}
	}
	void SeekNearState::Exit(const std::shared_ptr<SeekObject>& Obj) {
	}



}
// end namespace basecross
