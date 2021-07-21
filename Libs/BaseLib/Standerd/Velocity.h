/*!
@file Velocity.h
@brief 速度コンポーネント
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	速度コンポーネント
	//--------------------------------------------------------------------------------------
	class Velocity : public Component {
		//フォース
		Vec3 m_liniaFource;
		Vec3 m_liniaVelocity;
		Quat m_quatVelocity;
		//重力加速
		Vec3 m_gravity;
		void AddGravity();
	public:
		explicit Velocity(const shared_ptr<GameObject>& gameObjectPtr,
			const bsm::Vec3& gravity = bsm::Vec3(0, -9.8f, 0));
		virtual ~Velocity();
		void ClearLiniaFource() {
			m_liniaFource = Vec3(0.0f);
		}
		Vec3 GetLiniaFource()const {
			return m_liniaFource;
		}
		void SetLiniaFource(const Vec3& v) {
			m_liniaFource = v;
		}
		Vec3 GetLiniaVelocity()const {
			return m_liniaVelocity;
		}
		void SetLiniaVelocity(const Vec3& v) {
			m_liniaVelocity = v;
		}
		Quat GetQuatVelocity()const {
			return m_quatVelocity;
		}
		void SetQuatVelocity(const Quat& q) {
			m_quatVelocity = q;
		}


		//操作
		virtual void OnInit()override {}
		virtual void OnUpdate()override;
		virtual void OnRender()override {}
		virtual void OnDestroy()override {}
	};

}
// end basecross
