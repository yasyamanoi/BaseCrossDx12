/*!
@file BaseMathInc.h
@brief 計算インクルードクラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once

#include "stdafx.h"
#include "BaseMath.h"

namespace basecross {

	namespace bsm {



		//--------------------------------------------------------------------------------------
		///	Vec2インライン実装
		//--------------------------------------------------------------------------------------
		inline Vec2::Vec2() :
			XMFLOAT2(0, 0)
		{
		}

		inline Vec2::Vec2(const Vec2& vec) :
			XMFLOAT2(vec.x, vec.y)
		{
		}

		inline Vec2::Vec2(const Vec3& vec) :
			XMFLOAT2(vec.x, vec.y)
		{
		}

		inline Vec2::Vec2(const Vec4& vec) :
			XMFLOAT2(vec.x, vec.y)
		{
		}


		inline Vec2::Vec2(const XMFLOAT2& v) :
			XMFLOAT2(v)
		{
		}


		inline Vec2::Vec2(float _x, float _y) :
			XMFLOAT2(_x, _y)
		{
		}

		inline Vec2::Vec2(float val) :
			XMFLOAT2(val, val)
		{
		}

		inline Vec2::Vec2(const XMVECTOR& vec) :
			XMFLOAT2()
		{
			XMVECTOR temp = vec;
			XMStoreFloat2((XMFLOAT2*)this, temp);
		}

		inline Vec2::operator XMVECTOR() const {
			XMFLOAT2 temp = *this;
			XMVECTOR Vec = XMLoadFloat2(&temp);
			return Vec;
		}

		inline Vec2& Vec2::operator =(const Vec2& other)
		{
			if (this != &other) {
				x = other.x;
				y = other.y;
			}
			return *this;
		}

		inline Vec2& Vec2::operator =(const Vec3& other)
		{
			x = other.x;
			y = other.y;
			return *this;
		}

		inline Vec2& Vec2::operator =(const Vec4& other)
		{
			x = other.x;
			y = other.y;
			return *this;
		}


		inline Vec2& Vec2::operator=(const XMFLOAT2& other) {
			(XMFLOAT2)*this = other;
			return *this;
		}


		inline Vec2& Vec2::operator=(const XMVECTOR& other) {
			XMVECTOR temp = other;
			XMStoreFloat2((XMFLOAT2*)this, temp);
			return *this;
		}

		inline bool Vec2::operator==(const Vec2& other)const {
			return XMVector2Equal(*this, other);
		}

		inline bool Vec2::operator!=(const Vec2& other)const {
			return !XMVector2Equal(*this, other);
		}


		inline Vec2& Vec2::setElem(int idx, float value)
		{
			*(&x + idx) = value;
			return *this;
		}

		inline float Vec2::getElem(int idx) const
		{
			return *(&x + idx);
		}

		inline float& Vec2::operator [](int idx)
		{
			return *(&x + idx);
		}

		inline float Vec2::operator [](int idx) const
		{
			return *(&x + idx);
		}

		inline const Vec2 Vec2::operator +(const Vec2& vec) const
		{
			return (Vec2)XMVectorAdd(*this, vec);
		}

		inline const Vec2 Vec2::operator +(float val) const
		{
			Vec2 temp(val, val);
			return (Vec2)XMVectorAdd(*this, temp);
		}


		inline const Vec2 Vec2::operator -(const Vec2& vec) const
		{
			return (Vec2)XMVectorSubtract(*this, vec);
		}

		inline const Vec2 Vec2::operator -(float val) const
		{
			Vec2 temp(val, val);
			return (Vec2)XMVectorSubtract(*this, temp);
		}

		inline const Vec2 Vec2::operator *(const Vec2& vec) const {
			return (Vec2)XMVectorMultiply(*this, vec);
		}

		inline const Vec2 Vec2::operator *(float val) const
		{
			Vec2 temp(val, val);
			return (Vec2)XMVectorMultiply(*this, temp);
		}

		inline Vec2& Vec2::operator +=(const Vec2& vec)
		{
			*this = *this + vec;
			return *this;
		}

		inline Vec2& Vec2::operator +=(float val)
		{
			*this = *this + val;
			return *this;
		}


		inline Vec2& Vec2::operator -=(const Vec2& vec)
		{
			*this = *this - vec;
			return *this;
		}

		inline Vec2& Vec2::operator -=(float val)
		{
			*this = *this - val;
			return *this;
		}

		inline Vec2& Vec2::operator *=(const Vec2& vec) {
			*this = *this * vec;
			return *this;
		}

		inline Vec2& Vec2::operator *=(float val)
		{
			*this = *this * val;
			return *this;
		}

		inline const Vec2 Vec2::operator /(const Vec2& vec) const {
			Vec2 temp(XMVectorReciprocal(vec));
			return (Vec2)XMVectorMultiply(*this, temp);
		}


		inline const Vec2 Vec2::operator /(float val) const
		{
			Vec2 temp(XMVectorReciprocal(Vec2(val, val)));
			return (Vec2)XMVectorMultiply(*this, temp);
		}

		inline Vec2& Vec2::operator /=(const Vec2& vec) {
			*this = *this / vec;
			return *this;
		}


		inline Vec2& Vec2::operator /=(float val)
		{
			*this = *this / val;
			return *this;
		}

		inline const Vec2 Vec2::operator -() const
		{
			Vec2 temp(-1.0f, -1.0f);
			return (Vec2)XMVectorMultiply(*this, temp);
		}

		inline Vec2& Vec2::normalize() {
			*this = (Vec2)XMVector2Normalize(*this);
			return *this;
		}

		inline Vec2& Vec2::floor(int len) {
			x = bsm::bsmUtil::floor(x, len);
			y = bsm::bsmUtil::floor(y, len);
			return *this;

		}

		inline Vec2& Vec2::round(int len) {
			x = bsm::bsmUtil::round(x, len);
			y = bsm::bsmUtil::round(y, len);

			return *this;
		}

		inline Vec2& Vec2::ceil(int len) {
			x = bsm::bsmUtil::ceil(x, len);
			y = bsm::bsmUtil::ceil(y, len);
			return *this;
		}



		inline float Vec2::dot(const Vec2& vec1)const {
			return ((Vec2)XMVector2Dot(*this, vec1)).x;
		}

		inline float Vec2::lengthSqr()const {
			return ((Vec2)XMVector2LengthSq(*this)).x;
		}

		inline float Vec2::length()const {
			return ((Vec2)XMVector2Length(*this)).x;

		}

		inline bool Vec2::isNaN() const {
			return XMVector2IsNaN(*this);
		}
		inline bool Vec2::isInfinite() const {
			return XMVector2IsInfinite(*this);
		}



		//--------------------------------------------------------------------------------------
		///	Vec3インライン実装
		//--------------------------------------------------------------------------------------
		inline Vec3::Vec3() :
			XMFLOAT3(0, 0, 0)
		{
		}


		inline Vec3::Vec3(const Vec3& vec) :
			XMFLOAT3(vec.x, vec.y, vec.z)
		{
		}

		inline Vec3::Vec3(const Vec2& vec) :
			XMFLOAT3(vec.x, vec.y, 0)
		{
		}


		inline Vec3::Vec3(const Vec4& vec) :
			XMFLOAT3(vec.x, vec.y, vec.z)
		{
		}



		inline Vec3::Vec3(const XMFLOAT3& v) :
			XMFLOAT3(v)
		{
		}


		inline Vec3::Vec3(float _x, float _y, float _z) :
			XMFLOAT3(_x, _y, _z)
		{
		}

		inline Vec3::Vec3(float val) :
			XMFLOAT3(val, val, val)
		{
		}

		inline Vec3::Vec3(const XMVECTOR& vec) :
			XMFLOAT3()
		{
			XMVECTOR temp = vec;
			XMStoreFloat3((XMFLOAT3*)this, temp);
		}

		inline Vec3::operator XMVECTOR() const {
			XMFLOAT3 temp = *this;
			XMVECTOR Vec = XMLoadFloat3(&temp);
			return Vec;
		}

		inline Vec3& Vec3::operator =(const Vec2& other)
		{
			x = other.x;
			y = other.y;
			z = 0;
			return *this;
		}

		inline Vec3& Vec3::operator =(const Vec3& other)
		{
			if (this != &other) {
				x = other.x;
				y = other.y;
				z = other.z;
			}
			return *this;
		}

		inline Vec3& Vec3::operator =(const Vec4& other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
			return *this;
		}

		inline Vec3& Vec3::operator=(const XMFLOAT3& other) {
			(XMFLOAT3)*this = other;
			return *this;
		}


		inline Vec3& Vec3::operator=(const XMVECTOR& other) {
			XMVECTOR temp = other;
			XMStoreFloat3((XMFLOAT3*)this, temp);
			return *this;
		}

		inline bool Vec3::operator==(const Vec3& other)const {
			return XMVector3Equal(*this, other);
		}

		inline bool Vec3::operator!=(const Vec3& other)const {
			return !XMVector3Equal(*this, other);
		}


		inline Vec3& Vec3::setElem(int idx, float value)
		{
			*(&x + idx) = value;
			return *this;
		}

		inline float Vec3::getElem(int idx) const
		{
			return *(&x + idx);
		}

		inline float& Vec3::operator [](int idx)
		{
			return *(&x + idx);
		}

		inline float Vec3::operator [](int idx) const
		{
			return *(&x + idx);
		}

		inline const Vec3 Vec3::operator +(const Vec3& vec) const
		{
			return (Vec3)XMVectorAdd(*this, vec);
		}

		inline const Vec3 Vec3::operator +(float val) const
		{
			Vec3 temp(val, val, val);
			return (Vec3)XMVectorAdd(*this, temp);
		}


		inline const Vec3 Vec3::operator -(const Vec3& vec) const
		{
			return (Vec3)XMVectorSubtract(*this, vec);
		}

		inline const Vec3 Vec3::operator -(float val) const
		{
			Vec3 temp(val, val, val);
			return (Vec3)XMVectorSubtract(*this, temp);
		}

		inline const Vec3 Vec3::operator *(const Vec3& vec) const {
			return (Vec3)XMVectorMultiply(*this, vec);
		}


		inline const Vec3 Vec3::operator *(float val) const
		{
			Vec3 temp(val, val, val);
			return (Vec3)XMVectorMultiply(*this, temp);
		}

		inline const Vec3 Vec3::operator *(const Mat3x3& mat) const {
			return (Vec3)XMVector3Transform(*this, mat);
		}

		inline const Vec3 Vec3::operator *(const Mat4x4& mat) const {
			return (Vec3)XMVector3Transform(*this, mat);
		}

		inline Vec3& Vec3::operator +=(const Vec3& vec)
		{
			*this = *this + vec;
			return *this;
		}

		inline Vec3& Vec3::operator +=(float val)
		{
			*this = *this + val;
			return *this;
		}


		inline Vec3& Vec3::operator -=(const Vec3& vec)
		{
			*this = *this - vec;
			return *this;
		}

		inline Vec3& Vec3::operator -=(float val)
		{
			*this = *this - val;
			return *this;
		}

		inline Vec3& Vec3::operator *=(const Vec3& vec) {
			*this = *this * vec;
			return *this;
		}

		inline Vec3& Vec3::operator *=(float val)
		{
			*this = *this * val;
			return *this;
		}

		inline Vec3& Vec3::operator *=(const Mat3x3& mat) {
			*this = *this * mat;
			return *this;
		}

		inline Vec3& Vec3::operator *=(const Mat4x4& mat) {
			*this = *this * mat;
			return *this;
		}


		inline const Vec3 Vec3::operator /(const Vec3& vec) const {
			Vec3 temp(XMVectorReciprocal(vec));
			return (Vec3)XMVectorMultiply(*this, temp);
		}

		inline const Vec3 Vec3::operator /(float val) const
		{
			Vec3 temp(XMVectorReciprocal(Vec3(val, val, val)));
			return (Vec3)XMVectorMultiply(*this, temp);
		}

		inline Vec3& Vec3::operator /=(const Vec3& vec) {
			*this = *this / vec;
			return *this;
		}


		inline Vec3& Vec3::operator /=(float val)
		{
			*this = *this / val;
			return *this;
		}

		inline const Vec3 Vec3::operator -() const
		{
			Vec3 temp(-1.0f, -1.0f, -1.0f);
			return (Vec3)XMVectorMultiply(*this, temp);
		}

		inline Vec3& Vec3::normalize() {
			*this = (Vec3)XMVector3Normalize(*this);
			return *this;
		}

		inline Vec3& Vec3::floor(int len) {
			x = bsm::bsmUtil::floor(x, len);
			y = bsm::bsmUtil::floor(y, len);
			z = bsm::bsmUtil::floor(z, len);
			return *this;

		}

		inline Vec3& Vec3::round(int len) {
			x = bsm::bsmUtil::round(x, len);
			y = bsm::bsmUtil::round(y, len);
			z = bsm::bsmUtil::round(z, len);
			return *this;
		}

		inline Vec3& Vec3::ceil(int len) {
			x = bsm::bsmUtil::ceil(x, len);
			y = bsm::bsmUtil::ceil(y, len);
			z = bsm::bsmUtil::ceil(z, len);
			return *this;
		}

		inline float Vec3::dot(const Vec3& vec1)const {
			return ((Vec3)XMVector3Dot(*this, vec1)).x;
		}

		inline Vec3& Vec3::cross(const Vec3& vec1) {
			*this = (Vec3)XMVector3Cross(*this, vec1);
			return *this;
		}



		inline float Vec3::lengthSqr()const {
			return ((Vec3)XMVector3LengthSq(*this)).x;
		}

		inline float Vec3::length()const {
			return ((Vec3)XMVector3Length(*this)).x;

		}

		inline Vec3& Vec3::reflect(const Vec3& normal) {
			*this = (Vec3)XMVector3Reflect(*this, normal);
			return *this;
		}


		inline bool Vec3::isNaN() const {
			return XMVector3IsNaN(*this);
		}
		inline bool Vec3::isInfinite() const {
			return XMVector3IsInfinite(*this);
		}



		//--------------------------------------------------------------------------------------
		///	Vec4インライン実装
		//--------------------------------------------------------------------------------------
		inline Vec4::Vec4() :
			XMFLOAT4(0, 0, 0, 0)
		{
		}


		inline Vec4::Vec4(const Vec4& vec) :
			XMFLOAT4(vec.x, vec.y, vec.z, vec.w)
		{
		}

		inline Vec4::Vec4(const Vec2& vec) :
			XMFLOAT4(vec.x, vec.y, 0, 0)
		{
		}

		inline Vec4::Vec4(const Vec3& vec) :
			XMFLOAT4(vec.x, vec.y, vec.z, 0)
		{
		}


		inline Vec4::Vec4(const Vec3& vec, float _w) :
			XMFLOAT4(vec.x, vec.y, vec.z, _w)
		{
		}

		inline Vec4::Vec4(const XMFLOAT2& v) :
			XMFLOAT4(v.x, v.y, 0.0f, 0.0f)
		{
		}

		inline Vec4::Vec4(const XMFLOAT3& v) :
			XMFLOAT4(v.x, v.y, v.z, 0.0f)
		{
		}

		inline Vec4::Vec4(const XMFLOAT4& v) :
			XMFLOAT4(v)
		{
		}


		inline Vec4::Vec4(float _x, float _y, float _z, float _w) :
			XMFLOAT4(_x, _y, _z, _w)
		{
		}

		inline Vec4::Vec4(float val) :
			XMFLOAT4(val, val, val, val)
		{
		}

		inline Vec4::Vec4(const XMVECTOR& vec) :
			XMFLOAT4()
		{
			XMVECTOR temp = vec;
			XMStoreFloat4((XMFLOAT4*)this, temp);
		}

		inline Vec4::operator XMVECTOR() const {
			XMFLOAT4 temp = *this;
			XMVECTOR Vec = XMLoadFloat4(&temp);
			return Vec;
		}

		inline Vec4::operator Vec3() const {
			return Vec3(x, y, z);
		}

		inline Vec4& Vec4::operator =(const Vec2& other) {
			x = other.x;
			y = other.y;
			z = 0;
			w = 0;
			return *this;
		}


		inline Vec4& Vec4::operator =(const Vec3& other) {
			x = other.x;
			y = other.y;
			z = other.z;
			w = 0;
			return *this;
		}

		inline Vec4& Vec4::operator =(const Vec4& other)
		{
			if (this != &other) {
				x = other.x;
				y = other.y;
				z = other.z;
				w = other.w;
			}
			return *this;
		}


		inline Vec4& Vec4::operator=(const XMFLOAT4& other) {
			(XMFLOAT4)*this = other;
			return *this;
		}


		inline Vec4& Vec4::operator=(const XMVECTOR& other) {
			XMVECTOR temp = other;
			XMStoreFloat4((XMFLOAT4*)this, temp);
			return *this;
		}

		inline bool Vec4::operator==(const Vec4& other)const {
			return XMVector4Equal(*this, other);
		}

		inline bool Vec4::operator!=(const Vec4& other)const {
			return !XMVector4Equal(*this, other);
		}


		inline Vec4& Vec4::setElem(int idx, float value)
		{
			*(&x + idx) = value;
			return *this;
		}

		inline float Vec4::getElem(int idx) const
		{
			return *(&x + idx);
		}

		inline float& Vec4::operator [](int idx)
		{
			return *(&x + idx);
		}

		inline float Vec4::operator [](int idx) const
		{
			return *(&x + idx);
		}

		inline const Vec4 Vec4::operator +(const Vec4& vec) const
		{
			return (Vec4)XMVectorAdd(*this, vec);
		}

		inline const Vec4 Vec4::operator +(float val) const
		{
			Vec4 temp(val, val, val, val);
			return (Vec4)XMVectorAdd(*this, temp);
		}


		inline const Vec4 Vec4::operator -(const Vec4& vec) const
		{
			return (Vec4)XMVectorSubtract(*this, vec);
		}

		inline const Vec4 Vec4::operator -(float val) const
		{
			Vec4 temp(val, val, val, val);
			return (Vec4)XMVectorSubtract(*this, temp);
		}

		inline const Vec4 Vec4::operator *(const Vec4& vec) const {
			return (Vec4)XMVectorMultiply(*this, vec);
		}

		inline const Vec4 Vec4::operator *(float val) const
		{
			Vec4 temp(val, val, val, val);
			return (Vec4)XMVectorMultiply(*this, temp);
		}

		inline const Vec4 Vec4::operator *(const Mat3x3& mat) const {
			return (Vec4)XMVector4Transform(*this, mat);
		}

		inline const Vec4  Vec4::operator *(const Mat4x4& mat) const {
			return (Vec4)XMVector4Transform(*this, mat);
		}

		inline Vec4& Vec4::operator +=(const Vec4& vec)
		{
			*this = *this + vec;
			return *this;
		}

		inline Vec4& Vec4::operator +=(float val)
		{
			*this = *this + val;
			return *this;
		}

		inline Vec4& Vec4::operator -=(const Vec4& vec)
		{
			*this = *this - vec;
			return *this;
		}

		inline Vec4& Vec4::operator -=(float val)
		{
			*this = *this - val;
			return *this;
		}

		inline Vec4& Vec4::operator *=(const Vec4& vec) {
			*this = *this * vec;
			return *this;
		}


		inline Vec4& Vec4::operator *=(float val)
		{
			*this = *this * val;
			return *this;
		}

		inline Vec4& Vec4::operator *=(const Mat3x3& mat) {
			*this = *this * mat;
			return *this;
		}

		inline Vec4& Vec4::operator *=(const Mat4x4& mat) {
			*this = *this * mat;
			return *this;
		}

		inline const Vec4 Vec4::operator /(const Vec4& vec) const {
			Vec4 temp(XMVectorReciprocal(vec));
			return (Vec4)XMVectorMultiply(*this, temp);
		}


		inline const Vec4 Vec4::operator /(float val) const
		{
			Vec4 temp(XMVectorReciprocal(Vec4(val, val, val, val)));
			return (Vec4)XMVectorMultiply(*this, temp);
		}

		inline Vec4& Vec4::operator /=(const Vec4& vec) {
			*this = *this / vec;
			return *this;
		}


		inline Vec4& Vec4::operator /=(float val)
		{
			*this = *this / val;
			return *this;
		}

		inline const Vec4 Vec4::operator -() const
		{
			Vec4 temp(-1.0f, -1.0f, -1.0f, -1.0f);
			return (Vec4)XMVectorMultiply(*this, temp);
		}

		inline Vec4& Vec4::normalize() {
			*this = (Vec4)XMVector4Normalize(*this);
			return *this;
		}

		inline Vec4& Vec4::floor(int len) {
			x = bsm::bsmUtil::floor(x, len);
			y = bsm::bsmUtil::floor(y, len);
			z = bsm::bsmUtil::floor(z, len);
			w = bsm::bsmUtil::floor(w, len);
			return *this;

		}

		inline Vec4& Vec4::round(int len) {
			x = bsm::bsmUtil::round(x, len);
			y = bsm::bsmUtil::round(y, len);
			z = bsm::bsmUtil::round(z, len);
			w = bsm::bsmUtil::round(w, len);
			return *this;
		}

		inline Vec4& Vec4::ceil(int len) {
			x = bsm::bsmUtil::ceil(x, len);
			y = bsm::bsmUtil::ceil(y, len);
			z = bsm::bsmUtil::ceil(z, len);
			w = bsm::bsmUtil::ceil(w, len);
			return *this;
		}



		inline float Vec4::dot(const Vec4& vec1)const {
			return ((Vec4)XMVector4Dot(*this, vec1)).x;
		}

		inline float Vec4::lengthSqr()const {
			return ((Vec4)XMVector4LengthSq(*this)).x;
		}

		inline float Vec4::length()const {
			return ((Vec4)XMVector4Length(*this)).x;

		}

		inline bool Vec4::isNaN() const {
			return XMVector4IsNaN(*this);
		}
		inline bool Vec4::isInfinite() const {
			return XMVector4IsInfinite(*this);
		}



		//--------------------------------------------------------------------------------------
		///	Quatインライン関数
		//--------------------------------------------------------------------------------------
		inline Quat::Quat() :
			XMFLOAT4()
		{
			identity();
		}

		inline Quat::Quat(const Quat& quat) :
			XMFLOAT4(quat.x, quat.y, quat.z, quat.w)
		{
		}

		inline Quat::Quat(const XMFLOAT4& v) :
			XMFLOAT4(v)
		{
		}

		inline Quat::Quat(float _x, float _y, float _z, float _w) :
			XMFLOAT4(_x, _y, _z, _w)
		{
		}

		inline Quat::Quat(const Vec3& vec, float r) :
			XMFLOAT4()
		{
			*this = (Quat)XMQuaternionRotationAxis(vec, r);
		}

		inline Quat::Quat(const XMVECTOR& other) :
			XMFLOAT4()
		{
			XMVECTOR temp = other;
			XMStoreFloat4((XMFLOAT4*)this, temp);
		}

		inline Quat::operator XMVECTOR() const {
			XMFLOAT4 temp = *this;
			XMVECTOR Vec = XMLoadFloat4(&temp);
			return Vec;
		}


		inline Quat& Quat::operator=(const XMFLOAT4& other) {
			(XMFLOAT4)*this = other;
			return *this;
		}


		inline Quat& Quat::operator=(const XMVECTOR& other) {
			XMVECTOR temp = other;
			XMStoreFloat4((XMFLOAT4*)this, temp);
			return *this;
		}

		inline Quat& Quat::operator =(const Quat& quat)
		{
			if (this != &quat) {
				x = quat.x;
				y = quat.y;
				z = quat.z;
				w = quat.w;
			}
			return *this;
		}

		inline bool Quat::operator==(const Quat& other)const {
			return XMQuaternionEqual(*this, other);
		}

		inline bool Quat::operator!=(const Quat& other)const {
			return !XMQuaternionEqual(*this, other);
		}


		inline Quat& Quat::setElem(int idx, float value)
		{
			*(&x + idx) = value;
			return *this;
		}

		inline float Quat::getElem(int idx) const
		{
			return *(&x + idx);
		}

		inline float& Quat::operator [](int idx)
		{
			return *(&x + idx);
		}

		inline float Quat::operator [](int idx) const
		{
			return *(&x + idx);
		}

		inline const Quat Quat::operator +(const Quat& quat) const
		{
			return (Quat)XMVectorAdd(*this, quat);
		}

		inline const Quat Quat::operator -(const Quat& quat) const
		{
			return (Quat)XMVectorSubtract(*this, quat);
		}

		inline const Quat Quat::operator *(const Quat& quat) const
		{
			return (Quat)XMQuaternionMultiply(*this, quat);
		}

		inline const Quat Quat::operator *(float val) const
		{
			Quat temp(val, val, val, val);
			return (Quat)XMVectorMultiply(*this, temp);
		}

		inline Quat& Quat::operator *=(const Quat& quat) {
			*this = *this * quat;
			return *this;
		}


		inline Quat& Quat::normalize() {
			*this = (Quat)XMQuaternionNormalize(*this);
			return *this;
		}

		inline float Quat::dot(const Quat& quat)const {
			return ((Quat)XMQuaternionDot(*this, quat)).x;
		}

		inline Quat& Quat::conjugate(const Quat& quat) {
			*this = (Quat)XMQuaternionConjugate(quat);
			return *this;
		}

		inline Quat& Quat::identity()
		{
			*this = (Quat)XMQuaternionIdentity();
			return *this;
		}

		inline Quat& Quat::rotationAxis(const Vec3& unitVec, float radians) {
			*this = (Quat)XMQuaternionRotationAxis(unitVec, radians);
			return *this;
		}


		inline Quat& Quat::rotationRollPitchYawFromVector(const Vec3& rotVec) {
			*this = (Quat)XMQuaternionRotationRollPitchYawFromVector(rotVec);
			return *this;
		}

		inline const Vec3 Quat::toRotVec() const {
			Quat Temp = *this;
			Temp.normalize();
			Mat4x4 mt(Temp);
			Vec3 Rot;
			if (mt._32 == 1.0f) {
				Rot.x = XM_PI / 2.0f;
				Rot.y = 0;
				Rot.z = -atan2(mt._21, mt._11);
			}
			else if (mt._32 == -1.0f) {
				Rot.x = -XM_PI / 2.0f;
				Rot.y = 0;
				Rot.z = -atan2(mt._21, mt._11);
			}
			else {
				Rot.x = -asin(mt._32);
				Rot.y = -atan2(-mt._31, mt._33);
				Rot.z = atan2(mt._12, mt._11);
			}
			return Rot;
		}

		inline Quat& Quat::inverse() {
			*this = (Quat)XMQuaternionInverse(*this);
			return *this;
		}


		inline Quat& Quat::facing(const Vec3& norm) {
			Vec3 DefUp(0, 1.0f, 0);
			Vec3 Temp = norm;
			Vec2 TempVec2(Temp.x, Temp.z);
			if (TempVec2.length() < 0.1f) {
				DefUp = Vec3(0, 0, 1.0f);
			}
			Temp.normalize();
			Mat4x4 RotMatrix;
			RotMatrix = XMMatrixLookAtLH(Vec3(0, 0, 0), Temp, DefUp);
			RotMatrix.inverse();
			*this = RotMatrix.quatInMatrix();
			(*this).normalize();
			return *this;
		}


		inline Quat& Quat::facingY(const Vec3& norm) {
			Vec3 Temp = norm;
			Temp.normalize();
			*this = XMQuaternionRotationRollPitchYaw(0, atan2(Temp.x, Temp.z), 0);
			(*this).normalize();
			return *this;
		}

		inline bool Quat::isNaN() const {
			return XMQuaternionIsNaN(*this);
		}
		inline bool Quat::isInfinite() const {
			return XMQuaternionIsInfinite(*this);
		}

		//--------------------------------------------------------------------------------------
		///	Mat3x3インライン関数
		//--------------------------------------------------------------------------------------
		inline Mat3x3::Mat3x3() :
			XMFLOAT3X3()
		{
			identity();
		}

		inline Mat3x3::Mat3x3(const Mat3x3& mat) :
			XMFLOAT3X3(mat)
		{
		}

		inline Mat3x3::Mat3x3(const XMFLOAT3X3& other) :
			XMFLOAT3X3(other)
		{

		}

		inline Mat3x3::Mat3x3(const XMMATRIX& other) :
			XMFLOAT3X3()
		{
			XMMATRIX temp = other;
			XMStoreFloat3x3((XMFLOAT3X3*)this, temp);
		}

		inline  Mat3x3::operator XMMATRIX() const {
			Mat3x3 temp = *this;
			XMMATRIX m = XMLoadFloat3x3(&temp);
			return m;
		}

		inline Mat3x3& Mat3x3::operator=(const XMFLOAT3X3& other) {
			(XMFLOAT3X3)*this = other;
			return *this;
		}


		inline Mat3x3& Mat3x3::operator=(const XMMATRIX& other) {
			XMMATRIX temp = other;
			XMStoreFloat3x3((XMFLOAT3X3*)this, temp);
			return *this;
		}

		inline Mat3x3& Mat3x3::operator =(const Mat3x3& mat)
		{
			if (this != &mat) {
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						m[i][j] = mat[i][j];
					}
				}
			}
			return *this;
		}

		inline bool Mat3x3::operator==(const Mat3x3& other)const {
			for (int i = 0; i < 3; i++) {
				if (!XMVector3Equal(getMajor(i), other.getMajor(i))) {
					return false;
				}
			}
			return true;
		}

		inline bool Mat3x3::operator!=(const Mat3x3& other)const {
			for (int i = 0; i < 3; i++) {
				if (!XMVector3Equal(getMajor(i), other.getMajor(i))) {
					return true;
				}
			}
			return false;
		}



		inline Mat3x3::Mat3x3(const Quat& unitQuat) :
			XMFLOAT3X3()
		{
			*this = (Mat3x3)XMMatrixRotationQuaternion(unitQuat);
		}

		inline Mat3x3::Mat3x3(const Vec3& _major0, const Vec3& _major1, const Vec3& _major2) :
			XMFLOAT3X3()
		{
			setMajor(0, _major0);
			setMajor(1, _major1);
			setMajor(2, _major2);
		}

		inline Mat3x3& Mat3x3::setMajor(int major, const Vec3& vec)
		{
			m[major][0] = vec.x;
			m[major][1] = vec.y;
			m[major][2] = vec.z;
			return *this;
		}

		inline Mat3x3& Mat3x3::setMinor(int minor, const Vec3& vec)
		{
			setElem(0, minor, vec.getElem(0));
			setElem(1, minor, vec.getElem(1));
			setElem(2, minor, vec.getElem(2));
			return *this;
		}

		inline Mat3x3& Mat3x3::setElem(int major, int minor, float val)
		{
			m[major][minor] = val;
			return *this;
		}

		inline float Mat3x3::getElem(int major, int minor) const
		{
			return this->getMajor(major).getElem(minor);
		}

		inline const Vec3 Mat3x3::getMajor(int major) const
		{
			Vec3 temp(m[major][0], m[major][1], m[major][2]);
			return temp;
		}

		inline const Vec3 Mat3x3::getMinor(int minor) const
		{
			return Vec3(m[0][minor], m[1][minor], m[2][minor]);
		}

		inline Vec3& Mat3x3::operator [](int major)
		{
			return (Vec3&)(m[major][0]);
		}

		inline const Vec3 Mat3x3::operator [](int major) const
		{
			return (Vec3&)(m[major][0]);
		}


		inline const Mat3x3 Mat3x3::operator +(const Mat3x3& mat) const
		{
			Mat3x3 temp;
			for (int i = 0; i < 3; i++) {
				temp[i] = getMajor(i) + mat.getMajor(i);
			}
			return temp;
		}

		inline const Mat3x3 Mat3x3::operator -(const Mat3x3& mat) const
		{
			Mat3x3 temp;
			for (int i = 0; i < 3; i++) {
				temp[i] = getMajor(i) - mat.getMajor(i);
			}
			return temp;
		}

		inline const Mat3x3 Mat3x3::operator -() const
		{
			Mat3x3 temp;
			for (int i = 0; i < 3; i++) {
				temp[i] = -getMajor(i);
			}
			return temp;
		}

		inline const Mat3x3 Mat3x3::operator *(float val) const
		{
			Mat3x3 temp;
			for (int i = 0; i < 3; i++) {
				temp[i] = getMajor(i) * val;
			}
			return temp;
		}

		inline const Vec3 Mat3x3::operator *(const Vec3& vec) const
		{
			return (Vec3)XMVector3Transform(vec, *this);
		}


		inline const Mat3x3 Mat3x3::operator *(const Mat3x3& mat) const
		{
			return  (Mat3x3)XMMatrixMultiply(*this, mat);
		}



		inline Mat3x3& Mat3x3::operator +=(const Mat3x3& mat)
		{
			*this = *this + mat;
			return *this;
		}

		inline Mat3x3& Mat3x3::operator -=(const Mat3x3& mat)
		{
			*this = *this - mat;
			return *this;
		}

		inline Mat3x3& Mat3x3::operator *=(float val) {
			*this = *this * val;
			return *this;
		}

		inline Mat3x3& Mat3x3::operator *=(const Mat3x3& mat) {
			*this = *this * mat;
			return *this;
		}


		inline Mat3x3& Mat3x3::identity()
		{
			*this = (Mat3x3)XMMatrixIdentity();
			return *this;
		}

		inline Mat3x3& Mat3x3::scale(const Vec3& scaleVec)
		{
			*this = (Mat3x3)XMMatrixScalingFromVector(scaleVec);
			return *this;
		}

		inline Mat3x3& Mat3x3::rotation(const Quat& unitQuat)
		{
			*this = (Mat3x3)XMMatrixRotationQuaternion(unitQuat);
			return *this;
		}

		inline Mat3x3& Mat3x3::transpose() {
			*this = (Mat3x3)XMMatrixTranspose(*this);
			return *this;
		}
		inline Mat3x3& Mat3x3::inverse() {
			XMVECTOR Vec;
			*this = (Mat3x3)XMMatrixInverse(&Vec, *this);
			return *this;
		}

		inline Mat3x3& Mat3x3::crossMatrix(const Vec3& vec) {
			*this = Mat3x3(
				Vec3(0.0f, vec.z, -vec.y),
				Vec3(-vec.z, 0.0f, vec.x),
				Vec3(vec.y, -vec.x, 0.0f)
			);
			return *this;
		}


		inline Vec3 Mat3x3::rotXInMatrix()const {
			Vec3 ret(_11, _12, _13);
			return ret;
		}
		inline Vec3 Mat3x3::rotYInMatrix()const {
			Vec3 ret(_21, _22, _23);
			return ret;
		}
		inline Vec3 Mat3x3::rotZInMatrix()const {
			Vec3 ret(_31, _32, _33);
			return ret;
		}

		//--------------------------------------------------------------------------------------
		///	Mat4x4インライン関数
		//--------------------------------------------------------------------------------------
		inline Mat4x4::Mat4x4() :
			XMFLOAT4X4()
		{
			identity();
		}


		inline Mat4x4::Mat4x4(const Mat4x4& mat) :
			XMFLOAT4X4(mat)
		{
		}

		inline Mat4x4::Mat4x4(const aiMatrix4x4& mat) {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					m[i][j] = mat[i][j];
				}
			}
		}

		inline Mat4x4::Mat4x4(const Vec3& _major0, const Vec3& _major1, const Vec3& _major2, const Vec3& _major3) :
			XMFLOAT4X4()
		{
			identity();
			setMajor(0, _major0);
			setMajor(1, _major1);
			setMajor(2, _major2);
			setMajor(3, _major3);
		}

		inline Mat4x4::Mat4x4(const Vec4& _major0, const Vec4& _major1, const Vec4& _major2, const Vec4& _major3) :
			XMFLOAT4X4()
		{
			identity();
			setMajor(0, _major0);
			setMajor(1, _major1);
			setMajor(2, _major2);
			setMajor(3, _major3);
		}

		inline Mat4x4::Mat4x4(const Mat3x3& tfrm, const Vec3& translateVec)
		{
			identity();
			this->setUpper3x3(tfrm);
			this->setTranslation(translateVec);
		}

		inline Mat4x4::Mat4x4(const Quat& unitQuat, const Vec3& translateVec)
		{
			identity();
			this->setUpper3x3(Mat3x3(unitQuat));
			this->setTranslation(translateVec);
		}


		inline Mat4x4::Mat4x4(const Quat& unitQuat) :
			XMFLOAT4X4()
		{
			*this = (Mat4x4)XMMatrixRotationQuaternion(XMVECTOR(unitQuat));
		}

		inline Mat4x4::Mat4x4(const XMFLOAT4X4& other) :
			XMFLOAT4X4(other)
		{

		}


		inline Mat4x4::Mat4x4(const XMMATRIX& other) :
			XMFLOAT4X4()
		{
			XMMATRIX temp = other;
			XMStoreFloat4x4((XMFLOAT4X4*)this, temp);
		}

		inline  Mat4x4::operator XMMATRIX() const {
			Mat4x4 temp = *this;
			XMMATRIX m = XMLoadFloat4x4(&temp);
			return m;
		}

		inline Mat4x4& Mat4x4::operator=(const XMFLOAT4X4& other) {
			(XMFLOAT4X4)*this = other;
			return *this;
		}

		inline Mat4x4& Mat4x4::operator=(const XMMATRIX& other) {
			XMMATRIX temp = other;
			XMStoreFloat4x4((XMFLOAT4X4*)this, temp);
			return *this;
		}

		inline bool Mat4x4::operator==(const Mat4x4& other)const {
			for (int i = 0; i < 4; i++) {
				if (!XMVector4Equal(getMajor(i), other.getMajor(i))) {
					return false;
				}
			}
			return true;
		}

		inline bool Mat4x4::operator!=(const Mat4x4& other)const {
			for (int i = 0; i < 4; i++) {
				if (!XMVector4Equal(getMajor(i), other.getMajor(i))) {
					return true;
				}
			}
			return false;
		}

		inline bool Mat4x4::equalInt(const Mat4x4& other)const {
			for (int i = 0; i < 4; i++) {
				if (!XMVector4EqualInt(getMajor(i), other.getMajor(i))) {
					return false;
				}
			}
			return true;
		}

		inline bool Mat4x4::nearEqual(const Mat4x4& other, float epsilon)const {
			Vec4 temp(epsilon, epsilon, epsilon, epsilon);
			for (int i = 0; i < 4; i++) {
				if (!XMVector4NearEqual(getMajor(i), other.getMajor(i), temp)) {
					return false;
				}
			}
			return true;

		}

		inline Mat4x4& Mat4x4::setUpper3x3(const Mat3x3& tfrm)
		{
			this->operator[](0) = tfrm.getMajor(0);
			this->operator[](1) = tfrm.getMajor(1);
			this->operator[](2) = tfrm.getMajor(2);
			return *this;
		}

		inline const Mat3x3 Mat4x4::getUpper3x3() const
		{
			return Mat3x3(getMajor(0), getMajor(1), getMajor(2));
		}

		inline Mat4x4& Mat4x4::setTranslation(const Vec3& translateVec)
		{
			this->operator[](3) = translateVec;
			m[3][3] = 1.0f;
			return *this;
		}

		inline const Vec3 Mat4x4::getTranslation() const
		{
			return (Vec3)getMajor(3);
		}


		inline Mat4x4& Mat4x4::setMajor(int major, const Vec3& vec)
		{
			m[major][0] = vec.x;
			m[major][1] = vec.y;
			m[major][2] = vec.z;
			return *this;
		}

		inline Mat4x4& Mat4x4::setMajor(int major, const Vec4& vec)
		{
			m[major][0] = vec.x;
			m[major][1] = vec.y;
			m[major][2] = vec.z;
			m[major][3] = vec.w;
			m[3][3] = 1.0f;
			return *this;
		}


		inline Mat4x4& Mat4x4::setMinor(int minor, const Vec3& vec)
		{
			setElem(0, minor, vec.getElem(0));
			setElem(1, minor, vec.getElem(1));
			setElem(2, minor, vec.getElem(2));
			return *this;
		}

		inline Mat4x4& Mat4x4::setMinor(int minor, const Vec4& vec)
		{
			setElem(0, minor, vec.getElem(0));
			setElem(1, minor, vec.getElem(1));
			setElem(2, minor, vec.getElem(2));
			setElem(3, minor, vec.getElem(3));
			m[3][3] = 1.0f;
			return *this;
		}
		inline Mat4x4& Mat4x4::setElem(int major, int minor, float val)
		{
			m[major][minor] = val;
			return *this;
		}

		inline float Mat4x4::getElem(int major, int minor) const
		{
			return this->getMajor(major).getElem(minor);
		}

		inline const Vec4 Mat4x4::getMajor(int major) const
		{
			Vec4 temp(m[major][0], m[major][1], m[major][2], m[major][3]);
			return temp;
		}

		inline const Vec4 Mat4x4::getMinor(int minor) const
		{
			return Vec4(m[0][minor], m[1][minor], m[2][minor], m[3][minor]);
		}

		inline Vec4& Mat4x4::operator [](int major)
		{
			return (Vec4&)(m[major][0]);
		}

		inline const Vec4 Mat4x4::operator [](int major) const
		{
			return (Vec4&)(m[major][0]);
		}

		inline Mat4x4& Mat4x4::operator =(const Mat4x4& mat)
		{
			if (this != &mat) {
				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 4; j++) {
						m[i][j] = mat[i][j];
					}
				}
			}
			return *this;
		}


		inline const Mat4x4 Mat4x4::operator +(const Mat4x4& mat) const
		{
			Mat4x4 temp;
			for (int i = 0; i < 4; i++) {
				temp[i] = getMajor(i) + mat.getMajor(i);
			}
			temp[3][3] = 1.0f;
			return temp;
		}

		inline const Mat4x4 Mat4x4::operator -(const Mat4x4& mat) const
		{
			Mat4x4 temp;
			for (int i = 0; i < 4; i++) {
				temp[i] = getMajor(i) - mat.getMajor(i);
			}
			temp[3][3] = 1.0f;
			return temp;
		}

		inline const Mat4x4 Mat4x4::operator -() const
		{
			Mat4x4 temp;
			for (int i = 0; i < 4; i++) {
				temp[i] = -getMajor(i);
			}
			temp[3][3] = 1.0f;
			return temp;
		}

		inline const Mat4x4 Mat4x4::operator *(float val) const
		{
			Mat4x4 temp;
			for (int i = 0; i < 4; i++) {
				temp[i] = getMajor(i) * val;
			}
			temp[3][3] = 1.0f;
			return temp;
		}

		inline const Vec3 Mat4x4::operator *(const Vec3& vec) const
		{
			return (Vec3)XMVector3Transform(vec, *this);
		}

		inline const Vec4 Mat4x4::operator *(const Vec4& vec) const
		{
			return (Vec4)XMVector4Transform(vec, *this);
		}



		inline const Mat4x4 Mat4x4::operator *(const Mat4x4& mat) const
		{
			return  (Mat4x4)XMMatrixMultiply(*this, mat);
		}

		inline const Mat4x4 Mat4x4::operator *(const Mat3x3& mat) const
		{
			return  (Mat4x4)XMMatrixMultiply(*this, mat);
		}


		inline Mat4x4& Mat4x4::operator +=(const Mat4x4& mat)
		{
			*this = *this + mat;
			return *this;
		}

		inline Mat4x4& Mat4x4::operator -=(const Mat4x4& mat)
		{
			*this = *this - mat;
			return *this;
		}

		inline Mat4x4& Mat4x4::operator *=(float val) {
			*this = *this * val;
			return *this;
		}


		inline Mat4x4& Mat4x4::operator *=(const Mat4x4& tfrm)
		{
			*this = *this * tfrm;
			return *this;
		}

		inline Mat4x4& Mat4x4::operator *=(const Mat3x3& tfrm)
		{
			*this = *this * tfrm;
			return *this;
		}

		inline void Mat4x4::decompose(Vec3& rScaling, Quat& rQt, Vec3& rTranslation)const {
			XMVECTOR Scale;
			XMVECTOR Qt;
			XMVECTOR Translation;
			if (XMMatrixDecompose(&Scale, &Qt, &Translation, *this)) {
				rScaling = Scale;
				rQt = Qt;
				rTranslation = Translation;
			}
			else {
				//スケーリングマイナスの場合の処理
				Vec3 XVec(_11, _12, _13);
				Vec3 YVec(_21, _22, _23);
				Vec3 ZVec(_31, _32, _33);
				auto XLen = XVec.length();
				auto YLen = YVec.length();;
				auto ZLen = ZVec.length();
				//スケーリングが0の要素がある
				if (XLen == 0.0f) {
					XLen = 0.0001f;
				}
				if (YLen == 0.0f) {
					YLen = 0.0001f;
				}
				if (ZLen == 0.0f) {
					ZLen = 0.0001f;
				}
				rScaling = Vec3(XLen, YLen, ZLen);
				rTranslation = Vec3(_41, _42, _43);

				Vec3 vX = Vec3(_11, _12, _13) / XLen;
				Vec3 vY = Vec3(_21, _22, _23) / YLen;
				Vec3 vZ = Vec3(_31, _32, _33) / ZLen;

				Mat4x4 retM;
				retM.identity();
				retM._11 = vX.x;
				retM._12 = vX.y;
				retM._13 = vX.z;

				retM._21 = vY.x;
				retM._22 = vY.y;
				retM._23 = vY.z;

				retM._31 = vZ.x;
				retM._32 = vZ.y;
				retM._33 = vZ.z;

				rQt = (Quat)XMQuaternionRotationMatrix(retM);

			}
		}

		inline Vec3 Mat4x4::scaleInMatrix()const {
			Vec3 scale, pos;
			Quat qt;
			decompose(scale, qt, pos);
			return scale;
		}
		inline Quat Mat4x4::quatInMatrix()const {
			Vec3 scale, pos;
			Quat qt;
			decompose(scale, qt, pos);
			return qt;
		}
		inline Vec3 Mat4x4::transInMatrix()const {
			Vec3 pos;
			pos.x = _41;
			pos.y = _42;
			pos.z = _43;
			return pos;
		}

		inline Mat4x4& Mat4x4::identity()
		{
			*this = (Mat4x4)XMMatrixIdentity();
			return *this;
		}

		inline Mat4x4& Mat4x4::scaleIdentity() {
			Vec3 Scale, Translation;
			Quat Qt;
			decompose(Scale, Qt, Translation);
			Scale = Vec3(1.0f, 1.0f, 1.0f);
			return affineTransformation(Scale, Vec3(0.0f, 0.0f, 0.0f), Qt, Translation);
		}

		inline Mat4x4& Mat4x4::scale(const Vec3& scaleVec)
		{
			*this = (Mat4x4)XMMatrixScalingFromVector(scaleVec);
			return *this;
		}

		inline Mat4x4& Mat4x4::rotation(const Quat& unitQuat)
		{
			*this = (Mat4x4)XMMatrixRotationQuaternion(unitQuat);
			return *this;
		}

		inline Mat4x4& Mat4x4::translation(const Vec3& transVec) {
			*this = (Mat4x4)XMMatrixTranslationFromVector(transVec);
			return *this;
		}

		inline Mat4x4& Mat4x4::transpose() {
			*this = (Mat4x4)XMMatrixTranspose(*this);
			return *this;
		}
		inline Mat4x4& Mat4x4::inverse() {
			XMVECTOR Vec;
			*this = (Mat4x4)XMMatrixInverse(&Vec, *this);
			return *this;
		}

		inline Mat4x4& Mat4x4::affineTransformation2D(
			const Vec2& scaleVec,
			const Vec2& rotOrigin,
			const float& rotation,
			const Vec2& transVec) {
			*this = (Mat4x4)XMMatrixAffineTransformation2D(scaleVec, rotOrigin,
				rotation, transVec);
			return *this;
		}


		inline Mat4x4& Mat4x4::affineTransformation(
			const Vec3& scaleVec,
			const Vec3& rotOrigin,
			const Quat& unitQuat,
			const Vec3& transVec) {
			*this = (Mat4x4)XMMatrixAffineTransformation(scaleVec, rotOrigin,
				unitQuat, transVec);
			return *this;
		}

		inline Mat4x4& Mat4x4::lookatLH(
			const Vec3& eye,
			const Vec3& at,
			const Vec3& up) {
			*this = (Mat4x4)XMMatrixLookAtLH(eye, at, up);
			return *this;
		}

		inline Mat4x4& Mat4x4::lookatRH(
			const Vec3& eye,
			const Vec3& at,
			const Vec3& up) {
			*this = (Mat4x4)XMMatrixLookAtRH(eye, at, up);
			return *this;
		}

		inline Mat4x4& Mat4x4::perspectiveFovLH(
			float fovY,
			float aspectRatio,
			float nearPlane,
			float farPlane) {
			*this = XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearPlane, farPlane);
			return *this;
		}

		inline Mat4x4& Mat4x4::perspectiveFovRH(
			float fovY,
			float aspectRatio,
			float nearPlane,
			float farPlane) {
			*this = XMMatrixPerspectiveFovRH(fovY, aspectRatio, nearPlane, farPlane);
			return *this;
		}


		inline Mat4x4& Mat4x4::orthographicLH(
			float width,
			float height,
			float n,
			float f) {
			*this = (Mat4x4)XMMatrixOrthographicLH(width, height, n, f);
			return *this;
		}

		inline Mat4x4& Mat4x4::orthographicRH(
			float width,
			float height,
			float n,
			float f) {
			*this = (Mat4x4)XMMatrixOrthographicRH(width, height, n, f);
			return *this;
		}

		inline Mat4x4& Mat4x4::strTransformation(
			const Vec3& scaleVec,
			const Vec3& transVec,
			const Quat& unitQuat
		) {
			Mat4x4 Scale = (Mat4x4)XMMatrixScalingFromVector(scaleVec);
			Mat4x4 Trans = (Mat4x4)XMMatrixTranslationFromVector(transVec);
			Mat4x4 Qt = (Mat4x4)XMMatrixRotationQuaternion(unitQuat);
			*this = Scale * Trans * Qt;
			return *this;
		}

		inline Vec3 Mat4x4::rotXInMatrix()const {
			Vec3 ret(_11, _12, _13);
			Vec3 Scale = scaleInMatrix();
			ret.x /= Scale.x;
			ret.y /= Scale.x;
			ret.z /= Scale.x;
			return ret;
		}
		inline Vec3 Mat4x4::rotYInMatrix()const {
			Vec3 ret(_21, _22, _23);
			Vec3 Scale = scaleInMatrix();
			ret.x /= Scale.y;
			ret.y /= Scale.y;
			ret.z /= Scale.y;
			return ret;
		}
		inline Vec3 Mat4x4::rotZInMatrix()const {
			Vec3 ret(_31, _32, _33);
			Vec3 Scale = scaleInMatrix();
			ret.x /= Scale.z;
			ret.y /= Scale.z;
			ret.z /= Scale.z;
			return ret;
		}

		using Pt2 = Vec2;
		using Pt3 = Vec3;
		using Col4 = Vec4;
		using Plane4 = Vec4;


	}
	// end bsm
}
//end basecross
