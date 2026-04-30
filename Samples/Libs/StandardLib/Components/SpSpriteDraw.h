/*!
@file SpSpriteDraw.h
@brief スプライトコンポーネント群
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	DECLARE_DX12SHADER(SpVSPCTSprite)
	DECLARE_DX12SHADER(SpPSPCTSprite)



	//--------------------------------------------------------------------------------------
	///	標準的なスプライトコンポーネント（PCT）
	//--------------------------------------------------------------------------------------
	class SpPCTSpriteDraw : public Component {
	protected:
		/// エミッシブ色
		Col4 m_emissive;
		//透明かどうか
		bool m_alphaActive;
		SpriteConstant m_spriteConstant;
		size_t m_constantIndex;
		//オリジナルメッシュ
		std::shared_ptr<BaseMesh> m_mesh;
	public:
		Col4 GetEmissive()const {
			return m_emissive;
		}
		void SetEmissive(const Col4& col) {
			m_emissive = col;
		}
		SpPCTSpriteDraw(const std::shared_ptr<GameObject>& gameObjectPtr,
			std::vector<VertexPositionColorTexture>& Vertices, std::vector<uint32_t>& indices);
		virtual ~SpPCTSpriteDraw() {}
		//初期化
		virtual void OnCreate() override;
		//操作
		virtual void OnUpdate(double elapsedTime)override {}
		virtual void OnUpdateConstantBuffers()override;
		virtual void OnCommitConstantBuffers()override;
		virtual void OnSceneDraw(ID3D12GraphicsCommandList* pCommandList)override;
		virtual void OnDestroy()override {}
	};



}
// end namespace basecross
