/*!
@file ModelLoader.h
@brief モデル（メッシュ・マテリアル・ボーン）とアニメーションの読み込み、ポーズの計算
@note AssimpとSTLだけに依存します（Direct3Dには依存しません）。
モデルのglbなどから、メッシュ・マテリアル・ボーン階層を読み込みます。
アニメーションは、モデルとは別のファイルから読み込めます。ノード名（ボーン名）で一致させて反映します。
読み込み時にDirectXの左手座標系に変換します。
ボーン行列は、Assimpの行列のまま（列ベクトルに掛ける形）で出力します。
これはシェーダー（float4x3のBones）に、行ごとに渡す向きです。
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once

#include <windows.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/GltfMaterial.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace basecross {

	//--------------------------------------------------------------------------------------
	// 文字列変換（モデルの名前は内部ではUTF-8、公開APIではwstring）
	//--------------------------------------------------------------------------------------
	namespace ModelUtil {
		//UTF-8からwstring
		inline std::wstring ToWide(const std::string& src) {
			if (src.empty()) {
				return std::wstring();
			}
			int len = ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), static_cast<int>(src.size()), nullptr, 0);
			std::wstring dest(len, L'\0');
			::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), static_cast<int>(src.size()), &dest[0], len);
			return dest;
		}
		//wstringからUTF-8
		inline std::string ToUtf8(const std::wstring& src) {
			if (src.empty()) {
				return std::string();
			}
			int len = ::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), static_cast<int>(src.size()), nullptr, 0, nullptr, nullptr);
			std::string dest(len, '\0');
			::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), static_cast<int>(src.size()), &dest[0], len, nullptr, nullptr);
			return dest;
		}
		//wstringからシステムの文字コード（ファイルパス用。BaseAssimpと同じ扱い）
		inline std::string ToAnsi(const std::wstring& src) {
			if (src.empty()) {
				return std::string();
			}
			int len = ::WideCharToMultiByte(CP_ACP, 0, src.c_str(), static_cast<int>(src.size()), nullptr, 0, nullptr, nullptr);
			std::string dest(len, '\0');
			::WideCharToMultiByte(CP_ACP, 0, src.c_str(), static_cast<int>(src.size()), &dest[0], len, nullptr, nullptr);
			return dest;
		}
		//デバッグ出力
		inline void Warn(const std::wstring& msg) {
			::OutputDebugStringW((L"[ModelLoader] " + msg + L"\n").c_str());
		}
	}

	//--------------------------------------------------------------------------------------
	// 読み込みオプション
	//--------------------------------------------------------------------------------------
	struct ModelLoadOptions {
		//DirectXの左手座標系に変換する（Z軸反転・面の向きの反転・UVの扱いを含む）
		bool convertToLeftHanded = true;
		//テクスチャの情報（埋め込み画像のデータなど）を読み込む
		bool loadTextures = true;
	};

	//--------------------------------------------------------------------------------------
	// スキニング用の頂点。位置と法線は、初期姿勢（バインドポーズ）のもの
	//--------------------------------------------------------------------------------------
	struct ModelSkinVertex {
		float position[3];
		float normal[3];
		float uv[2];
		uint32_t bone[4];
		float weight[4];
	};

	//--------------------------------------------------------------------------------------
	// テクスチャの出どころ。aiSceneを解放した後も使えるように、データをコピーして持つ
	//--------------------------------------------------------------------------------------
	struct ModelTextureSource {
		enum class Type {
			None,
			EmbeddedCompressed,	///< 埋め込み。PNGやJPGなどのファイルの中身
			EmbeddedRaw,		///< 埋め込み。B8G8R8A8の生ピクセル
			File				///< 外部ファイル
		};
		Type type = Type::None;
		std::wstring name;				///< モデルの中での名前（"*0"や、ファイル名）
		std::vector<uint8_t> bytes;		///< 埋め込みのデータ
		std::string formatHint;			///< "png","jpg"など
		uint32_t width = 0;				///< EmbeddedRawの幅
		uint32_t height = 0;			///< EmbeddedRawの高さ
		std::wstring filePath;			///< Fileのパス（モデルと同じフォルダからの相対を解決済み）
	};

	//--------------------------------------------------------------------------------------
	// マテリアル
	//--------------------------------------------------------------------------------------
	struct ModelMaterialData {
		std::wstring name;
		float diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float emissive[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		bool alphaBlend = false;		///< glTFのalphaMode==BLEND（または不透明度<1）
		bool alphaMask = false;			///< glTFのalphaMode==MASK（今は不透明として描く）
		int textureIndex = -1;			///< ModelLoader::GetTextures()の番号。-1はテクスチャなし
	};

	//--------------------------------------------------------------------------------------
	// サブセット（同じマテリアルのインデックスの連続範囲）
	//--------------------------------------------------------------------------------------
	struct ModelSubset {
		uint32_t startIndex = 0;
		uint32_t indexCount = 0;
		uint32_t materialIndex = 0;
	};

	//--------------------------------------------------------------------------------------
	// ローカル姿勢（ノードごとの位置・回転・スケール）
	//--------------------------------------------------------------------------------------
	struct NodeTRS {
		aiVector3D t;
		aiQuaternion r;
		aiVector3D s = aiVector3D(1.0f, 1.0f, 1.0f);
	};
	struct LocalPose {
		std::vector<NodeTRS> nodes;
	};

	//--------------------------------------------------------------------------------------
	// アニメーション
	//--------------------------------------------------------------------------------------
	struct ModelVecKey { double time; aiVector3D value; };
	struct ModelQuatKey { double time; aiQuaternion value; };
	struct ModelChannel {
		int node = -1;
		std::vector<ModelVecKey> positions;
		std::vector<ModelQuatKey> rotations;
		std::vector<ModelVecKey> scalings;
	};
	//アニメーションイベント（glbには入っていないので、外部で定義する）
	struct AnimationEvent {
		double time = 0.0;			///< クリップ内の時刻（秒）
		std::wstring name;
		std::wstring param;
	};
	struct AnimationClip {
		std::wstring name;
		double duration = 0.0;		///< 秒。0は1キーだけの静止ポーズ（T-Poseなど）
		std::vector<ModelChannel> channels;
		std::vector<AnimationEvent> events;	///< 時刻順
	};

	//--------------------------------------------------------------------------------------
	// ルートモーション
	//--------------------------------------------------------------------------------------
	enum class RootMotionMode {
		AsAuthored,	///< データのまま再生する（移動も姿勢に含まれたまま）
		InPlace,	///< 移動ノードの指定軸の移動を姿勢から取り除いて捨てる（その場再生）
		Extract		///< 取り除いた移動を、移動量として返す（Transformへの反映は呼び出し側）
	};
	struct RootMotionSettings {
		RootMotionMode mode = RootMotionMode::AsAuthored;
		std::wstring motionNode = L"root";		///< 移動を読み取るノード（KayKitのDeathなどはL"hips"）
		bool axisX = true;						///< 抽出する軸（モデル空間）
		bool axisY = false;
		bool axisZ = true;
		bool extractYaw = false;				///< Y軸まわりの回転も抽出する
	};
	struct RootMotionDelta {
		aiVector3D translation;					///< モデル空間での移動量
		float yaw = 0.0f;						///< Y軸まわりの回転量（ラジアン）
	};

	class ModelLoader;

	//--------------------------------------------------------------------------------------
	// ボーンマスク（ノードごとの重み0..1）
	//--------------------------------------------------------------------------------------
	class BoneMask {
		std::vector<float> m_weights;
	public:
		BoneMask() {}
		//全ノードを同じ重みにする
		static BoneMask All(const ModelLoader& model, float weight = 1.0f);
		//指定ノードとその子孫だけを重みにする（ほかは0）
		static BoneMask Subtree(const ModelLoader& model, const std::wstring& nodeName, float weight = 1.0f);
		//ノード（とその子孫）の重みを設定する
		BoneMask& Include(const ModelLoader& model, const std::wstring& nodeName, bool recursive = true, float weight = 1.0f);
		//ノード（とその子孫）の重みを0にする
		BoneMask& Exclude(const ModelLoader& model, const std::wstring& nodeName, bool recursive = true);
		//ノードの重み（範囲外は1）
		float Get(size_t nodeIndex) const {
			return nodeIndex < m_weights.size() ? m_weights[nodeIndex] : 1.0f;
		}
		bool Empty() const {
			return m_weights.empty();
		}
		friend class ModelLoader;
	};

	//--------------------------------------------------------------------------------------
	// モデルローダー
	//--------------------------------------------------------------------------------------
	class ModelLoader {
	public:
		static const size_t kMaxBones = 200;
	private:
		//ノード
		struct Node {
			std::string name;
			int parent = -1;
			NodeTRS rest;						//初期姿勢のローカルTRS
			aiMatrix4x4 restLocal;
			aiMatrix4x4 restGlobal;
			aiMatrix4x4 invRestGlobal;
		};
		std::vector<Node> m_nodes;
		std::map<std::string, int> m_nodeIndex;
		std::vector<int> m_boneNode;			//ボーン番号 → ノード番号
		std::vector<ModelSkinVertex> m_vertices;
		std::vector<uint32_t> m_indices;
		std::vector<ModelSubset> m_subsets;
		std::vector<ModelMaterialData> m_materials;
		std::vector<ModelTextureSource> m_textures;
		std::map<std::wstring, AnimationClip> m_clips;
		LocalPose m_restPose;
		bool m_leftHanded = true;
		std::wstring m_modelDir;

		static aiVector3D SampleVec(const std::vector<ModelVecKey>& keys, double t, const aiVector3D& def) {
			if (keys.empty()) {
				return def;
			}
			if (keys.size() == 1 || t <= keys.front().time) {
				return keys.front().value;
			}
			if (t >= keys.back().time) {
				return keys.back().value;
			}
			//t を含む区間を二分探索で求める
			auto it = std::upper_bound(keys.begin(), keys.end(), t,
				[](double v, const ModelVecKey& k) { return v < k.time; });
			size_t i = static_cast<size_t>(std::distance(keys.begin(), it)) - 1;
			const double span = keys[i + 1].time - keys[i].time;
			const float f = span > 0.0 ? static_cast<float>((t - keys[i].time) / span) : 0.0f;
			return keys[i].value + (keys[i + 1].value - keys[i].value) * f;
		}
		static aiQuaternion SampleQuat(const std::vector<ModelQuatKey>& keys, double t, const aiQuaternion& def) {
			if (keys.empty()) {
				return def;
			}
			if (keys.size() == 1 || t <= keys.front().time) {
				return keys.front().value;
			}
			if (t >= keys.back().time) {
				return keys.back().value;
			}
			auto it = std::upper_bound(keys.begin(), keys.end(), t,
				[](double v, const ModelQuatKey& k) { return v < k.time; });
			size_t i = static_cast<size_t>(std::distance(keys.begin(), it)) - 1;
			const double span = keys[i + 1].time - keys[i].time;
			const float f = span > 0.0 ? static_cast<float>((t - keys[i].time) / span) : 0.0f;
			aiQuaternion out;
			aiQuaternion::Interpolate(out, keys[i].value, keys[i + 1].value, f);
			return out.Normalize();
		}
		//Y軸まわりの回転角（ねじれ成分。q = swing * twist の twist）
		static float YawOf(const aiQuaternion& q) {
			return 2.0f * std::atan2(q.y, q.w);
		}
		static aiQuaternion TwistOf(const aiQuaternion& q) {
			aiQuaternion t(q.w, 0.0f, q.y, 0.0f);
			const float len = std::sqrt(t.w * t.w + t.y * t.y);
			if (len < 1e-8f) {
				return aiQuaternion();
			}
			t.w /= len;
			t.y /= len;
			return t;
		}
		//角度を-π..πに収める
		static float WrapAngle(float a) {
			const float pi = 3.14159265358979f;
			while (a > pi) {
				a -= 2.0f * pi;
			}
			while (a <= -pi) {
				a += 2.0f * pi;
			}
			return a;
		}
		//ノードの階層を、親が先になる順で集める
		void CollectNodes(const aiNode* node, int parent, std::vector<int>& nodeOfMesh) {
			const int index = static_cast<int>(m_nodes.size());
			Node n;
			n.name = node->mName.C_Str();
			n.parent = parent;
			n.restLocal = node->mTransformation;
			n.restGlobal = parent >= 0 ? m_nodes[parent].restGlobal * node->mTransformation : node->mTransformation;
			node->mTransformation.Decompose(n.rest.s, n.rest.r, n.rest.t);
			m_nodes.push_back(n);
			m_nodeIndex[n.name] = index;
			for (unsigned int i = 0; i < node->mNumMeshes; i++) {
				if (node->mMeshes[i] >= nodeOfMesh.size()) {
					nodeOfMesh.resize(node->mMeshes[i] + 1, -1);
				}
				nodeOfMesh[node->mMeshes[i]] = index;
			}
			for (unsigned int i = 0; i < node->mNumChildren; i++) {
				CollectNodes(node->mChildren[i], index, nodeOfMesh);
			}
		}
		int GetBoneIndex(int nodeIndex) {
			for (size_t b = 0; b < m_boneNode.size(); b++) {
				if (m_boneNode[b] == nodeIndex) {
					return static_cast<int>(b);
				}
			}
			m_boneNode.push_back(nodeIndex);
			return static_cast<int>(m_boneNode.size() - 1);
		}
		//マテリアルのテクスチャを探して、重複しないように登録する
		int RegisterTexture(const aiScene* pScene, const aiMaterial* pMaterial) {
			aiString texPath;
			bool found = pMaterial->GetTexture(aiTextureType_BASE_COLOR, 0, &texPath) == AI_SUCCESS;
			if (!found) {
				found = pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS;
			}
			if (!found) {
				return -1;
			}
			const std::wstring name = ModelUtil::ToWide(texPath.C_Str());
			for (size_t i = 0; i < m_textures.size(); i++) {
				if (m_textures[i].name == name) {
					return static_cast<int>(i);
				}
			}
			ModelTextureSource src;
			src.name = name;
			const aiTexture* pTexture = pScene->GetEmbeddedTexture(texPath.C_Str());
			if (pTexture) {
				if (pTexture->mHeight == 0) {
					//PNGやJPGなどのファイルの中身がそのまま入っている
					const uint8_t* p = reinterpret_cast<const uint8_t*>(pTexture->pcData);
					src.type = ModelTextureSource::Type::EmbeddedCompressed;
					src.bytes.assign(p, p + pTexture->mWidth);
					src.formatHint = pTexture->achFormatHint;
				}
				else {
					//生のピクセル（aiTexelは b,g,r,a の順）
					const uint8_t* p = reinterpret_cast<const uint8_t*>(pTexture->pcData);
					src.type = ModelTextureSource::Type::EmbeddedRaw;
					src.width = pTexture->mWidth;
					src.height = pTexture->mHeight;
					src.bytes.assign(p, p + static_cast<size_t>(pTexture->mWidth) * pTexture->mHeight * 4);
				}
			}
			else {
				//外部ファイル（モデルと同じフォルダからの相対）
				src.type = ModelTextureSource::Type::File;
				std::wstring rel = name;
				for (auto& c : rel) {
					if (c == L'/') {
						c = L'\\';
					}
				}
				const bool absolute = rel.size() > 1 && (rel[1] == L':' || rel[0] == L'\\');
				src.filePath = absolute ? rel : m_modelDir + rel;
			}
			m_textures.push_back(src);
			return static_cast<int>(m_textures.size() - 1);
		}
		//マテリアルを読み込む
		void LoadMaterial(const aiScene* pScene, const aiMaterial* pMaterial, bool loadTextures, ModelMaterialData& out) {
			aiString name;
			if (pMaterial->Get(AI_MATKEY_NAME, name) == AI_SUCCESS) {
				out.name = ModelUtil::ToWide(name.C_Str());
			}
			aiColor4D color;
			if (pMaterial->Get(AI_MATKEY_BASE_COLOR, color) == AI_SUCCESS ||
				pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
				out.diffuse[0] = color.r;
				out.diffuse[1] = color.g;
				out.diffuse[2] = color.b;
				out.diffuse[3] = color.a;
			}
			aiColor3D emissive;
			if (pMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive) == AI_SUCCESS) {
				out.emissive[0] = emissive.r;
				out.emissive[1] = emissive.g;
				out.emissive[2] = emissive.b;
			}
			//アルファの扱い
			float opacity = 1.0f;
			pMaterial->Get(AI_MATKEY_OPACITY, opacity);
			aiString alphaMode;
			if (pMaterial->Get(AI_MATKEY_GLTF_ALPHAMODE, alphaMode) == AI_SUCCESS) {
				const std::string mode = alphaMode.C_Str();
				out.alphaBlend = (mode == "BLEND");
				out.alphaMask = (mode == "MASK");
			}
			else {
				out.alphaBlend = (opacity < 1.0f || out.diffuse[3] < 1.0f);
			}
			if (!out.alphaBlend) {
				//BLENDでなければ、不透明として扱う
				out.diffuse[3] = 1.0f;
			}
			if (loadTextures) {
				out.textureIndex = RegisterTexture(pScene, pMaterial);
			}
		}
		//イベントを時刻順に整列する
		static void SortEvents(AnimationClip& clip) {
			std::stable_sort(clip.events.begin(), clip.events.end(),
				[](const AnimationEvent& a, const AnimationEvent& b) { return a.time < b.time; });
		}
		//指定ノードのクリップ内の位置（その時刻をラップ済みとして）
		aiVector3D MotionLocalPos(const AnimationClip& clip, int node, double t) const {
			for (auto& ch : clip.channels) {
				if (ch.node == node) {
					return SampleVec(ch.positions, t, m_nodes[node].rest.t);
				}
			}
			return m_nodes[node].rest.t;
		}
		aiQuaternion MotionLocalRot(const AnimationClip& clip, int node, double t) const {
			for (auto& ch : clip.channels) {
				if (ch.node == node) {
					return SampleQuat(ch.rotations, t, m_nodes[node].rest.r);
				}
			}
			return m_nodes[node].rest.r;
		}
		//親のレスト姿勢の回転・スケール部分（ローカル位置をモデル空間の向きにする）
		aiMatrix3x3 ParentLinear(int node) const {
			const int p = m_nodes[node].parent;
			return p >= 0 ? aiMatrix3x3(m_nodes[p].restGlobal) : aiMatrix3x3();
		}
		//クリップ先頭からの累積の移動量（ローカル空間）。時間は巻き戻しなしの連続した値
		aiVector3D AccumLocalMove(const AnimationClip& clip, int node, double tU, bool loop) const {
			const double d = clip.duration;
			const aiVector3D p0 = MotionLocalPos(clip, node, 0.0);
			if (d <= 0.0) {
				return aiVector3D(0.0f, 0.0f, 0.0f);
			}
			if (loop) {
				const double k = std::floor(tU / d);
				const double tw = tU - k * d;
				const aiVector3D loopMove = MotionLocalPos(clip, node, d) - p0;
				return loopMove * static_cast<float>(k) + (MotionLocalPos(clip, node, tw) - p0);
			}
			const double tc = (std::max)(0.0, (std::min)(tU, d));
			return MotionLocalPos(clip, node, tc) - p0;
		}
		float AccumYaw(const AnimationClip& clip, int node, double tU, bool loop) const {
			const double d = clip.duration;
			if (d <= 0.0) {
				return 0.0f;
			}
			const float y0 = YawOf(MotionLocalRot(clip, node, 0.0));
			if (loop) {
				const double k = std::floor(tU / d);
				const double tw = tU - k * d;
				const float loopYaw = WrapAngle(YawOf(MotionLocalRot(clip, node, d)) - y0);
				return loopYaw * static_cast<float>(k) + WrapAngle(YawOf(MotionLocalRot(clip, node, tw)) - y0);
			}
			const double tc = (std::max)(0.0, (std::min)(tU, d));
			return WrapAngle(YawOf(MotionLocalRot(clip, node, tc)) - y0);
		}
	public:
		ModelLoader() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	モデルファイルからメッシュ・マテリアル・ボーン階層を読み込む
		@param[in]	modelFile	モデルファイル（glbなど）
		@param[in]	options	読み込みオプション
		@param[out]	errorText	失敗したときの理由
		@return	成功したらtrue
		*/
		//--------------------------------------------------------------------------------------
		bool LoadModel(const std::wstring& modelFile, const ModelLoadOptions& options, std::wstring& errorText) {
			Assimp::Importer importer;
			unsigned int flags = aiProcess_Triangulate |
				aiProcess_JoinIdenticalVertices |
				aiProcess_GenSmoothNormals |
				aiProcess_LimitBoneWeights;
			if (options.convertToLeftHanded) {
				flags |= aiProcess_ConvertToLeftHanded;
			}
			const aiScene* pScene = importer.ReadFile(ModelUtil::ToAnsi(modelFile), flags);
			if (!pScene || !pScene->mRootNode || pScene->mNumMeshes == 0) {
				errorText = ModelUtil::ToWide(importer.GetErrorString());
				if (errorText.empty()) {
					errorText = L"no meshes";
				}
				return false;
			}
			m_leftHanded = options.convertToLeftHanded;
			m_nodes.clear();
			m_nodeIndex.clear();
			m_boneNode.clear();
			m_vertices.clear();
			m_indices.clear();
			m_subsets.clear();
			m_materials.clear();
			m_textures.clear();
			m_clips.clear();
			{
				const size_t pos = modelFile.find_last_of(L"\\/");
				m_modelDir = pos == std::wstring::npos ? std::wstring() : modelFile.substr(0, pos + 1);
			}
			std::vector<int> nodeOfMesh;
			CollectNodes(pScene->mRootNode, -1, nodeOfMesh);
			nodeOfMesh.resize(pScene->mNumMeshes, -1);
			m_restPose.nodes.resize(m_nodes.size());
			for (size_t i = 0; i < m_nodes.size(); i++) {
				aiMatrix4x4 inv = m_nodes[i].restGlobal;
				m_nodes[i].invRestGlobal = inv.Inverse();
				m_restPose.nodes[i] = m_nodes[i].rest;
			}
			//マテリアル
			m_materials.resize(pScene->mNumMaterials);
			for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
				LoadMaterial(pScene, pScene->mMaterials[i], options.loadTextures, m_materials[i]);
			}
			//メッシュのボーンになっているノード
			std::vector<bool> isBoneNode(m_nodes.size(), false);
			for (unsigned int m = 0; m < pScene->mNumMeshes; m++) {
				for (unsigned int b = 0; b < pScene->mMeshes[m]->mNumBones; b++) {
					auto it = m_nodeIndex.find(pScene->mMeshes[m]->mBones[b]->mName.C_Str());
					if (it != m_nodeIndex.end()) {
						isBoneNode[it->second] = true;
					}
				}
			}
			//メッシュをマテリアル順に並べる（マテリアルごとに連続したインデックス範囲にするため）
			std::vector<unsigned int> order(pScene->mNumMeshes);
			for (unsigned int m = 0; m < pScene->mNumMeshes; m++) {
				order[m] = m;
			}
			std::stable_sort(order.begin(), order.end(), [&](unsigned int a, unsigned int b) {
				return pScene->mMeshes[a]->mMaterialIndex < pScene->mMeshes[b]->mMaterialIndex;
			});
			for (unsigned int m : order) {
				const aiMesh* pMesh = pScene->mMeshes[m];
				const uint32_t baseVertex = static_cast<uint32_t>(m_vertices.size());
				const uint32_t startIndex = static_cast<uint32_t>(m_indices.size());
				//頂点ごとの影響ボーン
				struct Influence { int bone; float weight; aiMatrix4x4 skin; };
				std::vector<std::vector<Influence>> influences(pMesh->mNumVertices);
				for (unsigned int b = 0; b < pMesh->mNumBones; b++) {
					const aiBone* pBone = pMesh->mBones[b];
					auto it = m_nodeIndex.find(pBone->mName.C_Str());
					if (it == m_nodeIndex.end()) {
						continue;
					}
					const int boneIndex = GetBoneIndex(it->second);
					//初期姿勢での、メッシュ空間からモデル空間への変換
					const aiMatrix4x4 skin = m_nodes[it->second].restGlobal * pBone->mOffsetMatrix;
					for (unsigned int w = 0; w < pBone->mNumWeights; w++) {
						influences[pBone->mWeights[w].mVertexId].push_back({ boneIndex, pBone->mWeights[w].mWeight, skin });
					}
				}
				//ボーンの無い頂点は、そのメッシュが属するノード自身を（疑似）ボーンにする。
				//ノードの階層のアニメーションに追従する
				const int meshNode = nodeOfMesh[m] >= 0 ? nodeOfMesh[m] : 0;
				const aiMatrix4x4 nodeGlobal = m_nodes[meshNode].restGlobal;
				int attachBone = -1;
				for (unsigned int v = 0; v < pMesh->mNumVertices; v++) {
					const aiVector3D srcPos = pMesh->mVertices[v];
					const aiVector3D srcNormal = pMesh->HasNormals() ? pMesh->mNormals[v] : aiVector3D(0.0f, 1.0f, 0.0f);
					auto& infl = influences[v];
					std::sort(infl.begin(), infl.end(), [](const Influence& a, const Influence& b) { return a.weight > b.weight; });
					if (infl.size() > 4) {
						infl.resize(4);
					}
					float total = 0.0f;
					for (auto& i : infl) {
						total += i.weight;
					}
					ModelSkinVertex sv = {};
					aiVector3D pos(0.0f, 0.0f, 0.0f);
					aiVector3D normal(0.0f, 0.0f, 0.0f);
					if (total > 1e-6f) {
						for (size_t i = 0; i < infl.size(); i++) {
							const float rate = infl[i].weight / total;
							pos += (infl[i].skin * srcPos) * rate;
							normal += (aiMatrix3x3(infl[i].skin) * srcNormal) * rate;
							sv.bone[i] = static_cast<uint32_t>(infl[i].bone);
							sv.weight[i] = rate;
						}
					}
					else {
						if (attachBone < 0) {
							attachBone = GetBoneIndex(meshNode);
						}
						pos = nodeGlobal * srcPos;
						normal = aiMatrix3x3(nodeGlobal) * srcNormal;
						sv.bone[0] = static_cast<uint32_t>(attachBone);
						sv.weight[0] = 1.0f;
					}
					normal.NormalizeSafe();
					sv.position[0] = pos.x;
					sv.position[1] = pos.y;
					sv.position[2] = pos.z;
					sv.normal[0] = normal.x;
					sv.normal[1] = normal.y;
					sv.normal[2] = normal.z;
					if (pMesh->HasTextureCoords(0)) {
						sv.uv[0] = pMesh->mTextureCoords[0][v].x;
						sv.uv[1] = pMesh->mTextureCoords[0][v].y;
					}
					m_vertices.push_back(sv);
				}
				for (unsigned int f = 0; f < pMesh->mNumFaces; f++) {
					const aiFace& face = pMesh->mFaces[f];
					if (face.mNumIndices != 3) {
						continue;
					}
					m_indices.push_back(baseVertex + face.mIndices[0]);
					m_indices.push_back(baseVertex + face.mIndices[1]);
					m_indices.push_back(baseVertex + face.mIndices[2]);
				}
				//同じマテリアルが続くときは、サブセットをまとめる
				const uint32_t count = static_cast<uint32_t>(m_indices.size()) - startIndex;
				if (count > 0) {
					if (!m_subsets.empty() && m_subsets.back().materialIndex == pMesh->mMaterialIndex) {
						m_subsets.back().indexCount += count;
					}
					else {
						ModelSubset subset;
						subset.startIndex = startIndex;
						subset.indexCount = count;
						subset.materialIndex = pMesh->mMaterialIndex;
						m_subsets.push_back(subset);
					}
				}
			}
			if (m_boneNode.size() > kMaxBones) {
				errorText = L"too many bones";
				return false;
			}
			if (m_boneNode.empty()) {
				//ボーンもノードのアニメも無いモデルは、ルートを1つのボーンにしておく
				GetBoneIndex(0);
			}
			return true;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	アニメーションファイルからアニメーションを追加する。
		ノード名がモデルと一致するチャンネルだけが反映される。長さの無い（ポーズだけの）ものも読む。
		同じ名前がすでにあれば、先に読んだものを残す
		@param[in]	animFile	アニメーションファイル（glbなど）
		@param[in]	namePrefix	クリップ名の前に付ける文字列（名前の衝突を避けたいとき）
		@param[out]	errorText	失敗したときの理由
		@return	追加できたクリップの数
		*/
		//--------------------------------------------------------------------------------------
		size_t LoadAnimations(const std::wstring& animFile, const std::wstring& namePrefix, std::wstring& errorText) {
			Assimp::Importer importer;
			unsigned int flags = aiProcess_Triangulate;
			if (m_leftHanded) {
				flags |= aiProcess_ConvertToLeftHanded;
			}
			const aiScene* pScene = importer.ReadFile(ModelUtil::ToAnsi(animFile), flags);
			if (!pScene || pScene->mNumAnimations == 0) {
				errorText = ModelUtil::ToWide(importer.GetErrorString());
				if (errorText.empty()) {
					errorText = L"no animations";
				}
				return 0;
			}
			size_t added = 0;
			for (unsigned int a = 0; a < pScene->mNumAnimations; a++) {
				const aiAnimation* pAnim = pScene->mAnimations[a];
				const double tps = pAnim->mTicksPerSecond > 0.0 ? pAnim->mTicksPerSecond : 25.0;
				AnimationClip clip;
				clip.name = namePrefix + ModelUtil::ToWide(pAnim->mName.C_Str());
				clip.duration = pAnim->mDuration / tps;
				size_t skipped = 0;
				for (unsigned int c = 0; c < pAnim->mNumChannels; c++) {
					const aiNodeAnim* pNodeAnim = pAnim->mChannels[c];
					auto it = m_nodeIndex.find(pNodeAnim->mNodeName.C_Str());
					if (it == m_nodeIndex.end()) {
						skipped++;
						continue;
					}
					ModelChannel ch;
					ch.node = it->second;
					for (unsigned int k = 0; k < pNodeAnim->mNumPositionKeys; k++) {
						ch.positions.push_back({ pNodeAnim->mPositionKeys[k].mTime / tps, pNodeAnim->mPositionKeys[k].mValue });
					}
					for (unsigned int k = 0; k < pNodeAnim->mNumRotationKeys; k++) {
						ch.rotations.push_back({ pNodeAnim->mRotationKeys[k].mTime / tps, pNodeAnim->mRotationKeys[k].mValue });
					}
					for (unsigned int k = 0; k < pNodeAnim->mNumScalingKeys; k++) {
						ch.scalings.push_back({ pNodeAnim->mScalingKeys[k].mTime / tps, pNodeAnim->mScalingKeys[k].mValue });
					}
					clip.channels.push_back(ch);
				}
				if (skipped > 0) {
					ModelUtil::Warn(clip.name + L": " + std::to_wstring(skipped) + L" channel(s) do not match the model nodes");
				}
				if (m_clips.find(clip.name) != m_clips.end()) {
					ModelUtil::Warn(clip.name + L": already loaded, skipped (" + animFile + L")");
					continue;
				}
				m_clips.insert({ clip.name, clip });
				added++;
			}
			return added;
		}
		//--------------------------------------------------------------------------------------
		// 読み込んだモデルの取得
		//--------------------------------------------------------------------------------------
		const std::vector<ModelSkinVertex>& GetVertices() const { return m_vertices; }
		//インデックス（マテリアル順。ベース頂点は加算済み）
		const std::vector<uint32_t>& GetIndices() const { return m_indices; }
		const std::vector<ModelSubset>& GetSubsets() const { return m_subsets; }
		const std::vector<ModelMaterialData>& GetMaterials() const { return m_materials; }
		const std::vector<ModelTextureSource>& GetTextures() const { return m_textures; }
		size_t GetBoneCount() const { return m_boneNode.size(); }
		//ボーン番号が指すノード番号
		int GetBoneNode(size_t bone) const { return m_boneNode[bone]; }
		size_t GetNodeCount() const { return m_nodes.size(); }
		//ノード番号（無ければ-1）
		int FindNode(const std::wstring& name) const {
			auto it = m_nodeIndex.find(ModelUtil::ToUtf8(name));
			return it == m_nodeIndex.end() ? -1 : it->second;
		}
		int GetNodeParent(int node) const { return m_nodes[node].parent; }
		std::wstring GetNodeName(int node) const { return ModelUtil::ToWide(m_nodes[node].name); }
		//ノードが、ancestorと同じか、その子孫か
		bool IsNodeInSubtree(int node, int ancestor) const {
			while (node >= 0) {
				if (node == ancestor) {
					return true;
				}
				node = m_nodes[node].parent;
			}
			return false;
		}
		//--------------------------------------------------------------------------------------
		// アニメーションの取得
		//--------------------------------------------------------------------------------------
		const AnimationClip* FindClip(const std::wstring& name) const {
			auto it = m_clips.find(name);
			return it == m_clips.end() ? nullptr : &it->second;
		}
		bool HasClip(const std::wstring& name) const {
			return m_clips.find(name) != m_clips.end();
		}
		double GetClipDuration(const std::wstring& name) const {
			auto it = m_clips.find(name);
			return it == m_clips.end() ? 0.0 : it->second.duration;
		}
		std::vector<std::wstring> GetClipNames() const {
			std::vector<std::wstring> names;
			for (auto& c : m_clips) {
				names.push_back(c.first);
			}
			return names;
		}
		//--------------------------------------------------------------------------------------
		// イベントの定義
		//--------------------------------------------------------------------------------------
		//クリップにイベントを追加する（時刻は秒）。クリップが無ければfalse
		bool AddAnimationEvent(const std::wstring& clipName, double timeSec, const std::wstring& eventName, const std::wstring& param = L"") {
			auto it = m_clips.find(clipName);
			if (it == m_clips.end()) {
				return false;
			}
			AnimationEvent e;
			e.time = timeSec;
			e.name = eventName;
			e.param = param;
			it->second.events.push_back(e);
			SortEvents(it->second);
			return true;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	テキストファイルからイベントを読み込む。
		1行に「クリップ名<TAB>時刻<TAB>イベント名[<TAB>パラメータ]」。時刻は秒、末尾に%を付けるとクリップの長さに対する割合。
		#で始まる行はコメント。文字コードは、UTF-8のBOMがあればUTF-8、なければシステムの文字コード（CP932）
		@param[in]	textFile	テキストファイル
		@param[out]	errorText	失敗したときの理由
		@return	追加できたイベントの数
		*/
		//--------------------------------------------------------------------------------------
		size_t LoadAnimationEvents(const std::wstring& textFile, std::wstring& errorText) {
			std::ifstream ifs(textFile.c_str(), std::ios::binary);
			if (!ifs) {
				errorText = L"cannot open file";
				return 0;
			}
			std::string bytes((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			std::wstring text;
			if (bytes.size() >= 3 && static_cast<uint8_t>(bytes[0]) == 0xEF && static_cast<uint8_t>(bytes[1]) == 0xBB && static_cast<uint8_t>(bytes[2]) == 0xBF) {
				text = ModelUtil::ToWide(bytes.substr(3));
			}
			else if (!bytes.empty()) {
				int len = ::MultiByteToWideChar(CP_ACP, 0, bytes.c_str(), static_cast<int>(bytes.size()), nullptr, 0);
				text.assign(len, L'\0');
				::MultiByteToWideChar(CP_ACP, 0, bytes.c_str(), static_cast<int>(bytes.size()), &text[0], len);
			}
			size_t added = 0;
			size_t lineStart = 0;
			size_t lineNo = 0;
			while (lineStart <= text.size()) {
				size_t lineEnd = text.find(L'\n', lineStart);
				if (lineEnd == std::wstring::npos) {
					lineEnd = text.size();
				}
				std::wstring line = text.substr(lineStart, lineEnd - lineStart);
				lineStart = lineEnd + 1;
				lineNo++;
				while (!line.empty() && (line.back() == L'\r' || line.back() == L'\n')) {
					line.pop_back();
				}
				if (line.empty() || line[0] == L'#') {
					continue;
				}
				std::vector<std::wstring> cols;
				size_t s = 0;
				while (true) {
					size_t t = line.find(L'\t', s);
					if (t == std::wstring::npos) {
						cols.push_back(line.substr(s));
						break;
					}
					cols.push_back(line.substr(s, t - s));
					s = t + 1;
				}
				if (cols.size() < 3) {
					ModelUtil::Warn(L"events line " + std::to_wstring(lineNo) + L": needs clip, time, name");
					continue;
				}
				const AnimationClip* pClip = FindClip(cols[0]);
				if (!pClip) {
					ModelUtil::Warn(L"events line " + std::to_wstring(lineNo) + L": unknown clip " + cols[0]);
					continue;
				}
				double time = 0.0;
				try {
					std::wstring tstr = cols[1];
					if (!tstr.empty() && tstr.back() == L'%') {
						tstr.pop_back();
						time = std::stod(tstr) * 0.01 * pClip->duration;
					}
					else {
						time = std::stod(tstr);
					}
				}
				catch (...) {
					ModelUtil::Warn(L"events line " + std::to_wstring(lineNo) + L": bad time " + cols[1]);
					continue;
				}
				if (AddAnimationEvent(cols[0], time, cols[2], cols.size() > 3 ? cols[3] : L"")) {
					added++;
				}
			}
			return added;
		}
		//--------------------------------------------------------------------------------------
		// ポーズの計算（すべてローカルの位置・回転・スケールの空間で行う）
		//--------------------------------------------------------------------------------------
		//初期姿勢（バインドポーズ）
		const LocalPose& GetRestPose() const { return m_restPose; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	クリップの指定時刻の姿勢を得る。アニメーションの無いノードは初期姿勢
		@param[in]	clip	クリップ
		@param[in]	time	時刻（秒）。クリップの長さの範囲に収めて渡すこと
		@param[out]	out	姿勢
		*/
		//--------------------------------------------------------------------------------------
		void SampleClip(const AnimationClip& clip, double time, LocalPose& out) const {
			out = m_restPose;
			for (auto& ch : clip.channels) {
				NodeTRS& n = out.nodes[ch.node];
				n.t = SampleVec(ch.positions, time, m_nodes[ch.node].rest.t);
				n.r = SampleQuat(ch.rotations, time, m_nodes[ch.node].rest.r);
				n.s = SampleVec(ch.scalings, time, m_nodes[ch.node].rest.s);
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	姿勢の上書きブレンド（クロスフェード、マスク付きの上書き層）。
		位置とスケールは線形補間、回転は球面線形補間（半球補正つき）。重みは weight × マスク
		@param[in]	base	下の姿勢
		@param[in]	layer	上の姿勢
		@param[in]	weight	重み 0..1
		@param[in]	mask	ボーンマスク（nullptrなら全ノード1）
		@param[out]	out	結果（base・layerと同じオブジェクトでもよい）
		*/
		//--------------------------------------------------------------------------------------
		static void BlendOverride(const LocalPose& base, const LocalPose& layer, float weight, const BoneMask* mask, LocalPose& out) {
			const size_t n = base.nodes.size();
			LocalPose result;
			result.nodes.resize(n);
			for (size_t i = 0; i < n; i++) {
				float w = weight * (mask ? mask->Get(i) : 1.0f);
				w = (std::max)(0.0f, (std::min)(1.0f, w));
				const NodeTRS& a = base.nodes[i];
				const NodeTRS& b = layer.nodes[i];
				NodeTRS& r = result.nodes[i];
				if (w <= 0.0f) {
					r = a;
				}
				else if (w >= 1.0f) {
					r = b;
				}
				else {
					r.t = a.t + (b.t - a.t) * w;
					r.s = a.s + (b.s - a.s) * w;
					aiQuaternion::Interpolate(r.r, a.r, b.r, w);
					r.r.Normalize();
				}
			}
			out = result;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	加算ブレンド。add と reference の差分を、base に足す。
		位置は差の加算、回転は reference^-1 × add を重みぶん（球面線形補間）掛ける、スケールは比
		@param[in,out]	base	下の姿勢（結果もここに入る）
		@param[in]	add	加算する姿勢
		@param[in]	reference	加算の基準になる姿勢
		@param[in]	weight	重み 0..1
		@param[in]	mask	ボーンマスク（nullptrなら全ノード1）
		*/
		//--------------------------------------------------------------------------------------
		static void ApplyAdditive(LocalPose& base, const LocalPose& add, const LocalPose& reference, float weight, const BoneMask* mask) {
			const size_t n = base.nodes.size();
			for (size_t i = 0; i < n; i++) {
				const float w = weight * (mask ? mask->Get(i) : 1.0f);
				if (w <= 0.0f) {
					continue;
				}
				NodeTRS& b = base.nodes[i];
				const NodeTRS& a = add.nodes[i];
				const NodeTRS& r = reference.nodes[i];
				b.t = b.t + (a.t - r.t) * w;
				//回転の差分（最短経路にする）
				aiQuaternion refInv = r.r;
				refInv.Conjugate();
				aiQuaternion delta = refInv * a.r;
				if (delta.w < 0.0f) {
					delta.w = -delta.w;
					delta.x = -delta.x;
					delta.y = -delta.y;
					delta.z = -delta.z;
				}
				aiQuaternion partial;
				aiQuaternion::Interpolate(partial, aiQuaternion(), delta, w);
				b.r = b.r * partial;
				b.r.Normalize();
				//スケールの比（基準が0に近い成分は比を1とする）
				aiVector3D ratio(1.0f, 1.0f, 1.0f);
				if (std::fabs(r.s.x) > 1e-6f) { ratio.x = a.s.x / r.s.x; }
				if (std::fabs(r.s.y) > 1e-6f) { ratio.y = a.s.y / r.s.y; }
				if (std::fabs(r.s.z) > 1e-6f) { ratio.z = a.s.z / r.s.z; }
				b.s.x *= 1.0f + (ratio.x - 1.0f) * w;
				b.s.y *= 1.0f + (ratio.y - 1.0f) * w;
				b.s.z *= 1.0f + (ratio.z - 1.0f) * w;
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ルートモーション。prev→curの間の移動量を求めて、姿勢から移動分を取り除く。
		時間は、ループで巻き戻さない連続した値（unwrapped）で渡す。逆再生・複数周でも正しく積算される
		@param[in]	clip	クリップ
		@param[in]	rm	設定
		@param[in]	prevU	前回の時刻（連続した値）
		@param[in]	curU	今回の時刻（連続した値）
		@param[in]	loop	ループするか
		@param[in,out]	pose	今回の時刻（巻き戻し済み）でサンプルした姿勢。移動分を取り除く
		@param[out]	delta	モデル空間での移動量（Extractのときだけ値が入る）
		*/
		//--------------------------------------------------------------------------------------
		void ExtractRootMotion(const AnimationClip& clip, const RootMotionSettings& rm, double prevU, double curU, bool loop,
			LocalPose& pose, RootMotionDelta& delta) const {
			delta = RootMotionDelta();
			if (rm.mode == RootMotionMode::AsAuthored) {
				return;
			}
			const int node = FindNode(rm.motionNode);
			if (node < 0) {
				return;
			}
			const aiMatrix3x3 linear = ParentLinear(node);
			aiMatrix3x3 linearInv = linear;
			linearInv.Inverse();
			auto maskAxes = [&](aiVector3D v) {
				if (!rm.axisX) { v.x = 0.0f; }
				if (!rm.axisY) { v.y = 0.0f; }
				if (!rm.axisZ) { v.z = 0.0f; }
				return v;
			};
			if (rm.mode == RootMotionMode::Extract) {
				const aiVector3D moveCur = maskAxes(linear * AccumLocalMove(clip, node, curU, loop));
				const aiVector3D movePrev = maskAxes(linear * AccumLocalMove(clip, node, prevU, loop));
				delta.translation = moveCur - movePrev;
				if (rm.extractYaw) {
					delta.yaw = AccumYaw(clip, node, curU, loop) - AccumYaw(clip, node, prevU, loop);
				}
			}
			//姿勢から、移動分（クリップ先頭からの変位）を取り除く。先頭の位置は残る
			double tw = curU;
			if (clip.duration > 0.0) {
				tw = loop ? curU - std::floor(curU / clip.duration) * clip.duration : (std::max)(0.0, (std::min)(curU, clip.duration));
			}
			else {
				tw = 0.0;
			}
			const aiVector3D disp = maskAxes(linear * (MotionLocalPos(clip, node, tw) - MotionLocalPos(clip, node, 0.0)));
			pose.nodes[node].t = pose.nodes[node].t - linearInv * disp;
			if (rm.extractYaw) {
				const aiQuaternion q = pose.nodes[node].r;
				aiQuaternion twistInv = TwistOf(q);
				twistInv.Conjugate();
				aiQuaternion twist0 = TwistOf(MotionLocalRot(clip, node, 0.0));
				aiQuaternion swing = q * twistInv;
				pose.nodes[node].r = swing * twist0;
				pose.nodes[node].r.Normalize();
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	prev→curの間に発火するイベントを集める。時間は連続した値（unwrapped）で渡す。
		順方向は (prev, cur]、逆方向は [cur, prev)。includeStartを真にすると prev も含める（再生開始直後用）。
		ループでは周回ごとに発火する（1回の呼び出しで最大4周まで）。ループしない場合は、0..長さ の範囲に収める
		@param[in]	clip	クリップ
		@param[in]	prevU	前回の時刻（連続した値）
		@param[in]	curU	今回の時刻（連続した値）
		@param[in]	loop	ループするか
		@param[in]	includeStart	prev のイベントも含めるか
		@param[out]	out	発火するイベントを、発火順に追加する
		*/
		//--------------------------------------------------------------------------------------
		static void CollectEvents(const AnimationClip& clip, double prevU, double curU, bool loop, bool includeStart,
			std::vector<const AnimationEvent*>& out) {
			if (clip.events.empty()) {
				return;
			}
			const double d = clip.duration;
			const bool forward = curU >= prevU;
			double lo = forward ? prevU : curU;
			double hi = forward ? curU : prevU;
			if (!loop || d <= 0.0) {
				lo = (std::max)(0.0, (std::min)(lo, d));
				hi = (std::max)(0.0, (std::min)(hi, d));
				if (hi <= lo && !(includeStart && forward && hi == lo)) {
					return;
				}
			}
			auto inRange = [&](double t) {
				if (forward) {
					return (includeStart ? t >= lo : t > lo) && t <= hi;
				}
				return t >= lo && (includeStart ? t <= hi : t < hi);
			};
			auto fireLap = [&](double offset) {
				if (forward) {
					for (auto& e : clip.events) {
						//再生開始の瞬間に、（再生していない）1周前の終端のイベントは発火させない
						if (loop && includeStart && e.time >= d - 1e-9 && offset + e.time <= lo + 1e-9) {
							continue;
						}
						if (inRange(offset + e.time)) {
							out.push_back(&e);
						}
					}
				}
				else {
					for (auto it = clip.events.rbegin(); it != clip.events.rend(); ++it) {
						if (inRange(offset + it->time)) {
							out.push_back(&*it);
						}
					}
				}
			};
			if (!loop || d <= 0.0) {
				fireLap(0.0);
				return;
			}
			//ループ。周回ごとに調べる（最大4周に制限）
			//（イベントの時刻が周期の終端と一致するものを拾うため、1周前から調べる）
			long long kLo = static_cast<long long>(std::floor(lo / d)) - 1;
			long long kHi = static_cast<long long>(std::floor(hi / d));
			if (kHi - kLo > 4) {
				kLo = kHi - 4;
			}
			if (forward) {
				for (long long k = kLo; k <= kHi; k++) {
					fireLap(static_cast<double>(k) * d);
				}
			}
			else {
				for (long long k = kHi; k >= kLo; k--) {
					fireLap(static_cast<double>(k) * d);
				}
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	姿勢から、全ノードのモデル空間の行列を計算する（Assimpの行列）
		*/
		//--------------------------------------------------------------------------------------
		void ComputeGlobals(const LocalPose& pose, std::vector<aiMatrix4x4>& outNodeGlobal) const {
			outNodeGlobal.resize(m_nodes.size());
			for (size_t i = 0; i < m_nodes.size(); i++) {
				const NodeTRS& n = pose.nodes[i];
				const aiMatrix4x4 local(n.s, n.r, n.t);
				outNodeGlobal[i] = m_nodes[i].parent >= 0 ? outNodeGlobal[m_nodes[i].parent] * local : local;
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ノードのモデル空間の行列から、ボーンごとのスキン行列（初期姿勢の頂点を、その姿勢の位置へ移す）を計算する。
		Assimpの行列のまま（列ベクトルに掛ける形）。シェーダーには行ごとに渡す
		*/
		//--------------------------------------------------------------------------------------
		void ComputeSkinMatrices(const std::vector<aiMatrix4x4>& nodeGlobal, std::vector<aiMatrix4x4>& outBones) const {
			outBones.resize(m_boneNode.size());
			for (size_t b = 0; b < m_boneNode.size(); b++) {
				outBones[b] = nodeGlobal[m_boneNode[b]] * m_nodes[m_boneNode[b]].invRestGlobal;
			}
		}
	};

	//--------------------------------------------------------------------------------------
	// BoneMask の実装
	//--------------------------------------------------------------------------------------
	inline BoneMask BoneMask::All(const ModelLoader& model, float weight) {
		BoneMask mask;
		mask.m_weights.assign(model.GetNodeCount(), weight);
		return mask;
	}
	inline BoneMask BoneMask::Subtree(const ModelLoader& model, const std::wstring& nodeName, float weight) {
		BoneMask mask;
		mask.m_weights.assign(model.GetNodeCount(), 0.0f);
		mask.Include(model, nodeName, true, weight);
		return mask;
	}
	inline BoneMask& BoneMask::Include(const ModelLoader& model, const std::wstring& nodeName, bool recursive, float weight) {
		if (m_weights.size() != model.GetNodeCount()) {
			m_weights.resize(model.GetNodeCount(), 0.0f);
		}
		const int node = model.FindNode(nodeName);
		if (node < 0) {
			ModelUtil::Warn(L"BoneMask: unknown node " + nodeName);
			return *this;
		}
		for (int i = 0; i < static_cast<int>(m_weights.size()); i++) {
			if (i == node || (recursive && model.IsNodeInSubtree(i, node))) {
				m_weights[i] = weight;
			}
		}
		return *this;
	}
	inline BoneMask& BoneMask::Exclude(const ModelLoader& model, const std::wstring& nodeName, bool recursive) {
		return Include(model, nodeName, recursive, 0.0f);
	}

}
// end namespace basecross
