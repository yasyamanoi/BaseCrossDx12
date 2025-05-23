/*!
@file Shadowmap.h
@brief シャドウマップコンポーネント
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
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
		bsm::Mat4x4 world;
		/// ビュー行列
		bsm::Mat4x4 view;
		/// 射影行列
		bsm::Mat4x4 projection;
		/// bone用
		bsm::Vec4 bones[3 * 72];
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
	class Shadowmap : public Component {
	protected:
		std::weak_ptr<BaseMesh> m_mesh;
		ShadowConstantBuffer m_shadowConstantBuffer;
		size_t m_shadowConstantBufferIndex;
	public:
		//static変数
		static float m_lightHeight;
		static float m_lightNear;
		static float m_lightFar;
		static float m_viewWidth;
		static float m_viewHeight;
		static float m_posAdjustment;

		void SetBaseMesh(const std::shared_ptr<BaseMesh>& mesh) {
			m_mesh = mesh;
		}
		Shadowmap(const std::shared_ptr<GameObject>& gameObjectPtr);
		virtual ~Shadowmap() {}
		virtual void OnUpdateConstantBuffers()override;
		virtual void OnCommitConstantBuffers()override;
		virtual void OnCreate()override;
		virtual void OnShadowDraw()override;
	};

}
// end namespace basecross

