/*!
@file BaseMath.h
@brief 計算ネームスペース
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once

#include "stdafx.h"
namespace basecross {

	struct TransParam {
		XMFLOAT3 scale;
		XMFLOAT3 rotOrigin;
		XMFLOAT4 quaternion;
		XMFLOAT3 position;
	};

	namespace bsm {
		static XMMATRIX affineTransformation(const TransParam& param) {
			XMVECTOR scale = XMLoadFloat3(&param.scale);
			XMVECTOR rotOrigin = XMLoadFloat3(&param.rotOrigin);
			XMVECTOR quaternion = XMLoadFloat4(&param.quaternion);
			XMVECTOR position = XMLoadFloat3(&param.position);
			return XMMatrixAffineTransformation(
				scale,
				rotOrigin,
				quaternion,
				position
			);
		}

		//XMFLOAT2からXMFLOAT3変換
		static XMFLOAT3 F2ToF3(const XMFLOAT2& f2,float z = 0.0f) {
			XMFLOAT3 ret(f2.x, f2.y, z);
			return ret;
		}

		//XMFLOAT2からXMFLOAT4変換
		static XMFLOAT4 F2ToF4(const XMFLOAT2& f2,float z = 0.0f,float w = 0.0f) {
			XMFLOAT4 ret(f2.x, f2.y, z, w);
			return ret;
		}

		//XMFLOAT3からXMFLOAT4変換
		static XMFLOAT4 F3ToF4(const XMFLOAT3& f3,float w = 0.0f) {
			XMFLOAT4 ret(f3.x, f3.y, f3.z, w);
			return ret;
		}

		//XMFLOAT2を作成する関数
		static XMFLOAT2 makeF2(const XMVECTOR& vec) {
			XMFLOAT2 tmp;
			XMStoreFloat2(&tmp, vec);
			return tmp;
		}

		//XMFLOAT3を作成する関数
		static XMFLOAT3 makeF3(const XMVECTOR& vec) {
			XMFLOAT3 tmp;
			XMStoreFloat3(&tmp, vec);
			return tmp;
		}

		//XMFLOAT4を作成する関数
		static XMFLOAT4 makeF4(const XMVECTOR& vec) {
			XMFLOAT4 tmp;
			XMStoreFloat4(&tmp, vec);
			return tmp;
		}

		//XMFLOAT3X3を作成する関数
		static XMFLOAT3X3 makeF3x3(const XMMATRIX& mat) {
			XMFLOAT3X3 tmp;
			XMStoreFloat3x3(&tmp, mat);
			return tmp;
		}

		//XMFLOAT3X4を作成する関数
		static XMFLOAT3X4 makeF3x4(const XMMATRIX& mat) {
			XMFLOAT3X4 tmp;
			XMStoreFloat3x4(&tmp, mat);
			return tmp;
		}

		//XMFLOAT4X3を作成する関数
		static XMFLOAT4X3 makeF4x3(const XMMATRIX& mat) {
			XMFLOAT4X3 tmp;
			XMStoreFloat4x3(&tmp, mat);
			return tmp;
		}

		//XMFLOAT4X4を作成する関数
		static XMFLOAT4X4 makeF4x4(const XMMATRIX& mat) {
			XMFLOAT4X4 tmp;
			XMStoreFloat4x4(&tmp, mat);
			return tmp;
		}

		//XMVECTORを作成する関数
		static XMVECTOR makeXM() {
			auto ret = XMVectorSet(0, 0, 0, 0);
			return ret;
		}

		static XMVECTOR makeXM(float all_float) {
			auto ret = XMVectorSet(all_float, all_float, all_float,all_float);
			return ret;
		}

		static XMVECTOR makeXM(float x, float y, float z = 0.0f, float w = 0.0f) {
			auto ret = XMVectorSet(x,y,z,w);
			return ret;
		}

		static XMVECTOR makeXM(const XMFLOAT2& f2,float z = 0.0f,float w = 0.0f) {
			auto ret = XMVectorSet(f2.x, f2.y, z, w);
			return ret;
		}

		static XMVECTOR makeXM(const XMFLOAT3& f3, float w = 0.0f) {
			auto ret = XMVectorSet(f3.x, f3.y, f3.z, w);
			return ret;
		}

		static XMVECTOR makeXM(const XMFLOAT4& f4) {
			auto ret = XMVectorSet(f4.x, f4.y, f4.z, f4.w);
			return ret;
		}

		//XMMATRIXを作成する関数
		static XMMATRIX makeXMMat(const XMFLOAT3X3& f3x3) {
			auto tmp = f3x3;
			return XMLoadFloat3x3(&tmp);
		}

		static XMMATRIX makeXMMat(const XMFLOAT3X4& f3x4) {
			auto tmp = f3x4;
			return XMLoadFloat3x4(&tmp);
		}

		static XMMATRIX makeXMMat(const XMFLOAT4X3& f4x3) {
			auto tmp = f4x3;
			return XMLoadFloat4x3(&tmp);
		}

		static XMMATRIX makeXMMat(const XMFLOAT4X4& f4x4) {
			auto tmp = f4x4;
			return XMLoadFloat4x4(&tmp);
		}


		//演算系
		template<typename T1,typename T2>
		static XMFLOAT3 f3add(const T1& t1, const T2& t2) {
			XMVECTOR v1 = bsm::makeXM(t1);
			XMVECTOR v2 = bsm::makeXM(t2);
			return bsm::makeF3(XMVectorAdd(v1, v2));
		}

		template<typename T1, typename T2>
		static XMFLOAT4 f4add(const T1& t1, const T2& t2) {
			XMVECTOR v1 = bsm::makeXM(t1);
			XMVECTOR v2 = bsm::makeXM(t2);
			return bsm::makeF4(XMVectorAdd(v1, v2));
		}


		template<typename T1, typename T2>
		static XMFLOAT3 f3multi(const T1& t1, const T2& t2) {
			XMVECTOR v1 = bsm::makeXM(t1);
			XMVECTOR v2 = bsm::makeXM(t2);
			return bsm::makeF3(XMVectorMultiply(v1, v2));
		}

		template<typename T1, typename T2>
		static XMFLOAT4 f4multi(const T1& t1, const T2& t2) {
			XMVECTOR v1 = bsm::makeXM(t1);
			XMVECTOR v2 = bsm::makeXM(t2);
			return bsm::makeF4(XMVectorMultiply(v1, v2));
		}

	}
}
// end namespace basecross
