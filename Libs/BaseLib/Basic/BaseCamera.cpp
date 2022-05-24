/*!
@file BaseCamera.cpp
@brief カメラクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#include "stdafx.h"

namespace basecross {

	BaseCamera::BaseCamera():
		m_eye(0, 0.0f, -20.0f),	//デフォルトは後方
		m_at(0, 0, 0),
		m_up(0, 1.0f, 0),
		m_pers(true),
		m_fovY(XM_PIDIV4),
		m_aspect(0),
		m_width(0),
		m_height(0),
		m_near(0.5f),
		m_far(1000.0f),
		m_viewMatrix(),
		m_projMatrix()
	{
		try {
			m_width = (float)App::GetBaseDevice()->GetWidth();
			m_height = (float)App::GetBaseDevice()->GetHeight();
			m_aspect = m_width / m_height;
		}
		catch (...) {
			throw;
		}

	}

	//アクセサ
	const bsm::Vec3& BaseCamera::GetEye() const { return m_eye; }
	void BaseCamera::SetEye(const bsm::Vec3& eye) {
		m_eye = eye;
		CalculateMatrix();
	}
	void BaseCamera::SetEye(float x, float y, float z) {
		m_eye = bsm::Vec3(x, y, z);
		CalculateMatrix();
	}

	const bsm::Vec3& BaseCamera::GetAt() const { return m_at; }
	void BaseCamera::SetAt(const bsm::Vec3& at) {
		m_at = at;
		CalculateMatrix();
	}
	void BaseCamera::SetAt(float x, float y, float z) {
		m_at = bsm::Vec3(x, y, z);
		CalculateMatrix();
}

	const bsm::Vec3& BaseCamera::GetUp() const { return m_up; }
	void BaseCamera::SetUp(const bsm::Vec3& Up) {
		m_up = Up;
		CalculateMatrix();
	}
	void BaseCamera::SetUp(float x, float y, float z) {
		m_up = bsm::Vec3(x, y, z);
		CalculateMatrix();
	}

	bool BaseCamera::IsPers()const { return m_pers; }
	bool BaseCamera::GetPers()const { return m_pers; }
	void BaseCamera::SetPers(bool b) {
		m_pers = b;
		CalculateMatrix();
	}

	float BaseCamera::GetFovY() const { return m_fovY; }
	void BaseCamera::SetFovY(float f) {
		m_fovY = f;
		CalculateMatrix();
	}

	float BaseCamera::GetAspect() const { return m_aspect; }
	void BaseCamera::SetAspect(float f) {
		m_aspect = f;
		CalculateMatrix();
	}

	float BaseCamera::GetWidth() const { return m_width; }
	void BaseCamera::SetWidth(float f) {
		m_width = f;
		CalculateMatrix();
	}

	float BaseCamera::GetHeight() const { return m_height; }
	void BaseCamera::SetHeight(float f) {
		m_height = f;
		CalculateMatrix();
	}

	float BaseCamera::GetNear() const { return m_near; }
	void BaseCamera::SetNear(float f) {
		m_near = f;
		CalculateMatrix();
	}

	float BaseCamera::GetFar() const { return m_far; }
	void BaseCamera::SetFar(float f) {
		m_far = f;
		CalculateMatrix();
	}

	shared_ptr<GameObject> BaseCamera::GetCameraObject() const {
		if (auto sh = m_cameraObject.lock()) {
			return sh;
		}
		else {
			return nullptr;
		}
	}
	void BaseCamera::SetCameraObject(const shared_ptr<GameObject>& obj) {
		if (!obj) {
			m_cameraObject.reset();
		}
		else {
			m_cameraObject = obj;
		}
		CalculateMatrix();
	}
	void BaseCamera::ClearCameraObject() {
		SetCameraObject(nullptr);
	}

	void BaseCamera::CalculateMatrix() {
		if (auto ShPtr = m_cameraObject.lock()) {
			auto TransPtr = ShPtr->GetComponent<Transform>();
			if (TransPtr) {
				m_eye = TransPtr->GetWorldPosition();
				m_viewMatrix = XMMatrixLookAtLH(m_eye, m_at, m_up);
				if (m_pers) {
					m_projMatrix = XMMatrixPerspectiveFovLH(m_fovY, m_aspect, m_near, m_far);
				}
				else {
					m_projMatrix = XMMatrixOrthographicLH(m_width, m_height, m_near, m_far);
				}
			}
		}
		else {
			if (m_pers) {
				m_viewMatrix = XMMatrixLookAtLH(m_eye, m_at, m_up);
				m_projMatrix = XMMatrixPerspectiveFovLH(m_fovY, m_aspect, m_near, m_far);
			}
			else {
				m_viewMatrix = XMMatrixLookAtLH(m_eye, m_at, m_up);
				m_projMatrix = XMMatrixOrthographicLH(m_width, m_height, m_near, m_far);
			}
		}
	}

	const bsm::Mat4x4& BaseCamera::GetViewMatrix() const { return m_viewMatrix; }
	const bsm::Mat4x4& BaseCamera::GetProjMatrix() const { return m_projMatrix; }

	void BaseCamera::OnCreate() {
		CalculateMatrix();
	}

	void BaseCamera::OnUpdate() {
		CalculateMatrix();
	}

}
// end namespace basecross

