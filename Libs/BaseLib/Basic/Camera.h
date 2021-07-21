/*!
@file Camera.h
@brief カメラクラス
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	//--------------------------------------------------------------------------------------
	///	カメラクラス
	//--------------------------------------------------------------------------------------
	class Camera
	{
	protected:
		Vec3 m_eye;
		Vec3 m_at;
		Vec3 m_up;
		float m_fovY;
		float m_aspectRatio;
		float m_nearPlane;
		float m_farPlane;
		Camera(const Vec3& eye, const Vec3& at);
		void ThrowIfNotSafe();
	public:
		virtual ~Camera() {}
		Mat4x4 GetViewMatrix();
		Mat4x4 GetProjectionMatrix();
		template <typename T>
		static shared_ptr<T>
			CreateCamera(const Vec3& eye, const Vec3& at) {
			shared_ptr<T> Ptr = shared_ptr<T>(new T(eye, at));
			auto chkPtr = dynamic_pointer_cast<Camera>(Ptr);
			if (!chkPtr) {
				throw BaseException(
					L"指定のクラスはカメラに変換できません",
					L"Camera::CreateCamera()"
				);
			}
			Ptr->ThrowIfNotSafe();
			return Ptr;
		}
		Vec3 GetEye() const { return m_eye; }
		void SetEye(const Vec3& eye) {
			m_eye = eye;
			ThrowIfNotSafe();
		}
		Vec3 GetAt() const { return m_at; }
		void SetAt(const Vec3& at) {
			m_at = at;
			ThrowIfNotSafe();
		}
		Vec3 GetUpDirection() const { return m_up; }
		void SetUp(const Vec3& up) {
			m_up = up;
			m_up.normalize();
		}
		float GetFovY()const { return m_fovY; }
		void SetFovY(float fov) { m_fovY = fov; }
		float GetAspectRatio()const { return m_aspectRatio; }
		void SetAspectRatio(float ratio) { m_aspectRatio = ratio; }
		float GetNearPlane()const { return m_nearPlane; }
		void SetNearPlane(float nrar) { m_nearPlane = nrar; }
		float GetFarPlane()const { return m_farPlane; }
		void SetFarPlane(float f) { m_farPlane = f; }
		virtual void OnUpdate() {}
	};

}
// end namespace basecross


