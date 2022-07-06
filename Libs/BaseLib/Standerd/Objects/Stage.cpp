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
		m_updateActive(true),
		m_physicsActive(false)
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
			auto baseDevice = App::GetBaseDevice();
			auto frames = baseDevice->GetBaseFrames();
			for (auto Ptr : m_waitAddObjectVec) {
				m_gameObjectVec.push_back(Ptr);
				for (UINT i = 0; i < baseDevice->m_frameCount; i++) {
					Ptr->OnInitFrame(frames[i]);
					Ptr->WriteConstantBuffers(frames[i]);
				}
			}
		}
		m_waitAddObjectVec.clear();
	}

	shared_ptr<GameObject> Stage::GetSharedGameObject(const wstring& key, bool exceptionActive)const {
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


	shared_ptr<GameObjectGroup> Stage::CreateSharedObjectGroup(const wstring& Key) {
		try {
			map<const wstring, shared_ptr<GameObjectGroup> >::iterator it;
			//重複キーの検査
			it = m_SharedGroupMap.find(Key);
			if (it != m_SharedGroupMap.end()) {
				//既に存在した
				//例外発生
				wstring keyerr = Key;
				throw BaseException(
					L"同名のシェアオブジェクト配列があります",
					keyerr,
					L"Stage::CreateSharedObjectGroup()"
				);
			}
			else {
				auto Ptr = ObjectFactory::Create<GameObjectGroup>();
				m_SharedGroupMap[Key] = Ptr;
				return Ptr;
			}
		}
		catch (...) {
			throw;
		}
	}

	shared_ptr<GameObjectGroup> Stage::GetSharedObjectGroup(const wstring& Key, bool ExceptionActive)const {
		//重複キーの検査
		auto it = m_SharedGroupMap.find(Key);
		if (it != m_SharedGroupMap.end()) {
			//ペアのsecondを返す
			return it->second;
		}
		else {
			//指定の名前が見つからなかった
			if (ExceptionActive) {
				//例外発生
				wstring keyerr = Key;
				throw BaseException(
					L"指定のキーが見つかりません",
					keyerr,
					L"Stage::GetSharedObjectGroup() const"
				);
			}
		}
		return nullptr;
	}

	void Stage::SetSharedObjectGroup(const wstring& Key, const shared_ptr<GameObjectGroup>& NewPtr) {
		//重複キーの検査
		auto it = m_SharedGroupMap.find(Key);
		if (it != m_SharedGroupMap.end()) {
			//例外発生
			wstring keyerr = Key;
			throw BaseException(
				L"同名のシェアオブジェクト配列があります",
				keyerr,
				L"Stage::SetSharedObjectGroup()"
			);
		}
		else {
			//指定の名前が見つからなかった
			//登録できる
			m_SharedGroupMap[Key] = NewPtr;
		}
	}

	uint16_t Stage::GetVacantPhysicsIndex() {
		uint16_t ret = UINT16_MAX;
		if (m_vacantPhysicsIndices.size() > 0) {
			//空いているIDがある。先頭を取得
			auto it = m_vacantPhysicsIndices.begin();
			ret = *it;
			//IDを使うのでそのIDを削除
			m_vacantPhysicsIndices.erase(ret);
		}
		return ret;
	}

	void Stage::SetVacantPhysicsIndex(uint16_t index) {
		m_vacantPhysicsIndices.insert(index);
	}

	void Stage::RemoveGameObject(const shared_ptr<GameObject>& Obj) {
		for (auto ptr : GetGameObjectVec()) {
			if (Obj == ptr) {
				RemoveBackGameObject(Obj);
				return;
			}
		}
		//存在しなくても何もしない
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


	//ステージ内の更新（シーンからよばれる）
	void Stage::UpdateStage() {
		bool physicsFlg = false;
		static float timeStep = 0.0f;
		timeStep += App::GetElapsedTime();
		
		if (timeStep >= m_basePhysics.GetTimeStep() * 0.5f) {
			physicsFlg = true;
			timeStep = 0.0f;
		}

		if (IsUpdatePerformanceActive()) {
			m_updatePerformance.Start();
		}
		//追加・削除まちオブジェクトの追加と削除
		SetWaitToObjectVec();
		//Transformコンポーネントの値をバックアップにコピー
		for (auto& ptr : GetGameObjectVec()) {
			if (ptr->IsUpdateActive()) {
				auto ptr2 = ptr->GetComponent<Transform>();
				ptr2->SetToBefore();
			}
		}
		//物理オブジェクトのフォースの初期化
		if (IsPhysicsActive()) {
			if (physicsFlg) {
				m_basePhysics.InitForce();
			}
		}

		//配置オブジェクトの更新処理
		for (auto& ptr : GetGameObjectVec()) {
			if (ptr->IsUpdateActive()) {
				ptr->OnUpdate();
			}
		}

		//自身の更新処理
		if (IsUpdateActive()) {
			OnUpdate();
		}
		//物理オブジェクトの更新
		if (IsPhysicsActive()) {
			if (physicsFlg) {
				m_basePhysics.Update(false);
			}
		}
		//配置オブジェクトのコンポーネント更新
		for (auto& ptr : GetGameObjectVec()) {
			if (ptr->IsUpdateActive()) {
				ptr->ComponentUpdate(physicsFlg);
			}
		}

		////衝突判定
		UpdateCollision();
		//配置オブジェクトの更新後処理
		for (auto& ptr : GetGameObjectVec()) {
			if (ptr->IsUpdateActive()) {
				ptr->OnUpdate2();
			}
		}
		//自身の更新後処理
		if (IsUpdateActive()) {
			OnUpdate2();
		}
		//カメラとライト
		OnUpdateLightCamera();
		if (IsUpdatePerformanceActive()) {
			m_updatePerformance.End();
		}
	}

	//衝突判定の更新（ステージから呼ぶ）
	//衝突判定をカスタマイズするためには
	//この関数を多重定義する
	void Stage::UpdateCollision() {
		if (m_collisionPerformance.IsAvtive()) {
			m_collisionPerformance.Start();
			//衝突判定管理者のUpdate(判定)
			m_collisionManager->OnUpdate();
			m_collisionPerformance.End();
		}
		else {
			//衝突判定管理者のUpdate(判定)
			m_collisionManager->OnUpdate();
		}
	}



	void Stage::OnPreCreate() {
		//コリジョン管理者の作成
		m_collisionManager = ObjectFactory::Create<CollisionManager>(GetThis<Stage>());
		//物理計算リセット
		m_basePhysics.Reset();
	}


	void Stage::OnDraw() {
		for (auto& v : m_gameObjectVec) {
			if (v->IsDrawActive()) {
				v->OnDraw();
			}
		}
	}

	void Stage::OnShadowmapDraw() {
		for (auto& v : m_gameObjectVec) {
			if (v->IsDrawActive()) {
				v->OnShadowmapDraw();
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
