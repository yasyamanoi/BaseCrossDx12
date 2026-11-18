/*!
@file BcModelDraw.h
@brief モデル描画コンポーネント（マテリアルごとのテクスチャ、アニメーション対応）
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	BcPNTModelDrawコンポーネント。
	///	BaseModelを描画する。マテリアル（サブセット）ごとに、テクスチャ・色・透明を切り替えて描く。
	///	ボーン行列は、同じオブジェクトのAnimatorコンポーネントがあればそれを使う。
	///	無ければ、SetBoneTransformsで設定した行列（無ければ初期姿勢）を使う。
	///	テクスチャなしのマテリアルは、マテリアルの色（diffuse、emissive）で描く。
	///	透明（BLEND）のマテリアルは、不透明のマテリアルを描いた後に描く。
	//--------------------------------------------------------------------------------------
	class BcPNTModelDraw : public BcPNTBoneDraw {
		std::shared_ptr<BaseModel> m_model;
		//マテリアルごとのコンスタントバッファ
		std::vector<size_t> m_materialBufferIndex;
		std::vector<BasicConstant> m_materialConstants;
		//マテリアルごとの上書き
		std::vector<std::shared_ptr<BaseTexture>> m_textureOverride;
		std::vector<bool> m_diffuseOverrideActive;
		std::vector<Col4> m_diffuseOverride;
		//マテリアルの数だけコンスタントバッファを用意する
		void EnsureMaterialBuffers(size_t count);
	public:
		BcPNTModelDraw(const std::shared_ptr<GameObject>& gameObjectPtr);
		virtual ~BcPNTModelDraw() {}
		//モデルを、Sceneに登録したキーで設定する
		void SetModel(const std::wstring& key);
		//モデルを設定する。メッシュも設定される
		void SetModel(const std::shared_ptr<BaseModel>& model);
		//モデルを得る
		const std::shared_ptr<BaseModel>& GetModel() const {
			return m_model;
		}
		//マテリアルのテクスチャを上書きする（nullptrで上書きを解除）
		void SetMaterialTexture(size_t materialIndex, const std::shared_ptr<BaseTexture>& texture);
		//マテリアルの拡散色を上書きする
		void SetMaterialDiffuse(size_t materialIndex, const Col4& color);
		//上書きをすべて解除する
		void ClearMaterialOverrides();
		virtual void OnUpdateConstantBuffers()override;
		virtual void OnCommitConstantBuffers()override;
		virtual void OnSceneDraw(ID3D12GraphicsCommandList* pCommandList)override;
	};

}
// end namespace basecross
