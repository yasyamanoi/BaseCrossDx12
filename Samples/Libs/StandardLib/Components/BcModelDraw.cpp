/*!
@file BcModelDraw.cpp
@brief モデル描画コンポーネント　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {

	BcPNTModelDraw::BcPNTModelDraw(const std::shared_ptr<GameObject>& gameObjectPtr) :
		BcPNTBoneDraw(gameObjectPtr)
	{
	}

	void BcPNTModelDraw::EnsureMaterialBuffers(size_t count) {
		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		while (m_materialBufferIndex.size() < count) {
			size_t index = 0;
			for (size_t i = 0; i < BaseDevice::FrameCount; i++) {
				index = frameResources[i]->AddBaseConstantBufferSet<BasicConstant>(pBaseDevice->GetD3D12Device());
			}
			m_materialBufferIndex.push_back(index);
		}
	}

	void BcPNTModelDraw::SetModel(const std::wstring& key) {
		SetModel(BaseScene::Get()->GetModel(key));
	}

	void BcPNTModelDraw::SetModel(const std::shared_ptr<BaseModel>& model) {
		m_model = model;
		ClearMaterialOverrides();
		if (m_model) {
			SetBaseMesh(m_model->GetMesh());
			EnsureMaterialBuffers(m_model->GetMaterials().size());
		}
	}

	void BcPNTModelDraw::SetMaterialTexture(size_t materialIndex, const std::shared_ptr<BaseTexture>& texture) {
		if (m_textureOverride.size() <= materialIndex) {
			m_textureOverride.resize(materialIndex + 1);
		}
		m_textureOverride[materialIndex] = texture;
	}

	void BcPNTModelDraw::SetMaterialDiffuse(size_t materialIndex, const Col4& color) {
		if (m_diffuseOverride.size() <= materialIndex) {
			m_diffuseOverride.resize(materialIndex + 1);
			m_diffuseOverrideActive.resize(materialIndex + 1, false);
		}
		m_diffuseOverride[materialIndex] = color;
		m_diffuseOverrideActive[materialIndex] = true;
	}

	void BcPNTModelDraw::ClearMaterialOverrides() {
		m_textureOverride.clear();
		m_diffuseOverride.clear();
		m_diffuseOverrideActive.clear();
	}

	void BcPNTModelDraw::OnUpdateConstantBuffers() {
		if (!m_model) {
			return;
		}
		auto gameObject = m_gameObject.lock();
		if (!gameObject) {
			return;
		}
		//同じオブジェクトにAnimatorがあれば、そのボーン行列を使う
		auto animator = gameObject->GetComponent<Animator>(false);
		if (animator) {
			SetBoneTransforms(animator->GetBoneTransforms());
		}
		//ライト、カメラ、霧、ボーンなどの共通の値を、親クラスで計算する
		BcPNTBoneDraw::OnUpdateConstantBuffers();
		//マテリアルごとに、色とテクスチャの有無を差し替える
		const auto& materials = m_model->GetMaterials();
		m_materialConstants.assign(materials.size(), m_constantBuffer);
		//親クラスが計算した、アンビエントを含むエミッシブ色（拡散色=1のとき）
		const Col4 ambient = m_constantBuffer.emissiveColor;
		for (size_t i = 0; i < materials.size(); i++) {
			BasicConstant& c = m_materialConstants[i];
			const ModelMaterialData& data = materials[i].data;
			float diffuse[4] = { data.diffuse[0], data.diffuse[1], data.diffuse[2], data.diffuse[3] };
			if (i < m_diffuseOverrideActive.size() && m_diffuseOverrideActive[i]) {
				diffuse[0] = m_diffuseOverride[i].x;
				diffuse[1] = m_diffuseOverride[i].y;
				diffuse[2] = m_diffuseOverride[i].z;
				diffuse[3] = m_diffuseOverride[i].w;
			}
			std::shared_ptr<BaseTexture> texture = materials[i].texture;
			if (i < m_textureOverride.size() && m_textureOverride[i]) {
				texture = m_textureOverride[i];
			}
			c.activeFlg.y = texture ? 1 : 0;
			const float a = diffuse[3];
			c.diffuseColor = Col4(diffuse[0] * a, diffuse[1] * a, diffuse[2] * a, a);
			//エミッシブ = マテリアルの発光 + アンビエント × 拡散色
			c.emissiveColor = Col4(
				data.emissive[0] + ambient.x * diffuse[0] * a,
				data.emissive[1] + ambient.y * diffuse[1] * a,
				data.emissive[2] + ambient.z * diffuse[2] * a,
				ambient.w
			);
		}
	}

	void BcPNTModelDraw::OnCommitConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto pCurrentFrameResource = scene->GetCurrentFrameResource();
		const size_t count = (std::min)(m_materialConstants.size(), m_materialBufferIndex.size());
		for (size_t i = 0; i < count; i++) {
			memcpy(pCurrentFrameResource->m_baseConstantBufferSetVec[m_materialBufferIndex[i]].m_pBaseConstantBufferWO,
				&m_materialConstants[i], sizeof(BasicConstant));
		}
	}

	void BcPNTModelDraw::OnSceneDraw(ID3D12GraphicsCommandList* pCommandList) {
		auto mesh = GetBaseMesh();
		if (!m_model || !mesh) {
			return;
		}
		const auto& materials = m_model->GetMaterials();
		const auto& subsets = m_model->GetSubsets();
		if (m_materialBufferIndex.size() < materials.size() || m_materialConstants.size() < materials.size()) {
			return;
		}
		auto pBaseScene = BaseScene::Get();
		auto pCurrentFrameResource = pBaseScene->GetCurrentFrameResource();
		const bool objectAlpha = GetGameObject()->IsAlphaActive();
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pCommandList->IASetVertexBuffers(0, 1, &mesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&mesh->GetIndexBufferView());
		//不透明のサブセット（pass 0）を先に、透明のサブセット（pass 1）を後に描く
		for (int pass = 0; pass < 2; pass++) {
			for (auto& subset : subsets) {
				if (subset.materialIndex >= materials.size()) {
					continue;
				}
				const ModelMaterial& material = materials[subset.materialIndex];
				const bool alpha = objectAlpha || material.data.alphaBlend;
				if ((pass == 1) != alpha) {
					continue;
				}
				//パイプラインステート、シャドウマップ、サンプラー
				SetCommonDrawState(pCommandList, alpha);
				//テクスチャ
				std::shared_ptr<BaseTexture> texture = material.texture;
				if (subset.materialIndex < m_textureOverride.size() && m_textureOverride[subset.materialIndex]) {
					texture = m_textureOverride[subset.materialIndex];
				}
				if (texture) {
					CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(
						pBaseScene->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
						texture->GetSrvIndex(),
						pBaseScene->GetCbvSrvUavDescriptorHandleIncrementSize()
					);
					pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t1"), srvHandle);
				}
				else {
					CD3DX12_GPU_DESCRIPTOR_HANDLE srvNullHandle(
						pBaseScene->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
					);
					pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t1"), srvNullHandle);
				}
				//このマテリアルのコンスタントバッファ
				pCommandList->SetGraphicsRootConstantBufferView(pBaseScene->GetGpuSlotID(L"b0"),
					pCurrentFrameResource->m_baseConstantBufferSetVec[m_materialBufferIndex[subset.materialIndex]].m_baseConstantBuffer->GetGPUVirtualAddress());
				pCommandList->DrawIndexedInstanced(subset.indexCount, 1, subset.startIndex, 0, 0);
			}
		}
	}

}
// end namespace basecross
