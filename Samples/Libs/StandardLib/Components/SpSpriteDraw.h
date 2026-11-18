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
		/// ディフューズ色（頂点色とは別に、スプライト全体にかかる色と透明度）
		Col4 m_diffuse;
		//透明かどうか
		bool m_alphaActive;
		SpriteConstant m_spriteConstant;
		size_t m_constantIndex;
		//頂点の配列（毎フレーム、現在のフレームのバッファへ書き写す）
		std::vector<VertexPositionColorTexture> m_vertices;
		//フレームごとの頂点バッファ（UPLOADヒープ、Mapしたまま）
		ComPtr<ID3D12Resource> m_vertexBuffers[BaseDevice::FrameCount];
		//Mapした書き込み専用ポインタ
		UINT8* m_pVertexBufferWOs[BaseDevice::FrameCount];
		//フレームごとの頂点バッファビュー
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferViews[BaseDevice::FrameCount];
		//インデックスバッファ（UPLOADヒープ、作成時に1回書くだけ）
		ComPtr<ID3D12Resource> m_indexBuffer;
		D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
		//インデックスの数
		UINT m_numIndices;
		//現在のフレーム番号を得る
		size_t GetCurrentFrameIndex()const;
	public:
		Col4 GetEmissive()const {
			return m_emissive;
		}
		void SetEmissive(const Col4& col) {
			m_emissive = col;
		}
		//ディフューズ色の取得
		Col4 GetDiffuse()const {
			return m_diffuse;
		}
		//ディフューズ色の設定
		void SetDiffuse(const Col4& col) {
			m_diffuse = col;
		}
		//頂点の取得
		const std::vector<VertexPositionColorTexture>& GetVertices()const {
			return m_vertices;
		}
		//頂点の設定（頂点の数は作成時と同じでなければならない）
		void SetVertices(const std::vector<VertexPositionColorTexture>& vertices);
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
