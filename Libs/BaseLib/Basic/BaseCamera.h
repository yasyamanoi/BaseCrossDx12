/*!
@file BaseCamera.h
@brief カメラクラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/
#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	カメラクラス
	//--------------------------------------------------------------------------------------
	class BaseCamera : public ObjectInterface {
		bsm::Vec3 m_eye;
		bsm::Vec3 m_at;
		bsm::Vec3 m_up;
		float m_near;
		float m_far;
		bool m_RH;
	protected:
		//ビュー行列
		bsm::Mat4x4 m_viewMatrix;
		//射影行列
		bsm::Mat4x4 m_projMatrix;
		//プロテクトコンストラクタ
		BaseCamera();
		//プロテクトデストラクタ
		virtual ~BaseCamera() {}
	public:
		//RH法かどうかを得る
		bool GetRH() const;
		//RH法かどうか設定する
		void SetRH(bool b);
		//カメラの位置を得る
		const bsm::Vec3& GetEye() const;
		//カメラの位置を設定する
		void SetEye(const bsm::Vec3& eye);
		//カメラの位置を設定する
		void SetEye(float x, float y, float z);
		//カメラの視点を得る
		const bsm::Vec3& GetAt() const;
		//カメラの視点を設定する
		void SetAt(const bsm::Vec3& at);
		//カメラの視点を設定する
		void SetAt(float x, float y, float z);
		//カメラの傾きを得る
		const bsm::Vec3& GetUp() const;
		//カメラの傾きを設定する
		void SetUp(const bsm::Vec3& up);
		//カメラの傾きを設定する
		void SetUp(float x, float y, float z);
		//Near距離を得る
		float GetNear() const;
		//Near距離を設定する
		void SetNear(float f);
		//Far距離を得る
		float GetFar() const;
		//Far距離を設定する
		void SetFar(float f);
		//行列を計算する
		virtual void CalculateMatrix() = 0;
		//ビュー行列を得る
		virtual const bsm::Mat4x4& GetViewMatrix();
		//射影行列を得る
		virtual const bsm::Mat4x4& GetProjMatrix();
		//初期化処理
		virtual void OnCreate()override;
		//更新処理
		virtual void OnUpdate()override;
		//描画処理。デフォルトは何も行わない
		virtual void OnDraw()override {}
		//終了処理。デフォルトは何も行わない
		virtual void OnDestroy()override {}
	};

	//--------------------------------------------------------------------------------------
	//	遠近法カメラ
	//--------------------------------------------------------------------------------------
	class PerspecCamera : public BaseCamera {
		float m_fovY;   //射影角度
		float m_aspect;	//アスペクト比
	public:
		PerspecCamera();
		virtual ~PerspecCamera() {}
		//初期化処理
		virtual void OnCreate()override;
		//画角を得る（遠近法が設定されているとき用）
		float GetFovY() const;
		//画角を設定する（遠近法が設定されているとき用）
		void SetFovY(float f);
		//アスペクト比を得る（遠近法が設定されているとき用）
		float GetAspect() const;
		//アスペクト比を設定する（遠近法が設定されているとき用）
		void SetAspect(float f);
		//行列を計算する
		virtual void CalculateMatrix()override;
	};

	//--------------------------------------------------------------------------------------
	//	正射影カメラ
	//--------------------------------------------------------------------------------------
	class OrthoCamera : public BaseCamera {
		float m_width;	//遠近法を使わない場合（正射影）の幅
		float m_height;	//遠近法を使わない場合（正射影）の高さ
	public:
		OrthoCamera();
		virtual ~OrthoCamera() {}
		//初期化処理
		virtual void OnCreate()override;
		//画面の幅を得る（遠近法が設定されないとき用）
		float GetWidth() const;
		//画面の幅を設定する（遠近法が設定されないとき用）
		void SetWidth(float f);
		//画面の高さを得る（遠近法が設定されないとき用）
		float GetHeight() const;
		//画面の高さを設定する（遠近法が設定されないとき用）
		void SetHeight(float f);
		//行列を計算する
		virtual void CalculateMatrix()override;
	};
}
using namespace basecross;
// end namespace basecross
