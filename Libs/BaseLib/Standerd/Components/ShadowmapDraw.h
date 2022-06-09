/*!
@file ShadowmapDraw.h
@brief シャドウマップ描画コンポーネント
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	Shadowmap描画コンポーネント
	//--------------------------------------------------------------------------------------
	DECLARE_DX12SHADER(PNTShadowmap)

	class Shadowmap : public Component {
		void CreatePipelineStates();
		ComPtr<ID3D12PipelineState> m_PNTPipelineState;

		static float m_lightHeight;
		static float m_lightNear;
		static float m_lightFar;
		static float m_viewWidth;
		static float m_viewHeight;
		static float m_posAdjustment;

	protected:
		//コンスタントバッファの登録
		void SetConstant(ShadowConstant& constant, const shared_ptr<Transform>& transform);
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
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

		virtual void OnInitFrame(BaseFrame* pBaseFrame)override;
		virtual void WriteConstantBuffers(BaseFrame* pBaseFrame)override;
		explicit Shadowmap(const shared_ptr<GameObject>& gameObjectPtr);
		virtual ~Shadowmap() {}
		//操作
		virtual void OnCreate()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};


}
// end namespace basecross
