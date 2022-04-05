/*!
@file Stage.cpp
@brief 親ステージクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#include "stdafx.h"

namespace basecross {
	//--------------------------------------------------------------------------------------
	///	親ステージ
	//--------------------------------------------------------------------------------------
	Stage::Stage():
		m_UpdateActive(true)
	{}


	void Stage::PushBackGameObject(const shared_ptr<GameObject>& Ptr) {
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
	void Stage::RemoveBackGameObject(const shared_ptr<GameObject>& Ptr) {
		m_waitRemoveObjectVec.push_back(Ptr);
	}

	void Stage::RemoveTargetGameObject(const shared_ptr<GameObject>& targetobj) {
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


	void Stage::SetWaitToObjectVec() {
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

	shared_ptr<GameObject> Stage::GetSharedGameObjectEx(const wstring& key, bool exceptionActive)const {
		map<const wstring, weak_ptr<GameObject> >::const_iterator it;
		//重複キーの検査
		it = m_SharedMap.find(key);
		if (it != m_SharedMap.end()) {
			auto shptr = it->second.lock();
			if (shptr) {
				return shptr;
			}
			else {
				//すでに無効
				if (exceptionActive) {
					//例外発生
					wstring keyerr = key;
					throw BaseException(
						L"オブジェクトが無効です",
						keyerr,
						L"Stage::GetSharedGameObject()"
					);
				}
			}
		}
		else {
			//指定の名前が見つからなかった
			if (exceptionActive) {
				//例外発生
				wstring keyerr = key;
				throw BaseException(
					L"オブジェクトが見つかりません",
					keyerr,
					L"Stage::GetSharedGameObject()"
				);
			}
		}
		return nullptr;
	}

	void Stage::SetSharedGameObject(const wstring& key, const shared_ptr<GameObject>& ptr) {
		map<const wstring, weak_ptr<GameObject> >::iterator it;
		//重複キーの検査
		it = m_SharedMap.find(key);
		if (it != m_SharedMap.end()) {
			//既に存在した
			//例外発生
			wstring keyerr = key;
			throw BaseException(
				L"同名のシェアオブジェクトがあります",
				keyerr,
				L"Stage::SetSharedGameObject()"
			);
		}
		else {
			m_SharedMap[key] = ptr;
		}
	}



	void Stage::OnInitFrame(BaseFrame* pBaseFrame) {
		for (auto& v : m_gameObjectVec) {
			v->OnInitFrame(pBaseFrame);
		}
	}
	void Stage::WriteConstantBuffers(BaseFrame* pBaseFrame) {
		for (auto& v : m_gameObjectVec) {
			v->WriteConstantBuffers(pBaseFrame);
		}
	}

	void Stage::UpdateGameObjects() {
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

	void Stage::OnRender() {
		for (auto& v : m_gameObjectVec) {
			if (v->IsRenderActive()) {
				v->OnRender();
			}
		}
	}


	void Stage::OnDestroy() {
		for (auto& v : m_gameObjectVec) {
			v->OnDestroy();
		}
	}
}
//end namespace basecross
