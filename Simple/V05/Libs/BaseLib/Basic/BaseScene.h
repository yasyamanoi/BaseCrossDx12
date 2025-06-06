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
	///	シーン特有のイベント構造体
	//--------------------------------------------------------------------------------------
	struct SceneEvent {
		///	遅延時間（SendEventの場合は常に0）
		float m_dispatchTime;
		///	メッセージ文字列
		wstring m_msgStr;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		@param[in]	dispatchTime	配送までの時間
		@param[in]	msgStr	メッセージ文字列
		*/
		//--------------------------------------------------------------------------------------
		SceneEvent(float dispatchTime, const wstring& msgStr) :
			m_dispatchTime(dispatchTime),
			m_msgStr(msgStr)
		{}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		~SceneEvent() {}
	};


	//--------------------------------------------------------------------------------------
	///	シーン親クラス
	//--------------------------------------------------------------------------------------
	class BaseScene {
		map<wstring, shared_ptr<BaseMesh>> m_meshMap;
		map<wstring, shared_ptr<BaseTexture>> m_textureMap;
		shared_ptr<Stage> m_activeStage;
		vector<shared_ptr<SceneEvent>> m_eventVec;
		void CreateDefaultMeshes();
		void ConvertVertex(const vector<VertexPositionNormalTexture>& vertices,
			vector<VertexPositionColor>& new_pc_vertices,
			vector<VertexPositionNormal>& new_pn_vertices,
			vector<VertexPositionTexture>& new_pt_vertices,
			vector<VertexPositionNormalTangentTexture>& new_pntnt_vertices
		);
	protected:
		BaseScene():
			m_eventVec(),
			m_activeStage(nullptr)
		{}
		virtual ~BaseScene() {}
		void SetActiveStage(const shared_ptr<Stage>& stage) {
			m_activeStage = stage;
		}
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	アクティブなステージを設定して初期化する
		@tparam	T	取得する型（Stageに型変換できるもの）
		@tparam	Ts	可変長変数の型
		@param[in]	params	このステージを構築するのに使用するパラメータ。
		@return	アクティブなステージ
		*/
		//--------------------------------------------------------------------------------------
		template<typename T, typename... Ts>
		shared_ptr<T> ResetActiveStage(Ts&&... params) {
			auto ActStagePtr = GetActiveStage(false);
			if (ActStagePtr) {
				//破棄を伝える
				ActStagePtr->OnDestroy();
				ActStagePtr = nullptr;
				App::GetBaseDevice()->SetStageReCreated(true);
			}
			auto Ptr = ObjectFactory::Create<T>(params...);
			auto StagePtr = dynamic_pointer_cast<Stage>(Ptr);
			if (!StagePtr) {
				throw BaseException(
					L"以下はStageに型キャストできません。",
					Util::GetWSTypeName<T>(),
					L"BaseScene::ResetActiveStage<T>()"
				);
			}
			SetActiveStage(StagePtr);
			return Ptr;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	アクティブなステージを設定して初期化する
		@tparam	T	取得する型（Stageに型変換できるもの）
		@tparam	Ts	可変長変数の型
		@param[in]	params	このステージを構築するのに使用するパラメータ。
		@return	アクティブなステージ
		*/
		//--------------------------------------------------------------------------------------
		template<typename T, typename... Ts>
		shared_ptr<T> ResetActiveStageWithParam(Ts&&... params) {
			auto ActStagePtr = GetActiveStage(false);
			if (ActStagePtr) {
				//破棄を伝える
				ActStagePtr->DestroyStage();
			}
			auto Ptr = ObjectFactory::CreateWithParam<T>(params...);
			auto StagePtr = dynamic_pointer_cast<Stage>(Ptr);
			if (!StagePtr) {
				throw BaseException(
					L"以下はStageに型キャストできません。",
					Util::GetWSTypeName<T>(),
					L"SceneBase::ResetActiveStageWithParam<T>()"
				);
			}
			SetActiveStage(StagePtr);
			//デバッグ用文字列
			auto dbgPtr = StagePtr->AddGameObject<DebugString>();
			StagePtr->SetSharedGameObject(L"DebugString", dbgPtr);
			return Ptr;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	アクティブなステージを取得する
		@tparam	Ts	可変長変数の型
		@param[in]	ExceptionActive	無効を認めるかどうか
		@return	アクティブなステージ
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<Stage> GetActiveStage(bool ExceptionActive = true) const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	メッシュをリソース登録する
		@param[in]	key	リソースのキー
		@param[in]	mesh	メッシュ
		@param[in]	keyCheck	キーの重複チェックするかどうか
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void RegisterMesh(const wstring& key, const shared_ptr<BaseMesh>& mesh, bool keyCheck = false);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	登録されているメッシュを取得する
		@param[in]	key	リソースのキー
		@return	メッシュ
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<BaseMesh> GetMesh(const wstring& key);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	テクスチャをリソース登録する
		@param[in]	key	リソースのキー
		@param[in]	texture	テクスチャ
		@param[in]	keyCheck	キーの重複チェックするかどうか
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void RegisterTexture(const wstring& key, const shared_ptr<BaseTexture>& texture, bool keyCheck = false);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	登録されているテクスチャを取得する
		@param[in]	key	リソースのキー
		@return	テクスチャ
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<BaseTexture> GetTexture(const wstring& key);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	シーンへのイベントのPOST（キューに入れる）
		@param[in]	dispatchTime	遅延時間
		@param[in]	msgStr	メッセージ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void PostSceneEvent(float dispatchTime, const wstring& msgStr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	イベントを受け取る
		@param[in]	sceneEvent	シーンイベント
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnEvent(const shared_ptr<SceneEvent>& sceneEvent) = 0;

		void DispatchDelayedEvent();


		virtual void OnPreCreate();
		virtual void OnCreate() = 0;
		virtual void OnInitFrame(BaseFrame* pBaseFrame);
		virtual void WriteConstantBuffers(BaseFrame* pBaseFrame);

		virtual void OnUpdate();
		virtual void OnDraw() {}
		virtual void OnDestroy();
		virtual void OnKeyDown(UINT8 key);
		virtual void OnKeyUp(UINT8 key);

		virtual void PopulateShadowmapCommandList(BaseFrame* pBaseFrame);
		virtual void PopulateCommandList(BaseFrame* pBaseFrame);

	};
}
// end namespace basecross
