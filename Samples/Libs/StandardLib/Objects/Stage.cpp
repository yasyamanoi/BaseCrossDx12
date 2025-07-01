/*!
@file Stage.cpp
@brief 親ステージクラス　実体
*/

#include "stdafx.h"

namespace basecross {

	void Stage::OnUpdateConstantBuffers()
	{
		for (auto& v : m_gameObjectVec) {
			v->OnUpdateConstantBuffers();
		}
	}

	void Stage::OnCommitConstantBuffers()
	{
		for (auto& v : m_gameObjectVec) {
			v->OnCommitConstantBuffers();
		}
	}

	//ステージ内の更新（シーンからよばれる）
	void Stage::UpdateStage() {
		//追加・削除まちオブジェクトの追加と削除
		SetWaitToObjectVec();
		//Transformコンポーネントの値をバックアップにコピー
		for (auto& ptr : GetGameObjectVec()) {
			if (ptr->IsUpdateActive()) {
				auto ptr2 = ptr->GetComponent<Transform>();
				ptr2->SetToBefore();
			}
		}
		//配置オブジェクトの更新処理
		for (auto& ptr : GetGameObjectVec()) {
			if (ptr->IsUpdateActive()) {
				ptr->OnUpdate(Scene::GetElapsedTime());
			}
		}
		//自身の更新処理
		if (IsUpdateActive()) {
			OnUpdate(Scene::GetElapsedTime());
		}
		//配置オブジェクトのコンポーネント更新
		for (auto& ptr : GetGameObjectVec()) {
			if (ptr->IsUpdateActive()) {
				ptr->ComponentUpdate();
			}
		}
		////衝突判定
		UpdateCollision();
		//配置オブジェクトの更新後処理
		for (auto& ptr : GetGameObjectVec()) {
			if (ptr->IsUpdateActive()) {
				ptr->OnUpdate2(Scene::GetElapsedTime());
			}
		}
		//自身の更新後処理
		if (IsUpdateActive()) {
			OnUpdate2(Scene::GetElapsedTime());
		}
		//カメラとライト
		m_camera->OnUpdate(Scene::GetElapsedTime());
		m_lightSet->OnUpdate(Scene::GetElapsedTime());
	}

	//衝突判定の更新（ステージから呼ぶ）
	//衝突判定をカスタマイズするためには
	//この関数を多重定義する
	void Stage::UpdateCollision() {
		//衝突判定管理者のUpdate(判定)
		m_collisionManager->OnUpdate(Scene::GetElapsedTime());
	}


	void Stage::OnPreCreate() {
		//コリジョン管理者の作成
		m_collisionManager = ObjectFactory::Create<CollisionManager>(GetThis<Stage>());
	}

	void Stage::OnShadowDraw() {
		for (auto& v : m_gameObjectVec) {
			v->OnShadowDraw();
		}
	}
	void Stage::OnSceneDraw() {
		std::vector<std::shared_ptr<GameObject>> alphaVec;
		std::vector<std::shared_ptr<GameObject>> normalVec;
		for (auto& v : m_gameObjectVec) {
			if (v->IsAlphaActive()) {
				alphaVec.push_back(v);
			}
			else {
				normalVec.push_back(v);
			}
		}
		//カメラの位置
		Vec3 CameraEye = GetCamera()->GetEye();
		//透明の3Dオブジェクトをカメラからの距離でソート
		//以下は、オブジェクトを引数に取りboolを返すラムダ式
		//--------------------------------------------------------
		auto func = [&](std::shared_ptr<GameObject>& Left, std::shared_ptr<GameObject>& Right)->bool {
			auto PtrLeftTrans = Left->GetComponent<Transform>();
			auto PtrRightTrans = Right->GetComponent<Transform>();

			auto LeftPos = PtrLeftTrans->GetWorldMatrix().transInMatrix();
			auto RightPos = PtrRightTrans->GetWorldMatrix().transInMatrix();

			auto LeftLen = bsmUtil::length(LeftPos - CameraEye);
			auto RightLen = bsmUtil::length(RightPos - CameraEye);

			return (LeftLen > RightLen);
			};
		std::sort(alphaVec.begin(), alphaVec.end(), func);
		for (auto& v : normalVec) {
			v->OnSceneDraw();
		}
		for (auto& v : alphaVec) {
			v->OnSceneDraw();
		}
	}

	void Stage::OnDestroy() {
		for (auto& v : m_gameObjectVec) {
			v->OnDestroy();
		}
	}

	void Stage::SetToBefore() {
		for (auto& v : m_gameObjectVec) {
			v->SetToBefore();
		}
	}


	void Stage::PushBackGameObject(const std::shared_ptr<GameObject>& Ptr) {
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
	void Stage::RemoveBackGameObject(const std::shared_ptr<GameObject>& Ptr) {
		m_waitRemoveObjectVec.push_back(Ptr);
	}

	void Stage::RemoveTargetGameObject(const std::shared_ptr<GameObject>& targetobj) {
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
	std::shared_ptr<GameObject> Stage::GetSharedGameObject(const std::wstring & key, bool exceptionActive)const {
		std::map<const std::wstring, std::weak_ptr<GameObject> >::const_iterator it;
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
						std::wstring keyerr = key;
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
					std::wstring keyerr = key;
					throw BaseException(
						L"オブジェクトが見つかりません",
						keyerr,
						L"Stage::GetSharedGameObject()"
					);
				}
			}
			return nullptr;
		}

	void Stage::SetSharedGameObject(const std::wstring& key, const std::shared_ptr<GameObject>& ptr) {
		std::map<const std::wstring, std::weak_ptr<GameObject> >::iterator it;
		//重複キーの検査
		it = m_SharedMap.find(key);
		if (it != m_SharedMap.end()) {
			//既に存在した
			//例外発生
			std::wstring keyerr = key;
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


	std::shared_ptr<GameObjectGroup> Stage::CreateSharedObjectGroup(const std::wstring& Key) {
		try {
			std::map<const std::wstring, std::shared_ptr<GameObjectGroup> >::iterator it;
			//重複キーの検査
			it = m_SharedGroupMap.find(Key);
			if (it != m_SharedGroupMap.end()) {
				//既に存在した
				//例外発生
				std::wstring keyerr = Key;
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

	std::shared_ptr<GameObjectGroup> Stage::GetSharedObjectGroup(const std::wstring& Key, bool ExceptionActive)const {
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
				std::wstring keyerr = Key;
				throw BaseException(
					L"指定のキーが見つかりません",
					keyerr,
					L"Stage::GetSharedObjectGroup() const"
				);
			}
		}
		return nullptr;
	}

	void Stage::SetSharedObjectGroup(const std::wstring& Key, const std::shared_ptr<GameObjectGroup>& NewPtr) {
		//重複キーの検査
		auto it = m_SharedGroupMap.find(Key);
		if (it != m_SharedGroupMap.end()) {
			//例外発生
			std::wstring keyerr = Key;
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


	void Stage::RemoveGameObject(const std::shared_ptr<GameObject>& Obj) {
		for (auto ptr : GetGameObjectVec()) {
			if (Obj == ptr) {
				RemoveBackGameObject(Obj);
				return;
			}
		}
		//存在しなくても何もしない
	}


	



}
// end namespace basecross
