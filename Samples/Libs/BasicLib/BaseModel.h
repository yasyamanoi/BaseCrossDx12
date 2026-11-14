/*!
@file BaseModel.h
@brief モデル（メッシュ・マテリアルごとのテクスチャ・アニメーション）クラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	モデルのマテリアル（GPU側）
	//--------------------------------------------------------------------------------------
	struct ModelMaterial {
		/// マテリアルの情報（色、透明など）
		ModelMaterialData data;
		/// テクスチャ（nullptrならテクスチャなし。色だけで描く）
		std::shared_ptr<BaseTexture> texture;
	};

	//--------------------------------------------------------------------------------------
	/// モデル。
	/// glbなどから、スキニング用のメッシュ、マテリアルごとのテクスチャを作成する。
	/// アニメーションは、別のファイルから追加で読み込める（ノード名がモデルと一致するもの）。
	/// モデルは作成後は変更しないので、複数のオブジェクトで共有できる。
	/// 再生状態はAnimatorコンポーネントが持つ。
	//--------------------------------------------------------------------------------------
	class BaseModel {
		std::shared_ptr<ModelLoader> m_loader;
		std::shared_ptr<BaseMesh> m_mesh;
		std::vector<ModelMaterial> m_materials;
		std::vector<std::shared_ptr<BaseTexture>> m_textures;
		bool m_hasAlphaBlend;
		BaseModel() :m_hasAlphaBlend(false) {}
	public:
		~BaseModel() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	モデルファイルから、モデルを作成する。
		メッシュやテクスチャをGPUへアップロードするので、コマンドリストが有効なときに呼ぶこと
		（Scene::CreateAssetResources内など）
		@param[in]	pCommandList	コマンドリスト
		@param[in]	modelFile	モデルファイル（glbなど、フルパス）
		@param[in]	options	読み込みオプション
		@return	モデル。失敗時は例外
		*/
		//--------------------------------------------------------------------------------------
		static std::shared_ptr<BaseModel> CreateFromFile(ID3D12GraphicsCommandList* pCommandList,
			const std::wstring& modelFile, const ModelLoadOptions& options = ModelLoadOptions());
		//--------------------------------------------------------------------------------------
		/*!
		@brief	アニメーションファイルからアニメーションを追加する（CPUだけの処理）
		@param[in]	animFile	アニメーションファイル（glbなど、フルパス）
		@param[in]	namePrefix	クリップ名の前に付ける文字列
		@return	追加したクリップの数
		*/
		//--------------------------------------------------------------------------------------
		size_t AddAnimationsFromFile(const std::wstring& animFile, const std::wstring& namePrefix = L"");
		//--------------------------------------------------------------------------------------
		/*!
		@brief	フォルダ内のアニメーションファイルをすべて読み込む
		@param[in]	dir	フォルダ（フルパス）
		@param[in]	pattern	ファイル名のパターン
		@return	追加したクリップの数
		*/
		//--------------------------------------------------------------------------------------
		size_t AddAnimationsFromDirectory(const std::wstring& dir, const std::wstring& pattern = L"*.glb");
		//--------------------------------------------------------------------------------------
		/*!
		@brief	アニメーションイベントを追加する（時刻は秒）
		@return	クリップがあって追加できたらtrue
		*/
		//--------------------------------------------------------------------------------------
		bool AddAnimationEvent(const std::wstring& clipName, double timeSec, const std::wstring& eventName, const std::wstring& param = L"") {
			return m_loader->AddAnimationEvent(clipName, timeSec, eventName, param);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	テキストファイルからアニメーションイベントを読み込む。
		形式は、1行に「クリップ名<TAB>時刻<TAB>イベント名[<TAB>パラメータ]」（時刻は秒、末尾に%で割合）
		@param[in]	textFile	テキストファイル
		@return	追加したイベントの数
		*/
		//--------------------------------------------------------------------------------------
		size_t LoadAnimationEvents(const std::wstring& textFile);
		//メッシュ（VertexPositionNormalTextureSkinning。全パーツをまとめたもの）
		const std::shared_ptr<BaseMesh>& GetMesh() const {
			return m_mesh;
		}
		//サブセット（マテリアルごとのインデックスの範囲）
		const std::vector<ModelSubset>& GetSubsets() const {
			return m_loader->GetSubsets();
		}
		//マテリアル
		const std::vector<ModelMaterial>& GetMaterials() const {
			return m_materials;
		}
		//モデルが持つテクスチャ（重複なし）
		const std::vector<std::shared_ptr<BaseTexture>>& GetTextures() const {
			return m_textures;
		}
		//透明（BLEND）のマテリアルがあるか
		bool HasAlphaBlendMaterial() const {
			return m_hasAlphaBlend;
		}
		//ローダー（ボーン、ノード、クリップ、姿勢の計算）
		ModelLoader& GetLoader() {
			return *m_loader;
		}
		const ModelLoader& GetLoader() const {
			return *m_loader;
		}
		//アニメーションがあるか
		bool HasClip(const std::wstring& clipName) const {
			return m_loader->HasClip(clipName);
		}
		//アニメーションの名前の一覧
		std::vector<std::wstring> GetClipNames() const {
			return m_loader->GetClipNames();
		}
		//アニメーションの長さ（秒）。無ければ0
		double GetClipDuration(const std::wstring& clipName) const {
			return m_loader->GetClipDuration(clipName);
		}
	};

}
//end basecross
