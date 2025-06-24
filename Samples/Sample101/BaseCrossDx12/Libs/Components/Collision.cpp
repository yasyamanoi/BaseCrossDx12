/*!
@file Collision.cpp
@brief 衝突判定コンポーネント実体
*/
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	class Collision : public Component ;
	//	用途: 衝突判定コンポーネントの親クラス
	//--------------------------------------------------------------------------------------
	//構築と破棄
	Collision::Collision(const std::shared_ptr<GameObject>& GameObjectPtr) :
		Component(GameObjectPtr),
		m_Fixed(false),
		m_AfterCollision(AfterCollision::Auto),
		m_SleepActive(false),
		m_SleepCheckWorldMatrix(),
		m_SleepCheckTimer(0.0f),
		m_SleepTime(2.0f),
		m_IsSleep(false)
	{
	}
	Collision::~Collision() {}

	bool Collision::GetFixed() const {
		return m_Fixed;
	}
	bool Collision::IsFixed() const {
		return m_Fixed;
	}
	void Collision::SetFixed(bool b) {
		m_Fixed = b;
	}

	AfterCollision Collision::GetAfterCollision() const {
		return m_AfterCollision;
	}
	void Collision::SetAfterCollision(AfterCollision a) {
		m_AfterCollision = a;
	}

	Vec3 Collision::GetVelocity() const {
		return GetGameObject()->GetComponent<Transform>()->GetVelocity();
	}

	std::vector<std::weak_ptr<GameObject>>& Collision::GetExcludeCollisionGameObjects() {
		return m_ExcludeCollisionGameObjects;
	}

	void Collision::AddExcludeCollisionGameObject(const std::shared_ptr<GameObject>& obj) {
		m_ExcludeCollisionGameObjects.push_back(obj);
	}

	void  Collision::RemoveExcludeCollisionGameObject(const std::shared_ptr<GameObject>& obj) {
		for (auto it = m_ExcludeCollisionGameObjects.begin();
			it != m_ExcludeCollisionGameObjects.end();
			it++)
		{
			auto shobj = (*it).lock();
			if (shobj && (shobj == obj)) {
				m_ExcludeCollisionGameObjects.erase(it);
				return;
			}
		}
	}



	std::shared_ptr<GameObjectGroup> Collision::GetExcludeCollisionGroup() const {
		auto shptr = m_ExcludeCollisionGroup.lock();
		if (shptr) {
			return shptr;
		}
		return nullptr;
	}

	void Collision::SetExcludeCollisionGroup(const std::shared_ptr<GameObjectGroup>& Group) {
		m_ExcludeCollisionGroup = Group;
	}

	void Collision::SetExcludeCollisionGroup(const std::wstring& GroupStr) {
		m_ExcludeCollisionGroup = GetGameObject()->GetStage()->GetSharedObjectGroup(GroupStr);
	}



	bool Collision::FindExcludeCollisionTag(const std::wstring& tagstr) const {
		if (m_ExcludeCollisionTags.find(tagstr) == m_ExcludeCollisionTags.end()) {
			return false;
		}
		return true;
	}
	void  Collision::AddExcludeCollisionTag(const std::wstring& tagstr) {
		if (tagstr == L"") {
			//空白なら例外
			throw BaseException(
				L"設定するタグが空です",
				L"if (tagstr == L\"\")",
				L"Collision::AddExcludeCollisionTag()"
			);
		}
		m_ExcludeCollisionTags.insert(tagstr);

	}
	void  Collision::RemoveExcludeCollisionTag(const std::wstring& tagstr) {
		m_ExcludeCollisionTags.erase(tagstr);
	}



	bool Collision::IsExcludeCollisionObject(const std::shared_ptr<GameObject>& Obj) const {
		auto& tagset = Obj->GetTagSet();
		auto it = tagset.begin();
		while (it != tagset.end()) {
			if (FindExcludeCollisionTag(*it)) {
				return true;
			}
			it++;
		}
		auto shptr = m_ExcludeCollisionGroup.lock();
		if (shptr) {
			auto Vec = shptr->GetGroupVector();
			for (auto& v : Vec) {
				auto shobj = v.lock();
				if (shobj && (shobj == Obj)) {
					return true;
				}
			}
		}
		for (auto v : m_ExcludeCollisionGameObjects) {
			auto shobj = v.lock();
			if (shobj && (shobj == Obj)) {
				return true;
			}
		}

		return false;
	}

	bool Collision::IsSleepActive()const {
		return m_SleepActive;
	}

	void Collision::SetSleepActive(bool b) {
		m_SleepActive = b;
	}

	float Collision::GetSleepTime() const {
		return m_SleepTime;
	}

	void Collision::SetSleepTime(float f) {
		if (!IsSleepActive() || IsFixed()) {
			return;
		}
		m_SleepTime = true;
	}

	void Collision::SleepCheckSet() {
		if (!IsSleepActive() || IsFixed()) {
			return;
		}
		auto WorldMat = GetGameObject()->GetComponent<Transform>()->GetWorldMatrix();
		if (WorldMat.nearEqual(m_SleepCheckWorldMatrix, 0.01f)) {
			float elapsedTime = (float)Scene::GetElapsedTime();
			m_SleepCheckTimer += elapsedTime;
			if (m_SleepCheckTimer >= m_SleepTime) {
				m_IsSleep = true;
			}
		}
		else {
			m_IsSleep = false;
			m_SleepCheckWorldMatrix = WorldMat;
			m_SleepCheckTimer = 0.0f;
		}
	}

	bool Collision::IsSleep()const {
		if (!IsSleepActive() || IsFixed()) {
			return false;
		}
		return m_IsSleep;
	}

	void Collision::WakeUp() {
		if (!IsSleepActive() || IsFixed()) {
			return;
		}
		if (m_IsSleep) {
			auto WorldMat = GetGameObject()->GetComponent<Transform>()->GetWorldMatrix();
			m_IsSleep = false;
			m_SleepCheckWorldMatrix = WorldMat;
			m_SleepCheckTimer = 0.0f;
		}
	}

	//操作
	std::shared_ptr<CollisionManager> Collision::GetCollisionManager() const {
		return GetGameObject()->GetStage()->GetCollisionManager();
	}

	void Collision::OnUpdate(double elapsedTime) {
	}

	//--------------------------------------------------------------------------------------
	//	class CollisionSphere : public Collision ;
	//	用途: 球衝突判定コンポーネント
	//--------------------------------------------------------------------------------------
	//構築と破棄
	CollisionSphere::CollisionSphere(const std::shared_ptr<GameObject>& GameObjectPtr) :
		Collision(GameObjectPtr),
		m_MakedDiameter(1.0f),
		m_IsHitVolumeIndex(0),
		m_CalcScaling(CalcScaling::YScale)
	{
	}
	CollisionSphere::~CollisionSphere() {}

	void CollisionSphere::OnCreate() {
		SetDrawActive(false);
	}

	//アクセサ
	float CollisionSphere::GetMakedDiameter() const {
		return m_MakedDiameter;
	}
	void CollisionSphere::SetMakedDiameter(float f) {
		m_MakedDiameter = f;
	}
	float CollisionSphere::GetMakedRadius() const {
		return m_MakedDiameter * 0.5f;
	}
	void CollisionSphere::SetMakedRadius(float f) {
		m_MakedDiameter = f * 2.0f;
	}

	CalcScaling CollisionSphere::GetCalcScaling() const {
		return m_CalcScaling;

	}
	void CollisionSphere::SetCalcScaling(CalcScaling s) {
		m_CalcScaling = s;
	}


	SPHERE CollisionSphere::GetSphere() const {
		auto TransPtr = GetGameObject()->GetComponent<Transform>();
		Mat4x4 MatBase;
		MatBase.scale(Vec3(m_MakedDiameter, m_MakedDiameter, m_MakedDiameter));
		MatBase *= TransPtr->GetWorldMatrix();
		//このオブジェクトのSPHEREを作成
		SPHERE Ret(MatBase.transInMatrix(), MatBase.scaleInMatrix().x * 0.5f);
		switch (m_CalcScaling) {
		case CalcScaling::XScale:
			Ret.m_Radius = MatBase.scaleInMatrix().x * 0.5f;
			break;
		case CalcScaling::YScale:
			Ret.m_Radius = MatBase.scaleInMatrix().y * 0.5f;
			break;
		case CalcScaling::ZScale:
			Ret.m_Radius = MatBase.scaleInMatrix().z * 0.5f;
			break;
		default:
			break;
		}
		return Ret;
	}


	SPHERE CollisionSphere::GetBeforeSphere() const {
		auto TransPtr = GetGameObject()->GetComponent<Transform>();
		Mat4x4 MatBase;
		MatBase.scale(Vec3(m_MakedDiameter, m_MakedDiameter, m_MakedDiameter));
		MatBase *= TransPtr->GetBeforeWorldMatrix();
		//このオブジェクトのSPHEREを作成
		SPHERE Ret(MatBase.transInMatrix(), MatBase.scaleInMatrix().x * 0.5f);
		switch (m_CalcScaling) {
		case CalcScaling::XScale:
			Ret.m_Radius = MatBase.scaleInMatrix().x * 0.5f;
			break;
		case CalcScaling::YScale:
			Ret.m_Radius = MatBase.scaleInMatrix().y * 0.5f;
			break;
		case CalcScaling::ZScale:
			Ret.m_Radius = MatBase.scaleInMatrix().z * 0.5f;
			break;
		default:
			break;
		}
		return Ret;
	}

	bool CollisionSphere::SimpleCollisionCall(const std::shared_ptr<Collision>& Src) {
		return Src->SimpleCollision(GetThis<CollisionSphere>());
	}


	bool CollisionSphere::SimpleCollision(const std::shared_ptr<CollisionSphere>& DestColl) {
		auto SrcSp = GetSphere();
		auto DestSp = DestColl->GetSphere();
		if (!HitTest::AABB_AABB(SrcSp.GetWrappedAABB(), DestSp.GetWrappedAABB())) {
			return false;
		}
		if (HitTest::SPHERE_SPHERE(SrcSp, DestSp)) {
			return true;
		}
		return false;
	}

	bool CollisionSphere::SimpleCollision(const std::shared_ptr<CollisionCapsule>& DestColl) {
		auto SrcSp = GetSphere();
		auto DestCap = DestColl->GetCapsule();
		if (!HitTest::AABB_AABB(SrcSp.GetWrappedAABB(), DestCap.GetWrappedAABB())) {
			return false;
		}
		Vec3 d;
		if (HitTest::SPHERE_CAPSULE(SrcSp, DestCap, d)) {
			return true;
		}
		return false;

	}

	bool CollisionSphere::SimpleCollision(const std::shared_ptr<CollisionObb>& DestColl) {
		auto SrcSp = GetSphere();
		auto DestObb = DestColl->GetObb();
		if (!HitTest::AABB_AABB(SrcSp.GetWrappedAABB(), DestObb.GetWrappedAABB())) {
			return false;
		}
		Vec3 d;
		if (HitTest::SPHERE_OBB(SrcSp, DestObb, d)) {
			return true;
		}
		return false;
	}

	bool CollisionSphere::SimpleCollision(const std::shared_ptr<CollisionRect>& DestColl) {
		auto SrcSp = GetSphere();
		auto DestCol = DestColl->GetColRect();
		if (!HitTest::AABB_AABB(SrcSp.GetWrappedAABB(), DestCol.GetWrappedAABB())) {
			return false;
		}
		Vec3 d;
		if (HitTest::SPHERE_COLRECT(SrcSp, DestCol, d)) {
			return true;
		}
		return false;
	}


	void CollisionSphere::CollisionCall(const std::shared_ptr<Collision>& Src) {
		Src->CollisionTest(GetThis<CollisionSphere>());
	}

	void CollisionSphere::CollisionTest(const std::shared_ptr<CollisionSphere>& DestColl) {

		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		auto PtrDestTransform = DestColl->GetGameObject()->GetComponent<Transform>();
		Vec3 SrcVelocity = PtrTransform->GetVelocity();
		Vec3 DestVelocity = PtrDestTransform->GetVelocity();
		//前回のターンからの時間
		float ElapsedTime = (float)Scene::GetElapsedTime();
		//球の場合は、すべて移動以外変化なしとする
		SPHERE SrcSphere = GetSphere();
		SPHERE SrcBeforSphere = GetBeforeSphere();
		//相手のCollisionSphere
		SPHERE DestSphere = DestColl->GetSphere();
		SPHERE DestBeforeSphere = DestColl->GetBeforeSphere();
		//簡易的な判定
		if (!HitTest::SPHERE_SPHERE(SrcSphere, DestSphere)) {
			return;
		}
		Vec3 SpanVelocity = SrcVelocity - DestVelocity;
		float HitTime = 0;
		if (HitTest::CollisionTestSphereSphere(SrcBeforSphere, SpanVelocity, DestBeforeSphere, 0, ElapsedTime, HitTime)) {
			CollisionPair pair;
			pair.m_Src = GetThis<Collision>();
			pair.m_Dest = DestColl;
			SPHERE SrcChkSphere = SrcBeforSphere;
			SrcChkSphere.m_Center += SrcVelocity * HitTime;
			pair.m_SrcCalcHitCenter = SrcChkSphere.m_Center;
			SPHERE DestChkSphere = DestBeforeSphere;
			DestChkSphere.m_Center += DestVelocity * HitTime;
			pair.m_DestCalcHitCenter = DestChkSphere.m_Center;
			pair.m_SrcHitNormal = SrcChkSphere.m_Center - DestChkSphere.m_Center;
			pair.m_SrcHitNormal.normalize();
			pair.m_CalcHitPoint = (DestChkSphere.m_Center + SrcChkSphere.m_Center) * 0.5f;
			GetCollisionManager()->InsertNewPair(pair);
		}
	}

	void CollisionSphere::CollisionTest(const std::shared_ptr<CollisionCapsule>& DestColl) {

		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		auto PtrDestTransform = DestColl->GetGameObject()->GetComponent<Transform>();
		Vec3 SrcVelocity = PtrTransform->GetVelocity();
		Vec3 DestVelocity = PtrDestTransform->GetVelocity();
		//前回のターンからの時間
		float ElapsedTime = (float)Scene::GetElapsedTime();
		//移動以外変化なし
		SPHERE SrcSphere = GetSphere();
		SPHERE SrcBeforSphere = GetBeforeSphere();
		//相手
		CAPSULE DestCap = DestColl->GetCapsule();
		CAPSULE DestBeforeCap = DestColl->GetBeforeCapsule();
		//簡易的な判定
		Vec3 ret;
		if (!HitTest::SPHERE_CAPSULE(SrcSphere, DestCap, ret)) {
			return;
		}
		Vec3 SpanVelocity = SrcVelocity - DestVelocity;
		float HitTime = 0;
		if (HitTest::CollisionTestSphereCapsule(SrcBeforSphere, SpanVelocity, DestBeforeCap, 0, ElapsedTime, HitTime)) {
			CollisionPair pair;
			pair.m_Src = GetThis<Collision>();
			pair.m_Dest = DestColl;
			SPHERE SrcChkSphere = SrcBeforSphere;
			SrcChkSphere.m_Center += SrcVelocity * HitTime;
			pair.m_SrcCalcHitCenter = SrcChkSphere.m_Center;
			CAPSULE DestChkCapsule = DestBeforeCap;
			pair.m_DestCalcHitCenter = DestChkCapsule.GetCenter() + DestVelocity * HitTime;
			DestChkCapsule.SetCenter(pair.m_DestCalcHitCenter);
			Vec3 ret;
			HitTest::SPHERE_CAPSULE(SrcChkSphere, DestChkCapsule, ret);
			//衝突した瞬間で法線を計算
			pair.m_SrcHitNormal = SrcChkSphere.m_Center - ret;
			pair.m_SrcHitNormal.normalize();
			pair.m_CalcHitPoint = ret;
			GetCollisionManager()->InsertNewPair(pair);
		}
	}


	void CollisionSphere::CollisionTest(const std::shared_ptr<CollisionObb>& DestColl) {

		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		auto PtrDestTransform = DestColl->GetGameObject()->GetComponent<Transform>();
		Vec3 SrcVelocity = PtrTransform->GetVelocity();
		Vec3 DestVelocity = PtrDestTransform->GetVelocity();
		//前回のターンからの時間
		float ElapsedTime = (float)Scene::GetElapsedTime();
		//移動以外変化なし
		SPHERE SrcSphere = GetSphere();
		SPHERE SrcBeforSphere = GetBeforeSphere();
		//相手
		OBB DestObb = DestColl->GetObb();
		OBB DestBeforeObb = DestColl->GetBeforeObb();
		//簡易的な判定
		Vec3 ret;
		if (!HitTest::SPHERE_OBB(SrcSphere, DestObb, ret)) {
			return;
		}
		Vec3 SpanVelocity = SrcVelocity - DestVelocity;
		float HitTime = 0;
		if (HitTest::CollisionTestSphereObb(SrcBeforSphere, SpanVelocity, DestBeforeObb, 0, ElapsedTime, HitTime)) {
			CollisionPair pair;
			pair.m_Src = GetThis<Collision>();
			pair.m_Dest = DestColl;
			SPHERE SrcChkSphere = SrcBeforSphere;
			SrcChkSphere.m_Center += SrcVelocity * HitTime;
			pair.m_SrcCalcHitCenter = SrcChkSphere.m_Center;
			OBB DestChkObb = DestBeforeObb;
			DestChkObb.m_Center += DestVelocity * HitTime;
			pair.m_DestCalcHitCenter = DestChkObb.m_Center;
			Vec3 ret;
			HitTest::SPHERE_OBB(SrcChkSphere, DestChkObb, ret);
			//衝突した瞬間で法線を計算
			pair.m_SrcHitNormal = SrcChkSphere.m_Center - ret;
			pair.m_SrcHitNormal.normalize();
			pair.m_CalcHitPoint = ret;
			GetCollisionManager()->InsertNewPair(pair);
		}
	}

	void CollisionSphere::CollisionTest(const std::shared_ptr<CollisionRect>& DestColl) {

		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		auto PtrDestTransform = DestColl->GetGameObject()->GetComponent<Transform>();
		Vec3 SrcVelocity = PtrTransform->GetVelocity();
		Vec3 DestVelocity = PtrDestTransform->GetVelocity();
		//前回のターンからの時間
		float ElapsedTime = (float)Scene::GetElapsedTime();
		//移動以外変化なし
		SPHERE SrcSphere = GetSphere();
		SPHERE SrcBeforSphere = GetBeforeSphere();
		//相手
		COLRECT DestRect = DestColl->GetColRect();
		COLRECT DestBeforeRect = DestColl->GetBeforeColRect();
		//簡易的な判定
		Vec3 ret;
		if (!HitTest::SPHERE_COLRECT(SrcSphere, DestRect, ret)) {
			return;
		}
		Vec3 SpanVelocity = SrcVelocity - DestVelocity;
		float HitTime = 0;
		if (HitTest::CollisionTestSphereRect(SrcBeforSphere, SpanVelocity, DestBeforeRect, 0, ElapsedTime, HitTime)) {
			CollisionPair pair;
			pair.m_Src = GetThis<Collision>();
			pair.m_Dest = DestColl;
			SPHERE SrcChkSphere = SrcBeforSphere;
			SrcChkSphere.m_Center += SrcVelocity * HitTime;
			pair.m_SrcCalcHitCenter = SrcChkSphere.m_Center;
			COLRECT DestChkRect = DestBeforeRect;
			DestBeforeRect.m_Center += DestVelocity * HitTime;
			pair.m_DestCalcHitCenter = DestChkRect.m_Center;
			Vec3 ret;
			HitTest::SPHERE_COLRECT(SrcChkSphere, DestChkRect, ret);
			//衝突した瞬間で法線を計算
			pair.m_SrcHitNormal = SrcChkSphere.m_Center - ret;
			pair.m_SrcHitNormal.normalize();
			pair.m_CalcHitPoint = ret;
			GetCollisionManager()->InsertNewPair(pair);
		}
	}


	Vec3 CollisionSphere::GetCenterPosition()const {
		SPHERE SrcSphere = GetSphere();
		return SrcSphere.m_Center;
	}

	AABB CollisionSphere::GetEnclosingAABB()const {
		AABB enc = GetBeforeSphere().GetWrappedAABB();
		enc.UnionAABB(GetSphere().GetWrappedAABB());
		return enc;
	}

	AABB CollisionSphere::GetWrappedAABB()const {
		return GetSphere().GetWrappedAABB();
	}

	//--------------------------------------------------------------------------------------
	//	class CollisionCapsule : public Collision ;
	//	用途: カプセル衝突判定コンポーネント
	//--------------------------------------------------------------------------------------
	//構築と破棄
	CollisionCapsule::CollisionCapsule(const std::shared_ptr<GameObject>& GameObjectPtr) :
		Collision(GameObjectPtr),
		m_MakedDiameter(1.0f),
		m_MakedHeight(1.0f),
		m_IsHitVolumeIndex(0)
	{
	}
	CollisionCapsule::~CollisionCapsule() {}

	//初期化
	void CollisionCapsule::OnCreate() {
		SetDrawActive(false);
	}

	//アクセサ
	float CollisionCapsule::GetMakedDiameter() const {
		return m_MakedDiameter;
	}
	void CollisionCapsule::SetMakedDiameter(float f) {
		m_MakedDiameter = f;
	}
	float CollisionCapsule::GetMakedRadius() const {
		return m_MakedDiameter * 0.5f;
	}
	void CollisionCapsule::SetMakedRadius(float f) {
		m_MakedDiameter = f * 2.0f;
	}

	float CollisionCapsule::GetMakedHeight() const {
		return m_MakedHeight;
	}
	void CollisionCapsule::SetMakedHeight(float f) {
		m_MakedHeight = f;
	}

	CAPSULE CollisionCapsule::GetCapsule()const {
		auto TransPtr = GetGameObject()->GetComponent<Transform>();
		auto WorldMatrix = TransPtr->GetWorldMatrix();
		auto WorldCapsule = CAPSULE(
			m_MakedDiameter * 0.5f,
			Vec3(0, m_MakedHeight * -0.5f, 0),
			Vec3(0, m_MakedHeight * 0.5f, 0),
			WorldMatrix);
		return WorldCapsule;
	}


	CAPSULE CollisionCapsule::GetBeforeCapsule()const {
		auto TransPtr = GetGameObject()->GetComponent<Transform>();
		auto BeforeWorldMatrix = TransPtr->GetBeforeWorldMatrix();
		auto BeforeWorldCapsule = CAPSULE(
			m_MakedDiameter * 0.5f,
			Vec3(0, m_MakedHeight * -0.5f, 0),
			Vec3(0, m_MakedHeight * 0.5f, 0),
			BeforeWorldMatrix);
		return BeforeWorldCapsule;
	}

	bool CollisionCapsule::SimpleCollisionCall(const std::shared_ptr<Collision>& Src) {
		return Src->SimpleCollision(GetThis<CollisionCapsule>());
	}

	bool CollisionCapsule::SimpleCollision(const std::shared_ptr<CollisionSphere>& DestColl) {
		auto SrcCap = GetCapsule();
		auto DestSp = DestColl->GetSphere();
		if (!HitTest::AABB_AABB(SrcCap.GetWrappedAABB(), DestSp.GetWrappedAABB())) {
			return false;
		}
		Vec3 d;
		if (HitTest::SPHERE_CAPSULE(DestSp, SrcCap, d)) {
			return true;
		}
		return false;
	}

	bool CollisionCapsule::SimpleCollision(const std::shared_ptr<CollisionCapsule>& DestColl) {
		auto SrcCap = GetCapsule();
		auto DestCap = DestColl->GetCapsule();
		if (!HitTest::AABB_AABB(SrcCap.GetWrappedAABB(), DestCap.GetWrappedAABB())) {
			return false;
		}
		Vec3 d1, d2;
		if (HitTest::CAPSULE_CAPSULE(SrcCap, DestCap, d1, d2)) {
			return true;
		}
		return false;
	}

	bool CollisionCapsule::SimpleCollision(const std::shared_ptr<CollisionObb>& DestColl) {
		auto SrcCap = GetCapsule();
		auto DestObb = DestColl->GetObb();
		if (!HitTest::AABB_AABB(SrcCap.GetWrappedAABB(), DestObb.GetWrappedAABB())) {
			return false;
		}
		Vec3 d;
		if (HitTest::CAPSULE_OBB(SrcCap, DestObb, d)) {
			return true;
		}
		return false;
	}

	bool CollisionCapsule::SimpleCollision(const std::shared_ptr<CollisionRect>& DestColl) {
		auto SrcCap = GetCapsule();
		auto DestRect = DestColl->GetColRect();
		if (!HitTest::AABB_AABB(SrcCap.GetWrappedAABB(), DestRect.GetWrappedAABB())) {
			return false;
		}
		Vec3 d;
		if (HitTest::CAPSULE_COLRECT(SrcCap, DestRect, d)) {
			return true;
		}
		return false;
	}



	void CollisionCapsule::CollisionCall(const std::shared_ptr<Collision>& Src) {
		Src->CollisionTest(GetThis<CollisionCapsule>());
	}


	void CollisionCapsule::CollisionTest(const std::shared_ptr<CollisionSphere>& DestColl) {

		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		auto PtrDestTransform = DestColl->GetGameObject()->GetComponent<Transform>();
		Vec3 SrcVelocity = PtrTransform->GetVelocity();
		Vec3 DestVelocity = PtrDestTransform->GetVelocity();
		//前回のターンからの時間
		float ElapsedTime = (float)Scene::GetElapsedTime();
		//移動以外変化なし
		CAPSULE SrcCapsule = GetCapsule();
		CAPSULE SrcBeforCapsule = GetBeforeCapsule();
		//相手のCollisionSphere
		SPHERE DestSphere = DestColl->GetSphere();
		SPHERE DestBeforeSphere = DestColl->GetBeforeSphere();
		//簡易的な判定
		Vec3 ret;
		if (!HitTest::SPHERE_CAPSULE(DestSphere, SrcCapsule, ret)) {
			return;
		}
		//SPHEREとCAPSULEの扱いが逆になる
		Vec3 SpanVelocity = DestVelocity - SrcVelocity;
		float HitTime = 0;
		if (HitTest::CollisionTestSphereCapsule(DestBeforeSphere, SpanVelocity, SrcBeforCapsule, 0, ElapsedTime, HitTime)) {
			CollisionPair pair;
			pair.m_Src = GetThis<Collision>();
			pair.m_Dest = DestColl;
			CAPSULE SrcChkCapsule = SrcBeforCapsule;
			pair.m_SrcCalcHitCenter = SrcChkCapsule.GetCenter() + SrcVelocity * HitTime;
			SrcChkCapsule.SetCenter(pair.m_SrcCalcHitCenter);
			SPHERE DestChkSphere = DestBeforeSphere;
			DestChkSphere.m_Center += DestVelocity * HitTime;
			pair.m_DestCalcHitCenter = DestChkSphere.m_Center;
			Vec3 ret;
			HitTest::SPHERE_CAPSULE(DestChkSphere, SrcChkCapsule, ret);
			//衝突した瞬間で法線を計算
			pair.m_SrcHitNormal = ret - DestChkSphere.m_Center;
			pair.m_SrcHitNormal.normalize();
			pair.m_CalcHitPoint = ret;
			GetCollisionManager()->InsertNewPair(pair);
		}
	}

	void CollisionCapsule::CollisionTest(const std::shared_ptr<CollisionCapsule>& DestColl) {

		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		auto PtrDestTransform = DestColl->GetGameObject()->GetComponent<Transform>();
		Vec3 SrcVelocity = PtrTransform->GetVelocity();
		Vec3 DestVelocity = PtrDestTransform->GetVelocity();
		//前回のターンからの時間
		float ElapsedTime = (float)Scene::GetElapsedTime();
		//移動以外変化なし
		CAPSULE SrcCapsule = GetCapsule();
		CAPSULE SrcBeforCapsule = GetBeforeCapsule();
		//相手のCollisionCapsule
		CAPSULE DestCapsule = DestColl->GetCapsule();
		CAPSULE DestBeforeCapsule = DestColl->GetBeforeCapsule();
		//簡易的な判定
		Vec3 ret1, ret2;
		if (!HitTest::CAPSULE_CAPSULE(SrcCapsule, DestCapsule, ret1, ret2)) {
			return;
		}
		Vec3 SpanVelocity = SrcVelocity - DestVelocity;
		float HitTime = 0;
		if (HitTest::CollisionTestCapsuleCapsule(SrcBeforCapsule, SpanVelocity, DestBeforeCapsule, 0, ElapsedTime, HitTime)) {
			CollisionPair pair;
			pair.m_Src = GetThis<Collision>();
			pair.m_Dest = DestColl;
			CAPSULE SrcChkCapsule = SrcBeforCapsule;
			pair.m_SrcCalcHitCenter = SrcChkCapsule.GetCenter() + SrcVelocity * HitTime;
			SrcChkCapsule.SetCenter(pair.m_SrcCalcHitCenter);
			CAPSULE DestChkCapsule = DestBeforeCapsule;
			pair.m_DestCalcHitCenter = DestChkCapsule.GetCenter() + DestVelocity * HitTime;
			DestChkCapsule.SetCenter(pair.m_DestCalcHitCenter);
			Vec3 ret1, ret2;
			HitTest::CAPSULE_CAPSULE(SrcChkCapsule, DestChkCapsule, ret1, ret2);
			//接点へのベクトル
			//DestCapの線分とRetVec1の線分上の最近接点とRetVec1の法線
			Vec3 Start = DestChkCapsule.m_PointBottom;
			Vec3 End = DestChkCapsule.m_PointTop;
			float t;
			Vec3 RetVec;
			HitTest::ClosetPtPointSegment(ret1, Start, End, t, RetVec);
			pair.m_SrcHitNormal = ret1 - RetVec;
			pair.m_SrcHitNormal.normalize();
			pair.m_CalcHitPoint = ret1;
			GetCollisionManager()->InsertNewPair(pair);
		}
	}

	void CollisionCapsule::CollisionTest(const std::shared_ptr<CollisionObb>& DestColl) {

		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		auto PtrDestTransform = DestColl->GetGameObject()->GetComponent<Transform>();
		Vec3 SrcVelocity = PtrTransform->GetVelocity();
		Vec3 DestVelocity = PtrDestTransform->GetVelocity();
		//前回のターンからの時間
		float ElapsedTime = (float)Scene::GetElapsedTime();
		//移動以外変化なし
		CAPSULE SrcCapsule = GetCapsule();
		CAPSULE SrcBeforCapsule = GetBeforeCapsule();
		//相手のCollisionObb
		OBB DestObb = DestColl->GetObb();
		OBB DestBeforeObb = DestColl->GetBeforeObb();
		//簡易的な判定
		Vec3 ret;
		if (!HitTest::CAPSULE_OBB(SrcCapsule, DestObb, ret)) {
			return;
		}
		Vec3 SpanVelocity = SrcVelocity - DestVelocity;
		float HitTime = 0;
		if (HitTest::CollisionTestCapsuleObb(SrcBeforCapsule, SpanVelocity, DestBeforeObb, 0, ElapsedTime, HitTime)) {
			CollisionPair pair;
			pair.m_Src = GetThis<Collision>();
			pair.m_Dest = DestColl;
			CAPSULE SrcChkCapsule = SrcBeforCapsule;
			pair.m_SrcCalcHitCenter = SrcChkCapsule.GetCenter() + SrcVelocity * HitTime;
			SrcChkCapsule.SetCenter(pair.m_SrcCalcHitCenter);
			OBB DestChkObb = DestBeforeObb;
			DestChkObb.m_Center += DestVelocity * HitTime;
			pair.m_DestCalcHitCenter = DestChkObb.m_Center;
			Vec3 RetVec;
			HitTest::CAPSULE_OBB(SrcChkCapsule, DestChkObb, RetVec);
			//接点へのベクトル
			float t;
			Vec3 SegPoint;
			HitTest::ClosetPtPointSegment(RetVec, SrcChkCapsule.m_PointBottom, SrcChkCapsule.m_PointTop, t, SegPoint);
			pair.m_SrcHitNormal = SegPoint - RetVec;
			pair.m_CalcHitPoint = RetVec;
			pair.m_SrcHitNormal.normalize();
			GetCollisionManager()->InsertNewPair(pair);
		}
	}

	void CollisionCapsule::CollisionTest(const std::shared_ptr<CollisionRect>& DestColl) {

		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		auto PtrDestTransform = DestColl->GetGameObject()->GetComponent<Transform>();
		Vec3 SrcVelocity = PtrTransform->GetVelocity();
		Vec3 DestVelocity = PtrDestTransform->GetVelocity();
		//前回のターンからの時間
		float ElapsedTime = (float)Scene::GetElapsedTime();
		//移動以外変化なし
		CAPSULE SrcCapsule = GetCapsule();
		CAPSULE SrcBeforCapsule = GetBeforeCapsule();
		//相手
		COLRECT DestRect = DestColl->GetColRect();
		COLRECT DestBeforeRect = DestColl->GetBeforeColRect();
		//簡易的な判定
		Vec3 ret;
		if (!HitTest::CAPSULE_COLRECT(SrcCapsule, DestRect, ret)) {
			return;
		}
		Vec3 SpanVelocity = SrcVelocity - DestVelocity;
		float HitTime = 0;
		if (HitTest::CollisionTestCapsuleRect(SrcBeforCapsule, SpanVelocity, DestBeforeRect, 0, ElapsedTime, HitTime)) {
			CollisionPair pair;
			pair.m_Src = GetThis<Collision>();
			pair.m_Dest = DestColl;
			CAPSULE SrcChkCapsule = SrcBeforCapsule;
			pair.m_SrcCalcHitCenter = SrcChkCapsule.GetCenter() + SrcVelocity * HitTime;
			SrcChkCapsule.SetCenter(pair.m_SrcCalcHitCenter);
			COLRECT DestChkRect = DestBeforeRect;
			DestChkRect.m_Center += DestVelocity * HitTime;
			pair.m_DestCalcHitCenter = DestChkRect.m_Center;
			HitTest::CAPSULE_COLRECT(SrcChkCapsule, DestChkRect, ret);
			//衝突した瞬間で法線を計算
			pair.m_SrcHitNormal = SrcChkCapsule.GetCenter() - ret;
			pair.m_SrcHitNormal.normalize();
			pair.m_CalcHitPoint = ret;
			GetCollisionManager()->InsertNewPair(pair);
		}
	}


	Vec3 CollisionCapsule::GetCenterPosition()const {
		CAPSULE SrcCapsule = GetCapsule();
		return SrcCapsule.GetCenter();
	}

	AABB CollisionCapsule::GetEnclosingAABB()const {
		AABB enc = GetBeforeCapsule().GetWrappedAABB();
		enc.UnionAABB(GetCapsule().GetWrappedAABB());
		return enc;
	}

	AABB CollisionCapsule::GetWrappedAABB()const {
		return GetCapsule().GetWrappedAABB();
	}

	//--------------------------------------------------------------------------------------
	//	class CollisionObb : public Collision ;
	//	用途: Obb衝突判定コンポーネント
	//--------------------------------------------------------------------------------------
	//構築と破棄
	CollisionObb::CollisionObb(const std::shared_ptr<GameObject>& GameObjectPtr) :
		Collision(GameObjectPtr),
		m_Size(1.0f),
		m_ChkOnUnderLaySize(0.1f)
	{
	}
	CollisionObb::~CollisionObb() {}

	//初期化
	void CollisionObb::OnCreate() {
		SetDrawActive(false);
	}


	//アクセサ
	float CollisionObb::GetMakedSize() const {
		return m_Size;
	}
	void CollisionObb::SetMakedSize(float f) {
		m_Size = f;
	}

	OBB CollisionObb::GetObb() const {
		auto TransPtr = GetGameObject()->GetComponent<Transform>();
		auto WorldMatrix = TransPtr->GetWorldMatrix();
		Mat4x4 MatBase;
		MatBase.scale(Vec3(m_Size, m_Size, m_Size));
		MatBase *= WorldMatrix;
		auto WorldObb = OBB(Vec3(m_Size, m_Size, m_Size), MatBase);
		return WorldObb;
	}


	OBB CollisionObb::GetBeforeObb() const {
		auto TransPtr = GetGameObject()->GetComponent<Transform>();
		auto BeforeWorldMatrix = TransPtr->GetBeforeWorldMatrix();
		Mat4x4 MatBase;
		MatBase.scale(Vec3(m_Size, m_Size, m_Size));
		MatBase *= BeforeWorldMatrix;
		auto BeforeWorldObb = OBB(Vec3(m_Size, m_Size, m_Size), MatBase);
		return BeforeWorldObb;
	}

	bool CollisionObb::SimpleCollisionCall(const std::shared_ptr<Collision>& Src) {
		return Src->SimpleCollision(GetThis<CollisionObb>());
	}

	bool CollisionObb::SimpleCollision(const std::shared_ptr<CollisionSphere>& DestColl) {
		auto SrcObb = GetObb();
		Vec3 d;
		auto DestSp = DestColl->GetSphere();
		if (!HitTest::AABB_AABB(SrcObb.GetWrappedAABB(), DestSp.GetWrappedAABB())) {
			return false;
		}
		if (HitTest::SPHERE_OBB(DestSp, SrcObb, d)) {
			return true;
		}
		return false;
	}

	bool CollisionObb::SimpleCollision(const std::shared_ptr<CollisionCapsule>& DestColl) {
		auto SrcObb = GetObb();
		auto DestCap = DestColl->GetCapsule();
		if (!HitTest::AABB_AABB(SrcObb.GetWrappedAABB(), DestCap.GetWrappedAABB())) {
			return false;
		}
		Vec3 d;
		if (HitTest::CAPSULE_OBB(DestCap, SrcObb, d)) {
			return true;
		}
		return false;
	}

	bool CollisionObb::SimpleCollision(const std::shared_ptr<CollisionObb>& DestColl) {
		auto SrcObb = GetObb();
		auto DestObb = DestColl->GetObb();
		if (!HitTest::AABB_AABB(SrcObb.GetWrappedAABB(), DestObb.GetWrappedAABB())) {
			return false;
		}
		if (HitTest::OBB_OBB(SrcObb, DestObb)) {
			return true;
		}
		return false;
	}

	bool CollisionObb::SimpleCollision(const std::shared_ptr<CollisionRect>& DestColl) {
		auto SrcObb = GetObb();
		auto DestRect = DestColl->GetColRect();
		if (!HitTest::AABB_AABB(SrcObb.GetWrappedAABB(), DestRect.GetWrappedAABB())) {
			return false;
		}
		if (HitTest::OBB_COLRECT(SrcObb, DestRect)) {
			return true;
		}
		return false;
	}


	void CollisionObb::CollisionCall(const std::shared_ptr<Collision>& Src) {
		Src->CollisionTest(GetThis<CollisionObb>());
	}

	void CollisionObb::CollisionTest(const std::shared_ptr<CollisionSphere>& DestColl) {

		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		auto PtrDestTransform = DestColl->GetGameObject()->GetComponent<Transform>();
		Vec3 SrcVelocity = PtrTransform->GetVelocity();
		Vec3 DestVelocity = PtrDestTransform->GetVelocity();
		//前回のターンからの時間
		float ElapsedTime = (float)Scene::GetElapsedTime();
		//移動以外変化なし
		OBB SrcObb = GetObb();
		OBB SrcBeforeObb = GetBeforeObb();
		//相手
		SPHERE DestSphere = DestColl->GetSphere();
		SPHERE DestBeforeSphere = DestColl->GetBeforeSphere();
		//簡易的な判定
		Vec3 ret;
		if (!HitTest::SPHERE_OBB(DestSphere, SrcObb, ret)) {
			return;
		}
		//SPHEREとOBBの扱いが逆になる
		Vec3 SpanVelocity = DestVelocity - SrcVelocity;
		float HitTime = 0;
		if (HitTest::CollisionTestSphereObb(DestBeforeSphere, SpanVelocity, SrcBeforeObb, 0, ElapsedTime, HitTime)) {
			CollisionPair pair;
			pair.m_Src = GetThis<Collision>();
			pair.m_Dest = DestColl;
			OBB SrcChkObb = SrcBeforeObb;
			SrcChkObb.m_Center += SrcVelocity * HitTime;
			pair.m_SrcCalcHitCenter = SrcChkObb.m_Center;
			SPHERE DestChkSphere = DestBeforeSphere;
			DestChkSphere.m_Center += DestVelocity * HitTime;
			pair.m_DestCalcHitCenter = DestChkSphere.m_Center;
			Vec3 ret;
			HitTest::SPHERE_OBB(DestChkSphere, SrcChkObb, ret);
			//衝突した瞬間で法線を計算
			pair.m_SrcHitNormal = ret - DestChkSphere.m_Center;
			pair.m_CalcHitPoint = ret;
			pair.m_SrcHitNormal.normalize();
			GetCollisionManager()->InsertNewPair(pair);
		}
	}

	void CollisionObb::CollisionTest(const std::shared_ptr<CollisionCapsule>& DestColl) {

		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		auto PtrDestTransform = DestColl->GetGameObject()->GetComponent<Transform>();
		Vec3 SrcVelocity = PtrTransform->GetVelocity();
		Vec3 DestVelocity = PtrDestTransform->GetVelocity();
		//前回のターンからの時間
		float ElapsedTime = (float)Scene::GetElapsedTime();
		//移動以外変化なし
		OBB SrcObb = GetObb();
		OBB SrcBeforeObb = GetBeforeObb();
		//相手
		CAPSULE DestCapsule = DestColl->GetCapsule();
		CAPSULE DestBeforeCapsule = DestColl->GetBeforeCapsule();
		//簡易的な判定
		Vec3 ret;
		if (!HitTest::CAPSULE_OBB(DestCapsule, SrcObb, ret)) {
			return;
		}
		//SPHEREとOBBの扱いが逆になる
		Vec3 SpanVelocity = DestVelocity - SrcVelocity;
		float HitTime = 0;
		if (HitTest::CollisionTestCapsuleObb(DestBeforeCapsule, SpanVelocity, SrcBeforeObb, 0, ElapsedTime, HitTime)) {
			CollisionPair pair;
			pair.m_Src = GetThis<Collision>();
			pair.m_Dest = DestColl;
			OBB SrcChkObb = SrcBeforeObb;
			SrcChkObb.m_Center += SrcVelocity * HitTime;
			pair.m_SrcCalcHitCenter = SrcChkObb.m_Center;
			CAPSULE DestChkCapsule = DestBeforeCapsule;
			pair.m_DestCalcHitCenter = DestChkCapsule.GetCenter() + DestVelocity * HitTime;
			DestChkCapsule.SetCenter(pair.m_DestCalcHitCenter);
			Vec3 RetVec;
			HitTest::CAPSULE_OBB(DestChkCapsule, SrcChkObb, RetVec);
			//接点へのベクトル
			float t;
			Vec3 SegPoint;
			HitTest::ClosetPtPointSegment(RetVec, DestChkCapsule.m_PointBottom, DestChkCapsule.m_PointTop, t, SegPoint);
			pair.m_SrcHitNormal = RetVec - SegPoint;
			pair.m_SrcHitNormal.normalize();
			pair.m_CalcHitPoint = RetVec;
			GetCollisionManager()->InsertNewPair(pair);
		}
	}


	void CollisionObb::CollisionTest(const std::shared_ptr<CollisionObb>& DestColl) {

		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		auto PtrDestTransform = DestColl->GetGameObject()->GetComponent<Transform>();
		Vec3 SrcVelocity = PtrTransform->GetVelocity();
		Vec3 DestVelocity = PtrDestTransform->GetVelocity();
		//前回のターンからの時間
		float ElapsedTime = (float)Scene::GetElapsedTime();
		//移動以外変化なし
		OBB SrcObb = GetObb();
		OBB SrcBeforeObb = GetBeforeObb();
		//相手
		OBB DestObb = DestColl->GetObb();
		OBB DestBeforeObb = DestColl->GetBeforeObb();
		//簡易的な判定
		if (!HitTest::OBB_OBB(SrcObb, DestObb)) {
			return;
		}
		Vec3 SpanVelocity = SrcVelocity - DestVelocity;
		float HitTime = 0;
		if (HitTest::CollisionTestObbObb(SrcBeforeObb, SpanVelocity, DestBeforeObb, 0, ElapsedTime, HitTime)) {
			CollisionPair pair;
			pair.m_Src = GetThis<Collision>();
			pair.m_Dest = DestColl;
			OBB SrcChkObb = SrcBeforeObb;
			SrcChkObb.m_Center += SrcVelocity * HitTime;
			pair.m_SrcCalcHitCenter = SrcChkObb.m_Center;
			OBB DestChkObb = DestBeforeObb;
			DestChkObb.m_Center += DestVelocity * HitTime;
			pair.m_DestCalcHitCenter = DestChkObb.m_Center;
			Vec3 RetVec;
			//SrcのOBBとDestの最近接点を得る
			HitTest::ClosestPtPointOBB(SrcChkObb.m_Center, DestChkObb, RetVec);
			//接点へのベクトル
			//衝突した瞬間で法線を計算
			pair.m_SrcHitNormal = SrcChkObb.m_Center - RetVec;
			pair.m_CalcHitPoint = RetVec;
			pair.m_SrcHitNormal.normalize();
			GetCollisionManager()->InsertNewPair(pair);
		}
	}

	void CollisionObb::CollisionTest(const std::shared_ptr<CollisionRect>& DestColl) {

		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		auto PtrDestTransform = DestColl->GetGameObject()->GetComponent<Transform>();
		Vec3 SrcVelocity = PtrTransform->GetVelocity();
		Vec3 DestVelocity = PtrDestTransform->GetVelocity();
		//前回のターンからの時間
		float ElapsedTime = (float)Scene::GetElapsedTime();
		//移動以外変化なし
		OBB SrcObb = GetObb();
		OBB SrcBeforeObb = GetBeforeObb();
		//相手
		COLRECT DestRect = DestColl->GetColRect();
		COLRECT DestBeforeRect = DestColl->GetBeforeColRect();
		//簡易的な判定
		if (!HitTest::OBB_COLRECT(SrcObb, DestRect)) {
			return;
		}
		Vec3 SpanVelocity = SrcVelocity - DestVelocity;
		float HitTime = 0;
		if (HitTest::CollisionTestObbRect(SrcBeforeObb, SpanVelocity, DestBeforeRect, 0, ElapsedTime, HitTime)) {
			CollisionPair pair;
			pair.m_Src = GetThis<Collision>();
			pair.m_Dest = DestColl;
			OBB SrcChkObb = SrcBeforeObb;
			SrcChkObb.m_Center += SrcVelocity * HitTime;
			pair.m_SrcCalcHitCenter = SrcChkObb.m_Center;
			COLRECT DestChkRect = DestBeforeRect;
			DestChkRect.m_Center += DestVelocity * HitTime;
			pair.m_DestCalcHitCenter = DestChkRect.m_Center;
			Vec3 RetVec;
			//SrcのOBBとDestの最近接点を得る
			HitTest::ClosetPtPointPlane(SrcChkObb.m_Center, DestChkRect.GetPLANE(), RetVec);
			//接点へのベクトル
			//衝突した瞬間で法線を計算
			pair.m_SrcHitNormal = SrcChkObb.m_Center - RetVec;
			pair.m_CalcHitPoint = RetVec;
			pair.m_SrcHitNormal.normalize();
			GetCollisionManager()->InsertNewPair(pair);
		}
	}


	Vec3 CollisionObb::GetCenterPosition()const {
		OBB SrcObb = GetObb();
		return SrcObb.m_Center;
	}

	AABB CollisionObb::GetEnclosingAABB()const {
		AABB enc = GetBeforeObb().GetWrappedAABB();
		enc.UnionAABB(GetObb().GetWrappedAABB());
		return enc;
	}

	AABB CollisionObb::GetWrappedAABB()const {
		return GetObb().GetWrappedAABB();
	}


	//--------------------------------------------------------------------------------------
	//	class CollisionRect : public Collision ;
	//	用途: Rect(矩形)衝突判定コンポーネント
	//--------------------------------------------------------------------------------------
	//構築と破棄
	CollisionRect::CollisionRect(const std::shared_ptr<GameObject>& GameObjectPtr) :
		Collision(GameObjectPtr),
		m_Size(1.0f)
	{
	}
	CollisionRect::~CollisionRect() {}

	//初期化
	void CollisionRect::OnCreate() {
		SetFixed(true),
			SetDrawActive(false);
	}

	//アクセサ
	void CollisionRect::SetFixed(bool b) {
		if (!b) {
			throw BaseException(
				L"CollisionRectはFixed以外は選択できません",
				L"if (!b)",
				L"CollisionRect::SetFixed()"
			);
		}
		else {
			Collision::SetFixed(true);
		}
	}



	float CollisionRect::GetMakedSize() const {
		return m_Size;
	}
	void CollisionRect::SetMakedSize(float f) {
		m_Size = f;
	}

	COLRECT CollisionRect::GetColRect() const {
		auto TrasnsPtr = GetGameObject()->GetComponent<Transform>();
		COLRECT rect(GetMakedSize(), GetMakedSize(), TrasnsPtr->GetWorldMatrix());
		return rect;
	}

	COLRECT CollisionRect::GetBeforeColRect() const {
		auto TrasnsPtr = GetGameObject()->GetComponent<Transform>();
		COLRECT rect(GetMakedSize(), GetMakedSize(), TrasnsPtr->GetBeforeWorldMatrix());
		return rect;
	}

	bool CollisionRect::SimpleCollisionCall(const std::shared_ptr<Collision>& Src) {
		return Src->SimpleCollision(GetThis<CollisionRect>());
	}

	void CollisionRect::CollisionCall(const std::shared_ptr<Collision>& Src) {
		Src->CollisionTest(GetThis<CollisionRect>());
	}

	AABB CollisionRect::GetEnclosingAABB()const {
		AABB enc = GetBeforeColRect().GetWrappedAABB();
		enc.UnionAABB(GetColRect().GetWrappedAABB());
		return enc;
	}

	AABB CollisionRect::GetWrappedAABB()const {
		COLRECT SrcColRect = GetColRect();
		return SrcColRect.GetWrappedAABB();
	}

	Vec3 CollisionRect::GetCenterPosition()const {
		COLRECT SrcColRect = GetColRect();
		return SrcColRect.m_Center;
	}

}
//end namespace basecross

