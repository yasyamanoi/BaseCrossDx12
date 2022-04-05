/*!
@file BaseScene.h
@brief シーンの親クラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once

#include "stdafx.h"

namespace basecross {


	class Stage;
	//--------------------------------------------------------------------------------------
	///	シーン親クラス
	//--------------------------------------------------------------------------------------
	class BaseScene {
		map<wstring, shared_ptr<BaseMesh>> m_meshMap;
		map<wstring, shared_ptr<BaseTexture>> m_textureMap;
		shared_ptr<Stage> m_activeStage;
		void CreateDefaultMeshes();
		void ConvertVertex(const vector<VertexPositionNormalTexture>& vertices,
			vector<VertexPositionColor>& new_pc_vertices,
			vector<VertexPositionNormal>& new_pn_vertices,
			vector<VertexPositionTexture>& new_pt_vertices,
			vector<VertexPositionNormalTangentTexture>& new_pntnt_vertices
		);
	protected:
		BaseScene():m_activeStage(nullptr){}
		virtual ~BaseScene() {}
		void SetActiveStage(const shared_ptr<Stage>& stage) {
			m_activeStage = stage;
		}
		//アクティブステージの設定
		template<typename T, typename... Ts>
		shared_ptr<T> ResetActiveStage(Ts&&... params) {
			auto ActStagePtr = GetActiveStage(false);
			if (ActStagePtr) {
				//破棄を伝える
				ActStagePtr->OnDestroy();
			}
			auto Ptr = ObjectFactory::Create<T>(params...);
			auto StagePtr = dynamic_pointer_cast<Stage>(Ptr);
			if (!StagePtr) {
				throw BaseException(
					L"以下はStageに型キャストできません。",
					Util::GetWSTypeName<T>(),
					L"SceneEx::ResetActiveStage<T>()"
				);
			}
			SetActiveStage(StagePtr);
			return Ptr;
		}

	public:
		shared_ptr<Stage> GetActiveStage(bool ExceptionActive = true) const {
			if (!m_activeStage) {
				//アクティブなステージが無効なら
				if (ExceptionActive) {
					throw BaseException(
						L"アクティブなステージがありません",
						L"if(!m_activeStage)",
						L"SceneEx::GetActiveStage()"
					);
				}
				else {
					return nullptr;
				}
			}
			return m_activeStage;
		}

		void AddMesh(const wstring& key, const shared_ptr<BaseMesh>& mesh, bool keyCheck = false) {
			if (keyCheck) {
				auto it = m_meshMap.find(key);
				if (it != m_meshMap.end()) {
					throw BaseException(
						L"指定のキーのメッシュがすでに存在します",
						key,
						L"BaseScene::AddMesh()"
					);
				}
			}
			m_meshMap[key] = mesh;
		}
		shared_ptr<BaseMesh> GetMesh(const wstring& key) {
			auto it = m_meshMap.find(key);
			if (it != m_meshMap.end()) {
				return it->second;
			}
			else {
				throw BaseException(
					L"指定のキーのメッシュが見つかりません",
					key,
					L"BaseScene::GetMesh()"
				);
			}
			return nullptr;
		}
		void AddTexture(const wstring& key, const shared_ptr<BaseTexture>& texture, bool keyCheck = false) {
			if (keyCheck) {
				auto it = m_textureMap.find(key);
				if (it != m_textureMap.end()) {
					throw BaseException(
						L"指定のキーのテクスチャがすでに存在します",
						key,
						L"BaseScene::AddTexture()"
					);
				}
			}
			m_textureMap[key] = texture;
		}
		shared_ptr<BaseTexture> GetTexture(const wstring& key) {
			auto it = m_textureMap.find(key);
			if (it != m_textureMap.end()) {
				return it->second;
			}
			else {
				throw BaseException(
					L"指定のキーのテクスチャが見つかりません",
					key,
					L"BaseScene::GetTexture()"
				);
			}
			return nullptr;
		}
		shared_ptr<BaseTexture> CreateTextureFlomFile(const wstring& falsename);

		virtual void OnInitScene();
		virtual void OnInit() {}
		virtual void OnInitFrame(BaseFrame* pBaseFrame);
		virtual void WriteConstantBuffers(BaseFrame* pBaseFrame);
		virtual void OnUpdate();
		virtual void OnDestroy();
		virtual void OnKeyDown(UINT8 key);
		virtual void OnKeyUp(UINT8 key);

		virtual void PopulateCommandList(BaseFrame* pBaseFrame);

	};
}
// end namespace basecross
