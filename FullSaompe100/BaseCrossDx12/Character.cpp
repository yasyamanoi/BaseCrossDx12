/*!
@file Character.cpp
@brief キャラクタークラス
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void WallObject::OnInit() {
		auto transPtr = GetComponent<Transform>();
		transPtr->SetScale(Vec3(1.0f));
		transPtr->SetPosition(Vec3(0.0f));

		auto renderPtr = AddComponent<BcPNTStatic>();
		renderPtr->SetBaseMesh(App::GetBaseScene()->GetMesh(L"DEFAULT_CUBE"));
		renderPtr->SetBaseTexture(App::GetBaseScene()->GetTexture(L"WALL_TEX"));
	}

	void WallObject::OnUpdate() {
		auto transPtr = GetComponent<Transform>();
		float elapsedTime = App::GetElapsedTime();
		Quat qtspan(Vec3(0, 1, 1), -elapsedTime);
		auto quat = transPtr->GetQuaternion();
		quat *= qtspan;
		quat.normalize();
		transPtr->SetQuaternion(quat);
		auto pos = transPtr->GetPosition();
		pos.x += m_posSpan * elapsedTime;
		if (abs(pos.x) >= 5.0f) {
			m_posSpan *= -1.0f;
		}
		transPtr->SetPosition(pos);

	}

	void WallObject::OnDestroy() {

	}


	void SkyObject::OnInit() {
		auto transPtr = GetComponent<Transform>();
		transPtr->SetScale(Vec3(5.0f, 1.0f, 5.0f));
		transPtr->SetPosition(Vec3(0, -0.5f, 0.0));

		auto renderPtr = AddComponent<BcPNTStatic>();
		renderPtr->SetBaseMesh(App::GetBaseScene()->GetMesh(L"DEFAULT_CUBE"));
		renderPtr->SetBaseTexture(App::GetBaseScene()->GetTexture(L"SKY_TEX"));


	}

	void SkyObject::OnUpdate() {
		//何もしない
	}

	void SkyObject::OnDestroy() {

	}


}
// end basecross
