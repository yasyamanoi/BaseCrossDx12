/*!
@file Character.cpp
@brief キャラクタークラス
*/


#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	物理計算する落下するボール
	//--------------------------------------------------------------------------------------
	//構築と破棄
	FallingBall::FallingBall(const shared_ptr<Stage>& StagePtr, const Vec3& Position, const Vec3& Velocity) :
		GameObject(StagePtr),
		m_Scale(0.5f),
		m_Pos(Position),
		m_Velocity(Velocity)
	{}

	FallingBall::~FallingBall() {}

	void FallingBall::Reset(const Vec3& Position, const Vec3& Velocity) {
		m_Pos = Position;
		m_Velocity = Velocity;
		auto ptrTransform = GetComponent<Transform>();
		ptrTransform->SetScale(m_Scale);
		ptrTransform->SetRotation(0, 0, 0);
		ptrTransform->SetPosition(m_Pos);

		//WorldMatrixをもとにRigidbodySphereのパラメータを作成
		PsSphereParam param(ptrTransform->GetWorldMatrix(), 1.0f, false, PsMotionType::MotionTypeActive);
		auto  ptrRigid = GetComponent<RigidbodySphere>();
		auto index = ptrRigid->GetIndex();
		ptrRigid->Reset(param, index);
		ptrRigid->SetLinearVelocity(m_Velocity);

		SetUpdateActive(true);
		SetDrawActive(true);


	}


	//初期化
	void FallingBall::OnCreate() {
		auto ptrTransform = GetComponent<Transform>();

		ptrTransform->SetScale(m_Scale);
		ptrTransform->SetRotation(0, 0, 0);
		ptrTransform->SetPosition(m_Pos);

		//WorldMatrixをもとにRigidbodySphereのパラメータを作成
		PsSphereParam param(ptrTransform->GetWorldMatrix(), 1.0f, false, PsMotionType::MotionTypeActive);
		//Rigidbodyをつける
		auto  ptrRigid = AddComponent<RigidbodySphere>(param);
		ptrRigid->SetLinearVelocity(m_Velocity);

		//影をつける
		auto ShadowPtr = AddComponent<Shadowmap>();
		ShadowPtr->SetMesh(L"DEFAULT_SPHERE");

		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMesh(L"DEFAULT_SPHERE");
		ptrDraw->SetTexture(L"WALL_TX");

		//オブジェクトのグループを得る
		auto group = GetStage()->GetSharedObjectGroup(L"FallingBallGroup");
		//グループに自分自身を追加
		group->IntoGroup(GetThis<FallingBall>());


	}

	void FallingBall::OnUpdate() {
		const float activeY = 25.0f;
		auto PtrTransform = GetComponent<Transform>();
		if (abs(PtrTransform->GetPosition().y) > activeY) {
			//範囲外に出たら消す
			SetUpdateActive(false);
			SetDrawActive(false);

//			GetStage()->RemoveGameObject(GetThis<GameObject>());
		}
	}



	//--------------------------------------------------------------------------------------
	///	物理計算する固定のボックス
	//--------------------------------------------------------------------------------------
	//構築と破棄
	FixedPsBox::FixedPsBox(const shared_ptr<Stage>& StagePtr,
		const Vec3& Scale,
		const Quat& Qt,
		const Vec3& Position
	) :
		GameObject(StagePtr),
		m_Scale(Scale),
		m_Qt(Qt),
		m_Position(Position)
	{}

	FixedPsBox::~FixedPsBox() {}
	//初期化
	void FixedPsBox::OnCreate() {

		auto ptrTrans = GetComponent<Transform>();

		ptrTrans->SetScale(m_Scale);
		ptrTrans->SetQuaternion(m_Qt);
		ptrTrans->SetPosition(m_Position);

		//影をつける
		auto ptrShadow = AddComponent<Shadowmap>();
		ptrShadow->SetMesh(L"DEFAULT_CUBE");

		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetFogEnabled(true);
		ptrDraw->SetMesh(L"DEFAULT_CUBE");
		ptrDraw->SetOwnShadowActive(true);
		ptrDraw->SetTexture(L"SKY_TX");
		//物理計算ボックス
		PsBoxParam param(ptrTrans->GetWorldMatrix(), 0.0f, true, PsMotionType::MotionTypeFixed);
		auto PsPtr = AddComponent<RigidbodyBox>(param);
	}

	//--------------------------------------------------------------------------------------
	///	物理計算するアクティブなボックス
	//--------------------------------------------------------------------------------------
	//構築と破棄
	ActivePsBox::ActivePsBox(const shared_ptr<Stage>& StagePtr,
		const Vec3& Scale,
		const Quat& Qt,
		const Vec3& Position
	) :
		GameObject(StagePtr),
		m_Scale(Scale),
		m_Qt(Qt),
		m_Position(Position)
	{}

	ActivePsBox::~ActivePsBox() {}
	//初期化
	void ActivePsBox::OnCreate() {
		auto ptrTrans = GetComponent<Transform>();

		ptrTrans->SetScale(m_Scale);
		ptrTrans->SetQuaternion(m_Qt);
		ptrTrans->SetPosition(m_Position);

		//影をつける
		auto ptrShadow = AddComponent<Shadowmap>();
		ptrShadow->SetMesh(L"DEFAULT_CUBE");

		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetFogEnabled(true);
		ptrDraw->SetMesh(L"DEFAULT_CUBE");
		ptrDraw->SetOwnShadowActive(true);
		ptrDraw->SetTexture(L"SKY_TX");
		//物理計算ボックス
		PsBoxParam param(ptrTrans->GetWorldMatrix(), 1.0f, true, PsMotionType::MotionTypeActive);
		auto PsPtr = AddComponent<RigidbodyBox>(param);
	}

	//--------------------------------------------------------------------------------------
	///	物理計算するアクティブな球体
	//--------------------------------------------------------------------------------------
	//構築と破棄
	ActivePsSphere::ActivePsSphere(const shared_ptr<Stage>& StagePtr,
		float Scale,
		const Quat& Qt,
		const Vec3& Position
	) :
		GameObject(StagePtr),
		m_Scale(Scale),
		m_Qt(Qt),
		m_Position(Position)
	{}

	ActivePsSphere::~ActivePsSphere() {}
	//初期化
	void ActivePsSphere::OnCreate() {

		auto ptrTransform = GetComponent<Transform>();

		ptrTransform->SetScale(Vec3(m_Scale));
		ptrTransform->SetQuaternion(m_Qt);
		ptrTransform->SetPosition(m_Position);

		//影をつける
		auto ptrShadow = AddComponent<Shadowmap>();
		ptrShadow->SetMesh(L"DEFAULT_SPHERE");

		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetFogEnabled(true);
		ptrDraw->SetMesh(L"DEFAULT_SPHERE");
		ptrDraw->SetOwnShadowActive(true);
		ptrDraw->SetTexture(L"SKY_TX");

		//物理計算球体
		//WorldMatrixをもとにRigidbodySphereのパラメータを作成
		PsSphereParam param(ptrTransform->GetWorldMatrix(), 1.0f, true, PsMotionType::MotionTypeActive);
		auto PsPtr = AddComponent<RigidbodySphere>(param);
	}


}
// end basecross
