/*!
@file SpSceneComp.cpp
@brief シンプル描画コンポーネント　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once
#include "stdafx.h"

namespace basecross {


	using namespace SceneEnums;

	IMPLEMENT_DX12SHADER(SpVSPNTStatic, App::GetShadersDir() + L"SpVSPNTStatic.cso")
	IMPLEMENT_DX12SHADER(SpVSPNTStaticShadow, App::GetShadersDir() + L"SpVSPNTStaticShadow.cso")

	IMPLEMENT_DX12SHADER(SpPSPNTStatic, App::GetShadersDir() + L"SpPSPNTStatic.cso")
	IMPLEMENT_DX12SHADER(SpPSPNTStaticShadow, App::GetShadersDir() + L"SpPSPNTStaticShadow.cso")

	void SpPNTStaticDraw::OnCreate() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		//シンプルコンスタントバッファ
		for (size_t i = 0; i < BaseDevice::FrameCount; i++) {
			m_constantBufferIndex =
				frameResources[i]->AddBaseConstantBufferSet<SimpleConstant>(pBaseDevice->GetD3D12Device());
		}
	}

	std::shared_ptr<BaseMesh> SpPNTStaticDraw::GetBaseMesh() const {
		auto mesh = m_mesh.lock();
		if (mesh) {
			return mesh;
		}
		return nullptr;
	}


	std::shared_ptr<BaseTexture> SpPNTStaticDraw::GetBaseTexture() const {
		auto texture = m_texture.lock();
		if (texture) {
			return texture;
		}
		return nullptr;
	}



	void SpPNTStaticDraw::OnUpdateConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto stage = scene->GetActiveStage();
		auto& frameResources = scene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		auto& viewport = scene->GetViewport();
		std::shared_ptr<PerspecCamera> myCamera;
		std::shared_ptr<LightSet> myLightSet;
		if (stage) {
			myCamera = std::dynamic_pointer_cast<PerspecCamera>(stage->GetTargetCamera());
			if (!myCamera) {
				return;
			}
			myLightSet = stage->GetTargetLightSet();
		}
		else {
			return;
		}
		auto gameObject = m_gameObject.lock();
		if (gameObject) {

			//Transformコンポーネントを取り出す
			auto ptrTrans = gameObject->GetComponent<Transform>();
			auto& param = ptrTrans->GetTransParam();
			//シーンのコンスタントバッファ
			{

				m_constantBuffer = {};
				bsm::Mat4x4 world = ptrTrans->GetWorldMatrix();
				//転置する
				world.transpose();
				//ビューと射影行列を得る
				bsm::Mat4x4 viewMat = (bsm::Mat4x4)myCamera->GetViewMatrix();
				//転置する
				viewMat.transpose();
				bsm::Mat4x4 projMat = (bsm::Mat4x4)myCamera->GetProjMatrix();
				//転置する
				projMat.transpose();
				m_constantBuffer.world = world;
				m_constantBuffer.view = viewMat;
				m_constantBuffer.projection = projMat;
				//エミッシブ
				m_constantBuffer.emissive = GetEmissive();
				//デフィーズ
				m_constantBuffer.diffuse = GetDiffuse();
				//スペキュラー
				m_constantBuffer.specular = GetSpecular();
				//ライティング
				auto index = myLightSet->GetMainIndex();
				auto light = myLightSet->GetLight(index);
				m_constantBuffer.lightDir = (bsm::Vec3)light.m_directional;
				m_constantBuffer.lightDir.w = 1.0f;
				m_constantBuffer.eyePos = (bsm::Vec4)myCamera->GetEye();
				m_constantBuffer.eyePos.w = 1.0f;
				//テクスチャがあった場合
				if (GetBaseTexture()) {
					m_constantBuffer.activeFlg.x = 1;
				}
				//影用
				if (IsOwnShadowActive()) {
					bsm::Vec3 CalcLightDir = (bsm::Vec3)light.m_directional * -1.0;
					bsm::Vec3 LightAt = (bsm::Vec3)myCamera->GetAt();
					bsm::Vec3 LightEye = CalcLightDir;
					LightEye *= Shadowmap::m_lightHeight;
					LightEye = LightAt + LightEye;
					m_constantBuffer.lightPos = LightEye;
					m_constantBuffer.lightPos.w = 1.0f;
					bsm::Mat4x4 lightView, lightProj;
					//ライトのビューと射影を計算
					lightView = XMMatrixLookAtLH(LightEye, LightAt, bsm::Vec3(0, 1.0f, 0));
					lightProj = XMMatrixOrthographicLH(Shadowmap::m_viewWidth, Shadowmap::m_viewHeight,
						Shadowmap::m_lightNear, Shadowmap::m_lightFar);
					m_constantBuffer.lightView = lightView.transpose();
					m_constantBuffer.lightProjection = lightProj.transpose();
				}

			}
		}
	}

	void SpPNTStaticDraw::OnCommitConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto pCurrentFrameResource = scene->GetCurrentFrameResource();
		//シーン
		memcpy(pCurrentFrameResource->m_baseConstantBufferSetVec[m_constantBufferIndex].m_pBaseConstantBufferWO,
			&m_constantBuffer, sizeof(m_constantBuffer));
	}



	void SpPNTStaticDraw::OnSceneDraw() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pCurrentFrameResource = pBaseScene->GetCurrentFrameResource();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		auto& viewport = pBaseScene->GetViewport();
		auto& scissorRect = pBaseScene->GetScissorRect();
		auto depthDsvs = pBaseScene->GetDepthDsvs();
		auto mesh = m_mesh.lock();
		auto texture = m_texture.lock();
		if (mesh && texture) {

			//t0: DepthGenPass::Shadow
			pCommandList->SetGraphicsRootDescriptorTable(
				pBaseScene->GetGpuSlotID(L"t0"),
				pBaseScene->GetDepthSrvGpuHandles()[DepthGenPass::Shadow]);
			// Set the shadow texture as an SRV.
			pCommandList->OMSetRenderTargets(1,
				&pBaseScene->GetCurrentBackBufferRtvCpuHandle(),
				FALSE,
				&depthDsvs[DepthGenPass::Scene]);
			auto SpPNTStaticShadowPileline
				= PipelineStatePool::GetPipelineState(L"SpPNTStaticShadow", true);
			pCommandList->SetPipelineState(SpPNTStaticShadowPileline.Get());
			//Sampler
			UINT index = pBaseScene->GetSamplerIndex(L"LinearClamp");
			if (index == UINT_MAX) {
				throw BaseException(
					L"LinearClampサンプラーが特定できません。",
					L"SpSceneComp::OnSceneDraw()"
				);
			}
			CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle(
				pBaseScene->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				index,
				pBaseScene->GetSamplerDescriptorHandleIncrementSize()
			);
			pCommandList->SetGraphicsRootDescriptorTable(
				pBaseScene->GetGpuSlotID(L"s0"),
				samplerHandle);

			index = pBaseScene->GetSamplerIndex(L"ComparisonLinear");
			if (index == UINT_MAX) {
				throw BaseException(
					L"ComparisonLinearサンプラーが特定できません。",
					L"Scene::ScenePass()"
				);
			}
			CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle2(
				pBaseScene->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				index,
				pBaseScene->GetSamplerDescriptorHandleIncrementSize()
			);
			pCommandList->SetGraphicsRootDescriptorTable(
				pBaseScene->GetGpuSlotID(L"s1"),
				samplerHandle2);

			//シェーダリソース（テクスチャ）のハンドルの設定
			CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(
				pBaseScene->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				texture->GetSrvIndex(),
				pBaseScene->GetCbvSrvUavDescriptorHandleIncrementSize()
			);
			pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t1"), srvHandle);
			//Cbv
			// Set scene constant buffer.
			pCommandList->SetGraphicsRootConstantBufferView(pBaseScene->GetGpuSlotID(L"b0"),
				pCurrentFrameResource->m_baseConstantBufferSetVec[m_constantBufferIndex].m_baseConstantBuffer->GetGPUVirtualAddress());
			//Draw
			pCommandList->IASetVertexBuffers(0, 1, &mesh->GetVertexBufferView());
			pCommandList->IASetIndexBuffer(&mesh->GetIndexBufferView());
			pCommandList->DrawIndexedInstanced(mesh->GetNumIndices(), 1, 0, 0, 0);

		}

	}



}
// end namespace basecross
