/*!
@file Light.h
@brief ライトクラス
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once
#include "stdafx.h"


namespace basecross {

	//--------------------------------------------------------------------------------------
	//	ライト
	//--------------------------------------------------------------------------------------
	struct Light {
		Vec3 m_directional;	//ライトの向き
		Vec4 m_diffuseColor;	//ディフィーズ色
		Vec4 m_specularColor;	//スペキュラー色
		Vec3 m_position;	//位置（使用しない場合あり）
		Vec3 m_at;		//指している場所（使用しない場合あり）

		Light() :
			m_directional(0, -1.0f, 0),
			m_diffuseColor(1.0f, 1.0f, 1.0f, 1.0f),
			m_specularColor(0.2f, 0.2f, 0.2f, 1.0f),
			m_position(0.0f),
			m_at(0.0f)
		{
		}
		Light(const Vec3& dir, const Vec4& def, const Vec4& sp) :
			m_directional(dir),
			m_diffuseColor(def),
			m_specularColor(sp),
			m_position(0.0f),
			m_at(0.0f)
		{
		}
		Light(const Light& other) :
			m_directional(other.m_directional),
			m_diffuseColor(other.m_diffuseColor),
			m_specularColor(other.m_specularColor),
			m_position(other.m_position),
			m_at(other.m_at)
		{
		}
		Light& operator=(const Light& other) {
			if (this != &other) {
				m_directional = other.m_directional;
				m_diffuseColor = other.m_diffuseColor;
				m_specularColor = other.m_specularColor;
				m_position = other.m_position;
				m_at = other.m_at;
			}
			return *this;
		}
		~Light() {}
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
	class LightSet {
		const size_t m_maxLights = 3;
		vector<Light> m_lights;
		Vec4 m_ambient;
		size_t m_mainIndex;
		LightSet() {}
	public:
		virtual ~LightSet() {}
		const Light& GetLight(size_t index) const;
		size_t GetNumLights() const {
			return m_lights.size();
		}
		void SetLight(size_t index,const Light& light);
		void AddLight(const Light& light);
		Vec4 GetAmbient() const { return m_ambient; }
		void SetAmbient(const Vec4& a) { m_ambient = a; }
		size_t GetMainIndex() const {return m_mainIndex;}
		void SetMainIndex(size_t index) { m_mainIndex = index; }

		static shared_ptr<LightSet>
		CreateDefaultLights();
	};



}
// end basecross
