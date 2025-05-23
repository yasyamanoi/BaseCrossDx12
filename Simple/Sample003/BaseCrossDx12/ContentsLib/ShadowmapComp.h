/*!
@file ShadowmapComp.h
@brief シャドウマップコンポーネント
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	Shadowコンスタントバッファ構造体
	//--------------------------------------------------------------------------------------
	struct ShadowConstantBuffer
	{
		/// ワールド行列
		Mat4x4 world;
		/// ビュー行列
		Mat4x4 view;
		/// 射影行列
		Mat4x4 projection;
		/// bone用
		Vec4 bones[3 * 72];
		ShadowConstantBuffer() {
			memset(this, 0, sizeof(ShadowConstantBuffer));
		};
	};

	//--------------------------------------------------------------------------------------
	///	Shadowmap描画
	//--------------------------------------------------------------------------------------
	DECLARE_DX12SHADER(PNTShadowmap)

	//--------------------------------------------------------------------------------------
	///	シャドウマップコンポーネント
	//--------------------------------------------------------------------------------------
	class ShadowmapComp : public Component {
	protected:
		std::weak_ptr<BaseMesh> m_mesh;
		ShadowConstantBuffer m_shadowConstantBuffer;
		size_t m_shadowConstantBufferIndex;
	public:
		void SetBaseMesh(const std::shared_ptr<BaseMesh>& mesh) {
			m_mesh = mesh;
		}
		ShadowmapComp(const std::shared_ptr<GameObject>& gameObjectPtr);
		virtual ~ShadowmapComp() {}
		virtual void OnUpdateConstantBuffers()override;
		virtual void OnCommitConstantBuffers()override;
		virtual void OnCreate()override;
		virtual void OnShadowDraw()override;
	};

}
// end namespace basecross

