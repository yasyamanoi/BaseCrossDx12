/*!
@file SpStaticDraw.h
@brief Simpleスタティック描画コンポーネント群
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	SpStatic描画コンポーネント(親)
	//--------------------------------------------------------------------------------------
	class SpStaticDraw : public Component {
		/// エミッシブ色
		Col4 m_emissive;
		/// デフューズ色
		Col4 m_diffuse;
		/// スペキュラー
		Col4 m_specular;
		//自分自身に影を描画するかどうか
		bool m_ownShadowActive;
	protected:
		SimpleConstant m_simpleConstant;
		size_t m_constantIndex;
		//オリジナルメッシュ
		std::shared_ptr<BaseMesh> m_mesh;
		explicit SpStaticDraw(const std::shared_ptr<GameObject>& gameObjectPtr) :
			Component(gameObjectPtr),
			m_emissive(0.0f),
			m_diffuse(1.0f),
			m_specular(0.0f),
			m_ownShadowActive(false)
		{
		}
		virtual ~SpStaticDraw() {}
	public:
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
		virtual void OnCreate()override;
	};




	//--------------------------------------------------------------------------------------
	///	SpPNTStatic描画コンポーネント
	//--------------------------------------------------------------------------------------

	DECLARE_DX12SHADER(SpVSPNTStatic)
	DECLARE_DX12SHADER(SpVSPNTStaticShadow)

	DECLARE_DX12SHADER(SpPSPNTStatic)
	DECLARE_DX12SHADER(SpPSPNTStaticShadow)

	class SpPNTStaticDraw : public SpStaticDraw {
	protected:
	public:
		explicit SpPNTStaticDraw(const std::shared_ptr<GameObject>& gameObjectPtr);
		SpPNTStaticDraw(const std::shared_ptr<GameObject>& gameObjectPtr,
			std::vector<VertexPositionNormalTexture>& vertices, std::vector<uint32_t>& indices);
		virtual ~SpPNTStaticDraw() {}
		//操作
		virtual void OnUpdate(double elapsedTime)override {}
		virtual void OnUpdateConstantBuffers()override;
		virtual void OnCommitConstantBuffers()override;
		virtual void OnSceneDraw()override;
		virtual void OnDestroy()override {}
	};



}
// end namespace basecross
