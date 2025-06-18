/*!
@file BaseCamera.h
@brief カメラクラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	class Camera;
	//--------------------------------------------------------------------------------------
	class Camera : public ObjectInterface {
	protected:
		bsm::Vec3 m_eye;	//カメラ位置　
		bsm::Vec3 m_at;	//注目点
		bsm::Vec3 m_up;   //カメラの傾き（アップ）

		float m_near;	//手前の最小距離
		float m_far;	//奥の最大距離
		bsm::Mat4x4 m_viewMatrix;
		bsm::Mat4x4 m_projMatrix;
		Camera();
		virtual ~Camera() {}
	public:
		//アクセサ
		virtual const bsm::Vec3& GetEye() const;
		virtual void SetEye(const bsm::Vec3& eye);
		virtual void SetEye(float x, float y, float z);
		virtual const bsm::Vec3& GetAt() const;
		virtual void SetAt(const bsm::Vec3& at);
		virtual void SetAt(float x, float y, float z);
		virtual const bsm::Vec3& GetUp() const;
		virtual void SetUp(const bsm::Vec3& up);
		virtual void SetUp(float x, float y, float z);
		virtual float GetNear() const;
		virtual void SetNear(float f);
		virtual float GetFar() const;
		virtual void SetFar(float f);
		virtual void CalculateMatrix() = 0;
		virtual const bsm::Mat4x4& GetViewMatrix() const;
		virtual const bsm::Mat4x4& GetProjMatrix() const;
		virtual void OnCreate();
		virtual void OnUpdate(double elapsedTime);

	};

	//--------------------------------------------------------------------------------------
	//	class PerspecCamera;
	//--------------------------------------------------------------------------------------
	class PerspecCamera : public Camera {
		float m_fovY;   //射影角度
		float m_aspect;	//アスペクト比
	public:
		PerspecCamera();
		virtual ~PerspecCamera() {}
		virtual float GetFovY() const;
		virtual void SetFovY(float f);
		virtual float GetAspect() const;
		virtual void SetAspect(float f);
		virtual void CalculateMatrix()override;
	};

	//--------------------------------------------------------------------------------------
	//	class OrthoCamera;
	//--------------------------------------------------------------------------------------
	class OrthoCamera : public Camera {
		float m_width;	//遠近法を使わない場合（正射影）の幅
		float m_height;	//遠近法を使わない場合（正射影）の高さ
	public:
		OrthoCamera();
		virtual ~OrthoCamera() {}
		virtual float GetWidth() const;
		virtual void SetWidth(float f);
		virtual float GetHeight() const;
		virtual void SetHeight(float f);
		virtual void CalculateMatrix()override;
	};



}
// end namespace basecross
