/*!
@file BaseModel.cpp
@brief モデルクラス　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {

	namespace {
		//ローダーの頂点を、スキニング用の頂点の型に変換する
		std::vector<VertexPositionNormalTextureSkinning> ToSkinningVertices(const ModelLoader& loader) {
			std::vector<VertexPositionNormalTextureSkinning> vertices;
			vertices.reserve(loader.GetVertices().size());
			for (auto& sv : loader.GetVertices()) {
				uint32_t bone[4];
				float weight[4];
				for (int i = 0; i < 4; i++) {
					bone[i] = sv.bone[i];
					weight[i] = sv.weight[i];
				}
				vertices.push_back(VertexPositionNormalTextureSkinning(
					XMFLOAT3(sv.position[0], sv.position[1], sv.position[2]),
					XMFLOAT3(sv.normal[0], sv.normal[1], sv.normal[2]),
					XMFLOAT2(sv.uv[0], sv.uv[1]),
					bone, weight
				));
			}
			return vertices;
		}
	}

	std::shared_ptr<BaseModel> BaseModel::CreateFromFile(ID3D12GraphicsCommandList* pCommandList,
		const std::wstring& modelFile, const ModelLoadOptions& options) {
		auto ptrModel = std::shared_ptr<BaseModel>(new BaseModel());
		ptrModel->m_loader = std::make_shared<ModelLoader>();
		std::wstring errorText;
		if (!ptrModel->m_loader->LoadModel(modelFile, options, errorText)) {
			throw BaseException(
				L"モデルの読み込みに失敗しました",
				modelFile + L" " + errorText,
				L"BaseModel::CreateFromFile()"
			);
		}
		auto& loader = *ptrModel->m_loader;
		//メッシュ
		ptrModel->m_mesh = BaseMesh::CreateBaseMesh<VertexPositionNormalTextureSkinning>(
			pCommandList, ToSkinningVertices(loader), loader.GetIndices());
		//テクスチャ（モデルの中で重複しないように1つずつ）
		for (auto& src : loader.GetTextures()) {
			std::shared_ptr<BaseTexture> texture;
			try {
				switch (src.type) {
				case ModelTextureSource::Type::EmbeddedCompressed:
					texture = BaseTexture::CreateTextureFromMemory(pCommandList,
						src.bytes.data(), src.bytes.size(), ModelUtil::ToWide(src.formatHint), src.name);
					break;
				case ModelTextureSource::Type::EmbeddedRaw:
					texture = BaseTexture::CreateTextureFromPixels(pCommandList,
						src.width, src.height, DXGI_FORMAT_B8G8R8A8_UNORM, src.bytes.data(), static_cast<size_t>(src.width) * 4);
					break;
				case ModelTextureSource::Type::File:
					texture = BaseTexture::CreateTextureFlomFile(pCommandList, src.filePath);
					break;
				default:
					break;
				}
			}
			catch (...) {
				//テクスチャが読めなくても、モデルは色だけで描けるので、警告にとどめる
				ModelUtil::Warn(L"texture load failed: " + src.name);
				texture = nullptr;
			}
			ptrModel->m_textures.push_back(texture);
		}
		//マテリアル
		for (auto& data : loader.GetMaterials()) {
			ModelMaterial material;
			material.data = data;
			if (data.textureIndex >= 0 && static_cast<size_t>(data.textureIndex) < ptrModel->m_textures.size()) {
				material.texture = ptrModel->m_textures[data.textureIndex];
			}
			if (data.alphaBlend) {
				ptrModel->m_hasAlphaBlend = true;
			}
			ptrModel->m_materials.push_back(material);
		}
		return ptrModel;
	}

	size_t BaseModel::AddAnimationsFromFile(const std::wstring& animFile, const std::wstring& namePrefix) {
		std::wstring errorText;
		size_t count = m_loader->LoadAnimations(animFile, namePrefix, errorText);
		if (count == 0) {
			ModelUtil::Warn(L"no animation added: " + animFile + L" " + errorText);
		}
		return count;
	}

	size_t BaseModel::AddAnimationsFromDirectory(const std::wstring& dir, const std::wstring& pattern) {
		std::wstring base = dir;
		if (!base.empty() && base.back() != L'\\' && base.back() != L'/') {
			base += L'\\';
		}
		size_t count = 0;
		WIN32_FIND_DATAW findData;
		HANDLE hFind = ::FindFirstFileW((base + pattern).c_str(), &findData);
		if (hFind == INVALID_HANDLE_VALUE) {
			return 0;
		}
		do {
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				continue;
			}
			count += AddAnimationsFromFile(base + findData.cFileName);
		} while (::FindNextFileW(hFind, &findData));
		::FindClose(hFind);
		return count;
	}

	size_t BaseModel::LoadAnimationEvents(const std::wstring& textFile) {
		std::wstring errorText;
		size_t count = m_loader->LoadAnimationEvents(textFile, errorText);
		if (count == 0 && !errorText.empty()) {
			ModelUtil::Warn(L"no event added: " + textFile + L" " + errorText);
		}
		return count;
	}

}
//end basecross
