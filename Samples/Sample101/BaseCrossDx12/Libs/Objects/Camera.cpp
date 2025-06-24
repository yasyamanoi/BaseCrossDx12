/*!
@file BaseCamera.cpp
@brief カメラクラス
*/


#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	class Camera;
	//--------------------------------------------------------------------------------------

	Camera::Camera() :
		m_eye(0, 10.0f, -20.0f),	//デフォルトは後方
		m_at(0, 0, 0),
		m_up(0, 1.0f, 0),
		m_near(0.5f),
		m_far(1000.0f),
		m_viewMatrix(),
		m_projMatrix()
	{
	}

	//アクセサ
	const Vec3& Camera::GetEye() const { return m_eye; }
	void Camera::SetEye(const Vec3& eye) {
		m_eye = eye;
		CalculateMatrix();
	}
	void Camera::SetEye(float x, float y, float z) {
		m_eye = XMFLOAT3(x, y, z);
		CalculateMatrix();
	}

	const Vec3& Camera::GetAt() const { return m_at; }
	void Camera::SetAt(const Vec3& at) {
		m_at = at;
		CalculateMatrix();
	}
	void Camera::SetAt(float x, float y, float z) {
		m_at = XMFLOAT3(x, y, z);
		CalculateMatrix();
	}

	const Vec3& Camera::GetUp() const { return m_up; }
	void Camera::SetUp(const Vec3& Up) {
		m_up = Up;
		CalculateMatrix();
	}
	void Camera::SetUp(float x, float y, float z) {
		m_up = XMFLOAT3(x, y, z);
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

	const Mat4x4& Camera::GetViewMatrix() const { return m_viewMatrix; }
	const Mat4x4& Camera::GetProjMatrix() const { return m_projMatrix; }

	void Camera::OnCreate() {
		CalculateMatrix();
	}

	void Camera::OnUpdate(double elapsedTime) {
		CalculateMatrix();
	}

	//--------------------------------------------------------------------------------------
	//	class PerspecCamera;
	//--------------------------------------------------------------------------------------
	PerspecCamera::PerspecCamera() :
		Camera(),
		m_fovY(XM_PIDIV4)

	{
		CalculateMatrix();
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
		auto device = BaseDevice::GetBaseDevice();
		float width = (float)device->GetWidth();
		float height = (float)device->GetHeight();
		m_aspect = width / height;
		m_viewMatrix 
			= Mat4x4(XMMatrixLookAtLH(XMLoadFloat3(&m_eye), XMLoadFloat3(&m_at), XMLoadFloat3(&m_up)));
		m_projMatrix 
			= Mat4x4(XMMatrixPerspectiveFovLH(m_fovY, m_aspect, m_near, m_far));
	}

	//--------------------------------------------------------------------------------------
	//	class OrthoCamera;
	//--------------------------------------------------------------------------------------
	OrthoCamera::OrthoCamera() :
		Camera(),
		m_width(32.0f),
		m_height(24.0f)
	{
		CalculateMatrix();
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
		m_viewMatrix
			= Mat4x4(XMMatrixLookAtLH(XMLoadFloat3(&m_eye), XMLoadFloat3(&m_at), XMLoadFloat3(&m_up)));
		m_projMatrix
			= Mat4x4(XMMatrixOrthographicLH(m_width, m_height, m_near, m_far));
	}

}
// end namespace basecross
