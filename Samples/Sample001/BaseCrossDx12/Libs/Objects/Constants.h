/*!
@file Constants.h
@brief コンスタントバッファ等まとめ
*/
#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	オブジェクト変換用
	//--------------------------------------------------------------------------------------
	struct TransParam {
		bsm::Vec3 scale;
		bsm::Vec3 rotOrigin;
		bsm::Quat quaternion;
		bsm::Vec3 position;
		TransParam() :
			scale(1.0f),
			rotOrigin(0.0f),
			quaternion(),
			position(0.0f)
		{
		}
	};

	//--------------------------------------------------------------------------------------
	///	スプライト用コンスタントバッファ構造体
	//--------------------------------------------------------------------------------------
	struct SpriteConstant
	{
		/// ワールド行列
		bsm::Mat4x4 world;
		/// エミッシブ色
		bsm::Col4 emissive;
		/// デフューズ色
		bsm::Col4 diffuse;
		SpriteConstant()
		{
			memset(this, 0, sizeof(SpriteConstant));
			emissive = bsm::Col4(0.0f);
			diffuse = bsm::Col4(1.0f, 1.0f, 1.0f, 1.0f);
		};
	};

	//--------------------------------------------------------------------------------------
	///	SimpleConstantコンスタントバッファ構造体
	//--------------------------------------------------------------------------------------
	struct SimpleConstant
	{
		/// ワールド行列
		bsm::Mat4x4 world;
		/// ビュー行列
		bsm::Mat4x4 view;
		/// 射影行列
		bsm::Mat4x4 projection;
		/// エミッシブ色
		bsm::Col4 emissive;
		/// デフューズ色
		bsm::Col4 diffuse;
		/// スペキュラー
		bsm::Col4 specular;
		/// テクスチャ=xがアクティブかどうか
		XMUINT4 activeFlg;
		/// ライイト方向
		bsm::Vec4 lightDir;
		/// ライト位置
		bsm::Vec4 lightPos;
		/// eyeの位置
		bsm::Vec4 eyePos;
		/// ライトビュー行列
		bsm::Mat4x4 lightView;
		/// ライト射影行列
		bsm::Mat4x4 lightProjection;
		/// Bone配列
		bsm::Vec4 Bones[3 * 72];
		SimpleConstant() {
			memset(this, 0, sizeof(SimpleConstant));
			diffuse = bsm::Col4(1.0f, 1.0f, 1.0f, 1.0f);
		};
	};



	//--------------------------------------------------------------------------------------
	///	Basicシェーダー用コンスタントバッファ
	//--------------------------------------------------------------------------------------
	struct BasicConstant
	{
		bsm::Col4 diffuseColor;
		bsm::Col4 emissiveColor;
		bsm::Col4 specularColorAndPower;

		bsm::Vec4 lightDirection[3];
		bsm::Vec4 lightDiffuseColor[3];
		bsm::Vec4 lightSpecularColor[3];

		bsm::Vec4 eyePosition;

		bsm::Col4 fogColor;
		bsm::Vec4 fogVector;

		bsm::Mat4x4 world;
		bsm::Vec4 worldInverseTranspose[3];
		bsm::Mat4x4 worldViewProj;
		//汎用フラグ
		XMUINT4 activeFlg;
		//以下影
		bsm::Vec4 lightPos;
		bsm::Vec4 eyePos;
		bsm::Mat4x4 lightView;
		bsm::Mat4x4 lightProjection;

		bsm::Vec4 bones[3 * 72];
	};


}
// end namespace basecross

