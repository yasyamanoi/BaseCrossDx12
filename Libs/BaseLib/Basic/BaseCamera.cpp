/*!
@file BaseCamera.cpp
@brief カメラクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#include "stdafx.h"

namespace basecross {

	BaseCamera::BaseCamera():
		m_Eye(0, 0.0f, -20.0f),	//デフォルトは後方
		m_At(0, 0, 0),
		m_Up(0, 1.0f, 0),
		m_Pers(true),
		m_FovY(XM_PIDIV4),
		m_Aspect(0),
		m_Width(0),
		m_Height(0),
		m_Near(0.5f),
		m_Far(1000.0f),
		m_ViewMatrix(),
		m_ProjMatrix()
	{
		try {
			m_Width = (float)App::GetBaseDevice()->GetWidth();
			m_Height = (float)App::GetBaseDevice()->GetHeight();
			m_Aspect = m_Width / m_Height;
		}
		catch (...) {
			throw;
		}

	}

	//アクセサ
	const bsm::Vec3& BaseCamera::GetEye() const { return m_Eye; }
	void BaseCamera::SetEye(const bsm::Vec3& Eye) {
		m_Eye = Eye;
		CalculateMatrix();
	}
	void BaseCamera::SetEye(float x, float y, float z) {
		m_Eye = bsm::Vec3(x, y, z);
		CalculateMatrix();
	}

	const bsm::Vec3& BaseCamera::GetAt() const { return m_At; }
	void BaseCamera::SetAt(const bsm::Vec3& At) {
		m_At = At;
		CalculateMatrix();
	}
	void BaseCamera::SetAt(float x, float y, float z) {
		m_At = bsm::Vec3(x, y, z);
		CalculateMatrix();
}

	const bsm::Vec3& BaseCamera::GetUp() const { return m_Up; }
	void BaseCamera::SetUp(const bsm::Vec3& Up) {
		m_Up = Up;
		CalculateMatrix();
	}
	void BaseCamera::SetUp(float x, float y, float z) {
		m_Up = bsm::Vec3(x, y, z);
		CalculateMatrix();
	}

	bool BaseCamera::IsPers()const { return m_Pers; }
	bool BaseCamera::GetPers()const { return m_Pers; }
	void BaseCamera::SetPers(bool b) {
		m_Pers = b;
		CalculateMatrix();
	}

	float BaseCamera::GetFovY() const { return m_FovY; }
	void BaseCamera::SetFovY(float f) {
		m_FovY = f;
		CalculateMatrix();
	}

	float BaseCamera::GetAspect() const { return m_Aspect; }
	void BaseCamera::SetAspect(float f) {
		m_Aspect = f;
		CalculateMatrix();
	}

	float BaseCamera::GetWidth() const { return m_Width; }
	void BaseCamera::SetWidth(float f) {
		m_Width = f;
		CalculateMatrix();
	}

	float BaseCamera::GetHeight() const { return m_Height; }
	void BaseCamera::SetHeight(float f) {
		m_Height = f;
		CalculateMatrix();
	}

	float BaseCamera::GetNear() const { return m_Near; }
	void BaseCamera::SetNear(float f) {
		m_Near = f;
		CalculateMatrix();
	}

	float BaseCamera::GetFar() const { return m_Far; }
	void BaseCamera::SetFar(float f) {
		m_Far = f;
		CalculateMatrix();
	}

	shared_ptr<GameObject> BaseCamera::GetCameraObject() const {
		if (auto sh = m_CameraObject.lock()) {
			return sh;
		}
		else {
			return nullptr;
		}
	}
	void BaseCamera::SetCameraObject(const shared_ptr<GameObject>& Obj) {
		if (!Obj) {
			m_CameraObject.reset();
		}
		else {
			m_CameraObject = Obj;
		}
		CalculateMatrix();
	}
	void BaseCamera::ClearCameraObject() {
		SetCameraObject(nullptr);
	}

	void BaseCamera::CalculateMatrix() {
		if (auto ShPtr = m_CameraObject.lock()) {
			auto TransPtr = ShPtr->GetComponent<Transform>();
			if (TransPtr) {
				m_Eye = TransPtr->GetWorldPosition();
				m_ViewMatrix = XMMatrixLookAtLH(m_Eye, m_At, m_Up);
				if (m_Pers) {
					m_ProjMatrix = XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_Near, m_Far);
				}
				else {
					m_ProjMatrix = XMMatrixOrthographicLH(m_Width, m_Height, m_Near, m_Far);
				}
			}
		}
		else {
			if (m_Pers) {
				m_ViewMatrix = XMMatrixLookAtLH(m_Eye, m_At, m_Up);
				m_ProjMatrix = XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_Near, m_Far);
			}
			else {
				m_ViewMatrix = XMMatrixLookAtLH(m_Eye, m_At, m_Up);
				m_ProjMatrix = XMMatrixOrthographicLH(m_Width, m_Height, m_Near, m_Far);
			}
		}
	}

	const bsm::Mat4x4& BaseCamera::GetViewMatrix() const { return m_ViewMatrix; }
	const bsm::Mat4x4& BaseCamera::GetProjMatrix() const { return m_ProjMatrix; }

	void BaseCamera::OnInit() {
		CalculateMatrix();
	}

	void BaseCamera::OnUpdate() {
		CalculateMatrix();
	}

#ifdef test

	void BaseCamera::ThrowIfNotSafe() {
		auto span = m_at - m_eye;
		if (span.length() < 0.001f) {
			throw BaseException(
				L"カメラ位置とカメラ視点の位置が近すぎます。",
				L"Camera::ThrowIfNotSafe()"
			);
		}
	}


	BaseCamera::BaseCamera(const Vec3& eye, const Vec3& at) :
		m_eye(eye),
		m_at(at),
		m_up(0, 1, 0),
		m_fovY(XM_PIDIV4),
		m_nearPlane(0.01f),
		m_farPlane(125.0f)
	{
		float w = (float)App::GetBaseDevice()->GetWidth();
		float h = (float)App::GetBaseDevice()->GetHeight();
		m_aspectRatio = w / h;
	}


	Mat4x4 BaseCamera::GetViewMatrix()
	{
		return (Mat4x4)XMMatrixLookAtLH(m_eye, m_at, m_up);
	}

	Mat4x4 BaseCamera::GetProjectionMatrix()
	{
		return (Mat4x4)XMMatrixPerspectiveFovLH(m_fovY, m_aspectRatio, m_nearPlane, m_farPlane);
	}

#endif

}
// end namespace basecross

