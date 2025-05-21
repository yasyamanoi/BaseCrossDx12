/*!
@file SpSceneComp.h
@brief シンプルシーンコンポーネント
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	using namespace std;
	using namespace basecross::bsm;

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

	DECLARE_DX12SHADER(SpVSPNTStatic)
	DECLARE_DX12SHADER(SpVSPNTStaticShadow)

	DECLARE_DX12SHADER(SpPSPNTStatic)
	DECLARE_DX12SHADER(SpPSPNTStaticShadow)

	//--------------------------------------------------------------------------------------
	///	SpSceneCompコンポーネント
	//--------------------------------------------------------------------------------------
	class SpSceneComp : public Component {
		/// エミッシブ色
		Col4 m_emissive;
		/// デフューズ色
		Col4 m_diffuse;
		/// スペキュラー
		Col4 m_specular;
		//自分自身に影を描画するかどうか
		bool m_ownShadowActive;
	protected:
		weak_ptr<BaseMesh> m_mesh;
		weak_ptr <BaseTexture> m_texture;
		SimpleConstant m_constantBuffer;
		size_t m_constantBufferIndex;
	public:
		explicit SpSceneComp(const shared_ptr<GameObject>& gameObjectPtr) :
			Component(gameObjectPtr),
			m_emissive(0.0f),
			m_diffuse(1.0f),
			m_specular(0.0f),
			m_ownShadowActive(true)
		{
		}
		virtual ~SpSceneComp() {}
		Col4 GetEmissive()const {
			return m_emissive;
		}
		void SetEmissive(const Col4& col) {
			m_emissive = col;
		}
		Col4 GetDiffuse()const {
			return m_diffuse;
		}
		void SetDiffuse(const Col4& col) {
			m_diffuse = col;
		}
		Col4 GetSpecular()const {
			return m_specular;
		}
		void SetSpecular(const Col4& col) {
			m_specular = col;
		}
		bool IsOwnShadowActive()const {
			return m_ownShadowActive;
		}
		void SetOwnShadowActive(bool b) {
			m_ownShadowActive = b;
		}
		shared_ptr<BaseMesh> GetBaseMesh() const;
		void SetBaseMesh(const shared_ptr<BaseMesh>& mesh) {
			m_mesh = mesh;
		}
		shared_ptr<BaseTexture> GetBaseTexture() const;
		void SetBaseTexture(const shared_ptr<BaseTexture>& baseTexture) {
			m_texture = baseTexture;
		}
		virtual void OnUpdateConstantBuffers()override;
		virtual void OnCommitConstantBuffers()override;
		virtual void OnCreate()override;
		virtual void OnSceneDraw()override;
	};

}
// end namespace basecross
