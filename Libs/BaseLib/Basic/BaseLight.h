/*!
@file BaseLight.h
@brief ライトクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once
#include "stdafx.h"


namespace basecross {

	//--------------------------------------------------------------------------------------
	//	ライト
	//--------------------------------------------------------------------------------------
	struct BaseLight {
		Vec3 m_directional;	//ライトの向き
		Vec4 m_diffuseColor;	//ディフィーズ色
		Vec4 m_specularColor;	//スペキュラー色
		Vec3 m_position;	//位置（使用しない場合あり）
		Vec3 m_at;		//指している場所（使用しない場合あり）

		BaseLight() :
			m_directional(0, -1.0f, 0),
			m_diffuseColor(1.0f, 1.0f, 1.0f, 1.0f),
			m_specularColor(0.2f, 0.2f, 0.2f, 1.0f),
			m_position(0.0f),
			m_at(0.0f)
		{
		}
		BaseLight(const Vec3& dir, const Vec4& def, const Vec4& sp) :
			m_directional(dir),
			m_diffuseColor(def),
			m_specularColor(sp),
			m_position(0.0f),
			m_at(0.0f)
		{
		}
		BaseLight(const BaseLight& other) :
			m_directional(other.m_directional),
			m_diffuseColor(other.m_diffuseColor),
			m_specularColor(other.m_specularColor),
			m_position(other.m_position),
			m_at(other.m_at)
		{
		}
		BaseLight& operator=(const BaseLight& other) {
			if (this != &other) {
				m_directional = other.m_directional;
				m_diffuseColor = other.m_diffuseColor;
				m_specularColor = other.m_specularColor;
				m_position = other.m_position;
				m_at = other.m_at;
			}
			return *this;
		}
		~BaseLight() {}
		void SetPositionToDirectional(const Vec3& pos, const Vec3& at) {
			m_position = pos;
			m_at = at;
			Vec3 dir = at - pos;
			dir.normalize();
			m_directional = dir;
		}
	};

	//--------------------------------------------------------------------------------------
	//	ライトのセット
	//--------------------------------------------------------------------------------------
	class LightSet :public ObjectInterface {
		const size_t m_maxLights = 3;
		vector<BaseLight> m_lights;
		Vec4 m_ambient;
		size_t m_mainIndex;
	public:
		LightSet() :m_mainIndex(0) {}
		virtual ~LightSet() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief ライトの取得
		@param[in]	index	インデックス
		@return	指定のインデックスのライトを返す
		*/
		//--------------------------------------------------------------------------------------
		BaseLight GetLight(size_t index) const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief ライト数の取得
		@return	ライト数
		*/
		//--------------------------------------------------------------------------------------
		size_t GetNumLights() const {
			return m_lights.size();
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief ライトの設定
		@param[in]	index	インデックス
		@param[in]	light	設定するライト
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetLight(size_t index, const BaseLight& light);
		//--------------------------------------------------------------------------------------
		/*!
		@brief ライトの追加
		@param[in]	light	追加するライト
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void AddLight(const BaseLight& light);
		//--------------------------------------------------------------------------------------
		/*!
		@brief アンビエントの取得
		@return	アンビエント
		*/
		//--------------------------------------------------------------------------------------
		Vec4 GetAmbient() const { return m_ambient; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief アンビエントの設定
		@param[in]	a	設定するアンビエント
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetAmbient(const Vec4& a) { m_ambient = a; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief メインライトインデックスの取得
		@return	メインライトインデックス
		*/
		//--------------------------------------------------------------------------------------
		size_t GetMainIndex() const { return m_mainIndex; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief メインライトインデックスの設定
		@param[in]	index	設定するインデックス
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetMainIndex(size_t index) { m_mainIndex = index; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief メインライトの取得
		@return	メインライト
		*/
		//--------------------------------------------------------------------------------------
		BaseLight GetMainBaseLight() const {
			return GetLight(GetMainIndex());
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief 初期化を行う（デフォルトのライトを作成する）
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCreate()override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief 更新処理。デフォルトは何も行わない
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate()override{}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	描画処理。デフォルトは何も行わない
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnDraw()override {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	終了処理。デフォルトは何も行わない
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnDestroy()override {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief デフォルトのライティングをしたライトセットの取得
		@return	ライトセットのポインタ
		*/
		//--------------------------------------------------------------------------------------
		//static shared_ptr<LightSet>
		//	CreateDefaultLights();
	};



}
// end basecross
