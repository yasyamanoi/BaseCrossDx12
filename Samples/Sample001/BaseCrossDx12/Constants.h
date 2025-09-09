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
		Vec3 scale;
		Vec3 rotOrigin;
		Quat quaternion;
		Vec3 position;
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
		Mat4x4 world;
		/// エミッシブ色
		Col4 emissive;
		/// デフューズ色
		Col4 diffuse;
		SpriteConstant()
		{
			memset(this, 0, sizeof(SpriteConstant));
			emissive = Col4(0.0f);
			diffuse = Col4(1.0f, 1.0f, 1.0f, 1.0f);
		};
	};

	//--------------------------------------------------------------------------------------
	///	SimpleConstantコンスタントバッファ構造体
	//--------------------------------------------------------------------------------------
	struct SimpleConstant
	{
		/// ワールド行列
		Mat4x4 world;
		/// ビュー行列
		Mat4x4 view;
		/// 射影行列
		Mat4x4 projection;
		/// エミッシブ色
		Col4 emissive;
		/// デフューズ色
		Col4 diffuse;
		/// スペキュラー
		Col4 specular;
		/// テクスチャ=xがアクティブかどうか
		XMUINT4 activeFlg;
		/// ライイト方向
		Vec4 lightDir;
		/// ライト位置
		Vec4 lightPos;
		/// eyeの位置
		Vec4 eyePos;
		/// ライトビュー行列
		Mat4x4 lightView;
		/// ライト射影行列
		Mat4x4 lightProjection;
		/// Bone配列
		Vec4 Bones[3 * 72];
		SimpleConstant() {
			memset(this, 0, sizeof(SimpleConstant));
			diffuse = Col4(1.0f, 1.0f, 1.0f, 1.0f);
		};
	};



	//--------------------------------------------------------------------------------------
	///	Basicシェーダー用コンスタントバッファ
	//--------------------------------------------------------------------------------------
	struct BasicConstant
	{
		Col4 diffuseColor;
		Col4 emissiveColor;
		Col4 specularColorAndPower;

		Vec4 lightDirection[3];
		Vec4 lightDiffuseColor[3];
		Vec4 lightSpecularColor[3];

		Vec4 eyePosition;

		Col4 fogColor;
		Vec4 fogVector;

		Mat4x4 world;
		Vec4 worldInverseTranspose[3];
		Mat4x4 worldViewProj;
		//汎用フラグ
		XMUINT4 activeFlg;
		//以下影
		Vec4 lightPos;
		Vec4 eyePos;
		Mat4x4 lightView;
		Mat4x4 lightProjection;

		Vec4 bones[3 * 72];
	};


}
// end namespace basecross

