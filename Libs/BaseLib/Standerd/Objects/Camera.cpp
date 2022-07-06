/*!
@file Camera.cpp
@brief カメラクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	class Camera;
	//--------------------------------------------------------------------------------------

	Camera::Camera() :
		ObjectInterface(),
		m_eye(0, 0.0f, -20.0f),	//デフォルトは後方
		m_at(0, 0, 0),
		m_up(0, 1.0f, 0),
		m_near(0.5f),
		m_far(1000.0f),
		m_viewMatrix(),
		m_projMatrix()
	{
	}

	//アクセサ
	const bsm::Vec3& Camera::GetEye() const { return m_eye; }
	void Camera::SetEye(const bsm::Vec3& eye) {
		m_eye = eye;
		CalculateMatrix();
	}
	void Camera::SetEye(float x, float y, float z) {
		m_eye = bsm::Vec3(x, y, z);
		CalculateMatrix();
	}

	const bsm::Vec3& Camera::GetAt() const { return m_at; }
	void Camera::SetAt(const bsm::Vec3& at) {
		m_at = at;
		CalculateMatrix();
	}
	void Camera::SetAt(float x, float y, float z) {
		m_at = bsm::Vec3(x, y, z);
		CalculateMatrix();
	}

	const bsm::Vec3& Camera::GetUp() const { return m_up; }
	void Camera::SetUp(const bsm::Vec3& Up) {
		m_up = Up;
		CalculateMatrix();
	}
	void Camera::SetUp(float x, float y, float z) {
		m_up = bsm::Vec3(x, y, z);
		CalculateMatrix();
	}

	float Camera::GetNear() const { return m_near; }
	void Camera::SetNear(float f) {
		m_near = f;
		CalculateMatrix();
	}

	float Camera::GetFar() const { return m_far; }
	void Camera::SetFar(float f) {
		m_far = f;
		CalculateMatrix();
	}

	const bsm::Mat4x4& Camera::GetViewMatrix() const { return m_viewMatrix; }
	const bsm::Mat4x4& Camera::GetProjMatrix() const { return m_projMatrix; }

	void Camera::OnCreate() {
		CalculateMatrix();
	}

	void Camera::OnUpdate() {
		CalculateMatrix();
	}

	//--------------------------------------------------------------------------------------
	//	class PerspecCamera;
	//--------------------------------------------------------------------------------------
	PerspecCamera::PerspecCamera() :
		Camera(),
		m_fovY(XM_PIDIV4)

	{
		try {
			auto width = (float)App::GetBaseDevice()->GetWidth();
			auto height = (float)App::GetBaseDevice()->GetHeight();
			m_aspect = width / height;
		}
		catch (...) {
			throw;
		}

	}

	float PerspecCamera::GetFovY() const { return m_fovY; }
	void PerspecCamera::SetFovY(float f) {
		m_fovY = f;
		CalculateMatrix();
	}

	float PerspecCamera::GetAspect() const { return m_aspect; }
	void PerspecCamera::SetAspect(float f) {
		m_aspect = f;
		CalculateMatrix();
	}

	void PerspecCamera::CalculateMatrix() {
		m_viewMatrix = XMMatrixLookAtLH(GetEye(), GetAt(), GetUp());
		m_projMatrix = XMMatrixPerspectiveFovLH(m_fovY, m_aspect, GetNear(), GetFar());
	}

	//--------------------------------------------------------------------------------------
	//	class OrthoCamera;
	//--------------------------------------------------------------------------------------
	OrthoCamera::OrthoCamera() :
		Camera(),
		m_width(0),
		m_height(0)
	{
		try {
			m_width = (float)App::GetBaseDevice()->GetWidth();
			m_height = (float)App::GetBaseDevice()->GetHeight();
		}
		catch (...) {
			throw;
		}

	}



	float OrthoCamera::GetWidth() const { return m_width; }
	void OrthoCamera::SetWidth(float f) {
		m_width = f;
		CalculateMatrix();
	}

	float OrthoCamera::GetHeight() const { return m_height; }
	void OrthoCamera::SetHeight(float f) {
		m_height = f;
		CalculateMatrix();
	}

	void OrthoCamera::CalculateMatrix() {
		m_viewMatrix = XMMatrixLookAtLH(GetEye(), GetAt(), GetUp());
		m_projMatrix = XMMatrixOrthographicLH(m_width, m_height, GetNear(), GetFar());
	}

}
// end namespace basecross
