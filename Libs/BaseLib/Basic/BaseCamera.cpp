/*!
@file BaseCamera.cpp
@brief カメラクラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	カメラクラス（親）
	//--------------------------------------------------------------------------------------

	BaseCamera::BaseCamera() :
		ObjectInterface(),
		m_eye(0, 0.0f, -20.0f),	//デフォルトは後方
		m_at(0, 0, 0),
		m_up(0, 1.0f, 0),
		m_near(0.5f),
		m_far(1000.0f),
		m_RH(false)
	{
	}

	bool BaseCamera::GetRH() const {
		return m_RH;
	}
	void BaseCamera::SetRH(bool b) {
		m_RH = b;
	}
	const bsm::Vec3& BaseCamera::GetEye() const { return m_eye; }
	void BaseCamera::SetEye(const bsm::Vec3& eye) {
		m_eye = eye;
	}
	void BaseCamera::SetEye(float x, float y, float z) {
		m_eye = bsm::Vec3(x, y, z);
	}

	const bsm::Vec3& BaseCamera::GetAt() const { return m_at; }
	void BaseCamera::SetAt(const bsm::Vec3& at) {
		m_at = at;
	}
	void BaseCamera::SetAt(float x, float y, float z) {
		m_at = bsm::Vec3(x, y, z);
	}
	const bsm::Vec3& BaseCamera::GetUp() const { return m_up; }
	void BaseCamera::SetUp(const bsm::Vec3& Up) {
		m_up = Up;
	}
	void BaseCamera::SetUp(float x, float y, float z) {
		m_up = bsm::Vec3(x, y, z);
	}
	float BaseCamera::GetNear() const { return m_near; }
	void BaseCamera::SetNear(float f) {
		m_near = f;
	}

	float BaseCamera::GetFar() const { return m_far; }
	void BaseCamera::SetFar(float f) {
		m_far = f;
	}

	const bsm::Mat4x4& BaseCamera::GetViewMatrix(){
		CalculateMatrix();
		return m_viewMatrix; 
	}
	const bsm::Mat4x4& BaseCamera::GetProjMatrix(){
		CalculateMatrix();
		return m_projMatrix;
	}

	void BaseCamera::OnCreate() {
		CalculateMatrix();
	}

	void BaseCamera::OnUpdate() {
		CalculateMatrix();
	}

	//--------------------------------------------------------------------------------------
	//	遠近法カメラ
	//--------------------------------------------------------------------------------------
	PerspecCamera::PerspecCamera() :
		BaseCamera(),
		m_fovY(XM_PIDIV4)

	{
		try {
			auto width = (float)App::GetWidth();
			auto height = (float)App::GetHeight();
			m_aspect = width / height;
		}
		catch (...) {
			throw;
		}

	}

	void PerspecCamera::OnCreate() {
		auto width = (float)App::GetWidth();
		auto height = (float)App::GetHeight();
		m_aspect = width / height;
	}


	float PerspecCamera::GetFovY() const { return m_fovY; }
	void PerspecCamera::SetFovY(float f) {
		m_fovY = f;
	}

	float PerspecCamera::GetAspect() const { return m_aspect; }
	void PerspecCamera::SetAspect(float f) {
		m_aspect = f;
	}

	void PerspecCamera::CalculateMatrix() {
		if (GetRH()) {
			m_viewMatrix = XMMatrixLookAtRH(GetEye(), GetAt(), GetUp());
			m_projMatrix = XMMatrixPerspectiveFovRH(m_fovY, m_aspect, GetNear(), GetFar());
		}
		else {
			m_viewMatrix = XMMatrixLookAtLH(GetEye(), GetAt(), GetUp());
			m_projMatrix = XMMatrixPerspectiveFovLH(m_fovY, m_aspect, GetNear(), GetFar());
		}
	}

	//--------------------------------------------------------------------------------------
	//	正射影カメラ
	//--------------------------------------------------------------------------------------
	OrthoCamera::OrthoCamera() :
		BaseCamera(),
		m_width(0),
		m_height(0)
	{
		try {
			m_width = (float)App::GetWidth();
			m_height = (float)App::GetHeight();
		}
		catch (...) {
			throw;
		}

	}

	void OrthoCamera::OnCreate() {
		m_width = (float)App::GetWidth();
		m_height = (float)App::GetHeight();
	}


	float OrthoCamera::GetWidth() const { return m_width; }
	void OrthoCamera::SetWidth(float f) {
		m_width = f;
	}

	float OrthoCamera::GetHeight() const { return m_height; }
	void OrthoCamera::SetHeight(float f) {
		m_height = f;
	}

	void OrthoCamera::CalculateMatrix() {
		if (GetRH()) {
			m_viewMatrix = XMMatrixLookAtRH(GetEye(), GetAt(), GetUp());
			m_projMatrix = XMMatrixOrthographicRH(m_width, m_height, GetNear(), GetFar());
		}
		else {
			m_viewMatrix = XMMatrixLookAtLH(GetEye(), GetAt(), GetUp());
			m_projMatrix = XMMatrixOrthographicLH(m_width, m_height, GetNear(), GetFar());
		}
	}

}
// end namespace basecross
