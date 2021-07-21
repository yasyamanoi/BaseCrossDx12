/*!
@file Stage.cpp
@brief 親ステージクラス
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	親ステージ
	//--------------------------------------------------------------------------------------
	void BaseStage::PushBackGameObject(const shared_ptr<GameObject>& Ptr) {
		//このステージはクリエイト後である
		if (IsCreated()) {
			m_waitAddObjectVec.push_back(Ptr);
		}
		else {
			//クリエイト前
			m_gameObjectVec.push_back(Ptr);
		}
		Ptr->TransformInit();
	}

	//削除オブジェクトの設定
	void BaseStage::RemoveBackGameObject(const shared_ptr<GameObject>& Ptr) {
		m_waitRemoveObjectVec.push_back(Ptr);
	}

	void BaseStage::RemoveTargetGameObject(const shared_ptr<GameObject>& targetobj) {
		auto it = m_gameObjectVec.begin();
		while (it != m_gameObjectVec.end()) {
			if (*it == targetobj) {
				//削除されることをオブジェクトに伝える
				targetobj->ComponentDestroy();
				targetobj->OnDestroy();
				m_gameObjectVec.erase(it);
				return;
			}
			it++;
		}
	}


	void BaseStage::SetWaitToObjectVec() {
		if (!m_waitRemoveObjectVec.empty()) {
			for (auto Ptr : m_waitRemoveObjectVec) {
				RemoveTargetGameObject(Ptr);
			}
		}
		m_waitRemoveObjectVec.clear();
		if (!m_waitAddObjectVec.empty()) {
			for (auto Ptr : m_waitAddObjectVec) {
				m_gameObjectVec.push_back(Ptr);
			}
		}
		m_waitAddObjectVec.clear();
	}



	void BaseStage::OnInitFrame(FrameResource* pFrameResource) {
		for (auto& v : m_gameObjectVec) {
			v->OnInitFrame(pFrameResource);
		}
	}
	void BaseStage::WriteConstantBuffers(FrameResource* pFrameResource) {
		for (auto& v : m_gameObjectVec) {
			v->WriteConstantBuffers(pFrameResource);
		}
	}

	void BaseStage::UpdateGameObjects() {
		SetWaitToObjectVec();
		for (auto& v : m_gameObjectVec) {
			if (v->IsUpdateActive()) {
				auto v2 = v->GetComponent<Transform>();
				v2->SetToBefore();
				auto veloPtr = v->GetComponent<Velocity>(false);
				if (veloPtr && veloPtr->IsUpdateActive()) {
					veloPtr->ClearLiniaFource();
				}
			}
		}
		//物理オブジェクトのフォースの初期化
//		m_basePhysics.InitForce();
		for (auto& v : m_gameObjectVec) {
			if (v->IsUpdateActive()) {
				v->OnUpdate();
			}
		}
		//m_basePhysics.Update(false);
		for (auto& v : m_gameObjectVec) {
			if (v->IsUpdateActive()) {
				v->ComponentUpdate();
			}
		}
	}

	void BaseStage::OnRender() {
		for (auto& v : m_gameObjectVec) {
			if (v->IsRenderActive()) {
				v->OnRender();
			}
		}
	}


	void BaseStage::OnDestroy() {
		for (auto& v : m_gameObjectVec) {
			v->OnDestroy();
		}
	}


}
// end basecross