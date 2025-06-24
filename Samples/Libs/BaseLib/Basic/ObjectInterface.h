/*!
@file ObjectInterface.h
@brief GameObject等の親クラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once

#include "stdafx.h"

namespace basecross {


	class ObjectFactory;
	class ObjectInterface;



	//--------------------------------------------------------------------------------------
	///	Objectインターフェイス
	//--------------------------------------------------------------------------------------
	class ObjectInterface : public std::enable_shared_from_this<ObjectInterface> {
		friend class ObjectFactory;
	protected:
		ObjectInterface() {}
		virtual ~ObjectInterface() {}
	public:
		//クリエイト済みかどうか
		//Create関数が呼び出し後にtrueになる
		bool m_created{ false };
		void SetCreated(bool b) {
			m_created = b;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief thisポインタ（shared_ptr）の取得
		@tparam T	ポインタの型
		@return　オブジェクトのshared_ptr
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		std::shared_ptr<T> GetThis() {
			auto ptr = std::dynamic_pointer_cast<T>(shared_from_this());
			if (ptr) {
				return ptr;
			}
			else {
				std::wstring str(L"thisを");
				str += Util::GetWSTypeName<T>();
				str += L"型にキャストできません";
				throw BaseException(
					str,
					L"if( ! dynamic_pointer_cast<T>(shared_from_this()) )",
					L"ObjectInterface::GetThis()"
				);
			}
			return nullptr;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief クリエイト済みかを取得
		@return　クリエイト済みならtrue
		*/
		//--------------------------------------------------------------------------------------
		bool IsCreated()const {
			return m_created;
		}
		//仮想関数群
		//--------------------------------------------------------------------------------------
		/*!
		@brief	初期化前処理
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnPreCreate() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	初期化処理
		@param[in]	pCommandList コマンドリスト
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCreate() = 0;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	更新処理
		@param[in]	elapsedTime 前のターンからの時間
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate(double elapsedTime) = 0;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	更新処理２
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate2(double elapsedTime) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	描画処理
		@param[in]	pCommandList コマンドリスト
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnShadowDraw(){}
		virtual void OnSceneDraw() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	破棄時処理
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnDestroy(){}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	キーボード押された処理
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnKeyDown(UINT8 /*key*/) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	キーボード離された処理
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnKeyUp(UINT8 /*key*/) {}
	private:
		//コピー禁止
		ObjectInterface(const ObjectInterface&) = delete;
		ObjectInterface& operator=(const ObjectInterface&) = delete;
		//ムーブ禁止
		ObjectInterface(const ObjectInterface&&) = delete;
		ObjectInterface& operator=(const ObjectInterface&&) = delete;
	};


	//--------------------------------------------------------------------------------------
	///	Objectを構築する
	//--------------------------------------------------------------------------------------
	class ObjectFactory {
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief オブジェクト作成（static関数）
		@tparam T	作成する型
		@tparam Ts...	可変長パラメータ型
		@param[in]	params	可変長パラメータ
		@return　作成したオブジェクトのshared_ptr
		*/
		//--------------------------------------------------------------------------------------
		template<typename T, typename... Ts>
		static std::shared_ptr<T> Create(Ts&&... params) {
			std::shared_ptr<T> ptr = std::shared_ptr<T>(new T(params...));
			//初期化関数呼び出し
			ptr->OnPreCreate();
			ptr->OnCreate();
			ptr->SetCreated(true);
			return ptr;
		}


		//--------------------------------------------------------------------------------------
		/*!
		@brief オブジェクト作成（static関数）。パラメータはOnInitに渡される
		@tparam T	作成する型
		@tparam Ts...	可変長パラメータ型
		@param[in]	params	可変長パラメータ
		@return　作成したオブジェクトのshared_ptr
		*/
		//--------------------------------------------------------------------------------------
		template<typename T, typename... Ts>
		static std::shared_ptr<T> CreateInitParam(Ts&&... params) {
			std::shared_ptr<T> ptr = std::shared_ptr<T>(new T());
			//初期化関数呼び出し
			ptr->OnCreate(params...);
			ptr->SetCreated(true);
			return ptr;
		}

	};


}
// end namespace basecross
