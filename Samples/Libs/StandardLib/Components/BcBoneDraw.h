/*!
@file BcBoneDraw.h
@brief ベイシック描画コンポーネント
*/


#pragma once
#include "stdafx.h"

namespace basecross {


	DECLARE_DX12SHADER(BcVSPNTBonePL)
	DECLARE_DX12SHADER(BcVSPNTBonePLShadow)


	//--------------------------------------------------------------------------------------
	///	BcPNTBoneDrawコンポーネント
	//--------------------------------------------------------------------------------------
	class BcPNTBoneDraw : public Component {
	protected:
		BasicConstant m_constantBuffer;
		size_t m_constantBufferIndex;
		//自分自身に影を描画するかどうか
		bool m_ownShadowActive;
		//フォグが有効かどうか
		bool m_fogEnabled = true;
		//フォグの開始位置
		float m_fogStart = -10.0f;
		//フォグの終了位置
		float m_fogEnd = -40.0f;
		//フォグ色
		XMFLOAT4 m_fogColor;
		//フォグベクトル
		XMFLOAT3 m_fogVector;

		std::vector<Mat4x4>  m_BoneTransforms;

		//--------------------------------------------------------------------------------------
		/*!
		@brief	シーン描画の共通の状態（パイプラインステート、シャドウマップ、サンプラー）を設定する
		@param[in]	pCommandList	コマンドリスト
		@param[in]	alpha	透明で描くか
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetCommonDrawState(ID3D12GraphicsCommandList* pCommandList, bool alpha);

	public:
		bool IsOwnShadowActive()const {
			return m_ownShadowActive;
		}
		void SetOwnShadowActive(bool b) {
			m_ownShadowActive = b;
		}
		bool IsSetFogEnabled()const {
			return m_fogEnabled;
		}
		void SetFogEnabled(bool b) {
			m_fogEnabled = b;
		}
		//ボーン行列を設定する（外部で計算した行列、たとえばAnimatorの行列を使うとき）
		//Assimpの行列のまま（行ごとにシェーダーのBonesに渡す形）
		void SetBoneTransforms(const std::vector<Mat4x4>& transforms) {
			m_BoneTransforms = transforms;
		}
		//ボーン行列を得る
		const std::vector<Mat4x4>& GetBoneTransforms() const {
			return m_BoneTransforms;
		}



		BcPNTBoneDraw(const std::shared_ptr<GameObject>& gameObjectPtr);
		virtual ~BcPNTBoneDraw() {}
		virtual void OnUpdateConstantBuffers()override;
		virtual void OnCommitConstantBuffers()override;
		virtual void OnCreate()override;
		virtual void OnSceneDraw(ID3D12GraphicsCommandList* pCommandList)override;

		virtual bool UpdateAnimation(double animeTime);

	};

}
// end namespace basecross
