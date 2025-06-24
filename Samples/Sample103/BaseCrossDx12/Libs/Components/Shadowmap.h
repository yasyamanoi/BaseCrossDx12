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
	class Shadowmap : public Component {
		static float m_lightHeight;
		static float m_lightNear;
		static float m_lightFar;
		static float m_viewWidth;
		static float m_viewHeight;
		static float m_posAdjustment;
	protected:
		ShadowConstantBuffer m_shadowConstantBuffer;
		size_t m_shadowConstantBufferIndex;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ライトの高さを得る
		@return	ライトの高さ
		*/
		//--------------------------------------------------------------------------------------
		static float GetLightHeight() {
			return m_lightHeight;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ライトのNear値を得る
		@return	ライトのNear値
		*/
		//--------------------------------------------------------------------------------------
		static float GetLightNear() {
			return m_lightNear;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ライトのFar値を得る
		@return	ライトのFar値
		*/
		//--------------------------------------------------------------------------------------
		static float GetLightFar() {
			return m_lightFar;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ビューの幅を得る
		@return	ビューの幅
		*/
		//--------------------------------------------------------------------------------------
		static float GetViewWidth() {
			return m_viewWidth;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ビューの高さを得る
		@return	ビューの高さ
		*/
		//--------------------------------------------------------------------------------------
		static float GetViewHeight() {
			return m_viewHeight;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	影の位置調整値（Y方向）を得る
		@return	影の位置調整値（Y方向）
		*/
		//--------------------------------------------------------------------------------------
		static float GetPosAdjustment() {
			return m_posAdjustment;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ライトの高さを設定する
		@param[in]	f	高さ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		static void SetLightHeight(float f) {
			m_lightHeight = f;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ライトのNear値を設定する
		@param[in]	f	Near値
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		static void SetLightNear(float f) {
			m_lightNear = f;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ライトのFar値を設定する
		@param[in]	f	Far値
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		static void SetLightFar(float f) {
			m_lightFar = f;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ビューの幅を設定する
		@param[in]	f	ビューの幅
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		static void SetViewWidth(float f) {
			m_viewWidth = f;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ビューの高さを設定する
		@param[in]	f	ビューの高さ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		static void SetViewHeight(float f) {
			m_viewHeight = f;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ビューの幅及び高さを同じ値に設定する
		@param[in]	f	ビューの幅及び高さ（同値）
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		static void SetViewSize(float f) {
			m_viewWidth = m_viewHeight = f;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	影の位置調整値（Y方向）を設定する
		@param[in]	f	影の位置調整値（Y方向）
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		static void SetPosAdjustment(float f) {
			m_posAdjustment = f;
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

