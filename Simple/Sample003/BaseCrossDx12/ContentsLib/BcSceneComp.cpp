/*!
@file BcSceneComp.cpp
@brief ベイシックシーンコンポーネント　実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	
	
	using namespace std;
	using namespace basecross::bsm;
	using namespace SceneEnums;

	IMPLEMENT_DX12SHADER(BcVSPNTStaticPL, App::GetShadersDir() + L"BcVSPNTStaticPL.cso")
	IMPLEMENT_DX12SHADER(BcPSPNTPL, App::GetShadersDir() + L"BcPSPNTPL.cso")

	IMPLEMENT_DX12SHADER(BcVSPNTStaticPLShadow, App::GetShadersDir() + L"BcVSPNTStaticPLShadow.cso")
	IMPLEMENT_DX12SHADER(BcPSPNTPLShadow, App::GetShadersDir() + L"BcPSPNTPLShadow.cso")



	BcSceneComp::BcSceneComp(const shared_ptr<GameObject>& gameObjectPtr) :
		Component(gameObjectPtr)
	{
		m_fogColor = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		m_fogVector = XMFLOAT3(0.0, 0.0, 1.0f);
	}

	void BcSceneComp::OnCreate() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		//ベイシックコンスタントバッファ
		for (size_t i = 0; i < BaseDevice::FrameCount; i++) {
			m_constantBufferIndex =
				frameResources[i]->AddBaseConstantBufferSet<BasicConstantBuffer>(pBaseDevice->GetD3D12Device());
		}
	}

	void BcSceneComp::OnUpdateConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto gameStage = dynamic_pointer_cast<GameStage>(scene->GetActiveStage());
		auto& frameResources = scene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		auto& viewport = scene->GetViewport();
		shared_ptr<PerspecCamera> myCamera;
		shared_ptr<LightSet> myLightSet;
		if (gameStage) {
			myCamera = gameStage->GetMyCamera();
			myLightSet = gameStage->GetLightSet();

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

				//初期化
				m_constantBuffer = {};
				m_constantBuffer.activeFlg.x = 3;
				m_constantBuffer.activeFlg.y = 1;

				//ワールド行列の設定
				auto world = XMMatrixAffineTransformation(
					param.scale,
					param.rotOrigin,
					param.quaternion,
					param.position
				);
				auto view = (XMMATRIX)((Mat4x4)myCamera->GetViewMatrix());
				auto proj = (XMMATRIX)((Mat4x4)myCamera->GetProjMatrix());
				auto worldView = world * view;
				m_constantBuffer.worldViewProj = Mat4x4(XMMatrixTranspose(XMMatrixMultiply(worldView, proj)));
				//フォグの設定
				if (m_fogEnabled)
				{
					auto start = m_fogStart;
					auto end = m_fogEnd;
					if (start == end)
					{
						// Degenerate case: force everything to 100% fogged if start and end are the same.
						static const XMVECTORF32 fullyFogged = { 0, 0, 0, 1 };
						m_constantBuffer.fogVector = Vec4(fullyFogged);
					}
					else
					{
						XMMATRIX worldViewTrans = worldView;
						// _13, _23, _33, _43
						XMVECTOR worldViewZ = XMVectorMergeXY(XMVectorMergeZW(worldViewTrans.r[0], worldViewTrans.r[2]),
							XMVectorMergeZW(worldViewTrans.r[1], worldViewTrans.r[3]));
						XMVECTOR wOffset = XMVectorSwizzle<1, 2, 3, 0>(XMLoadFloat(&start));
						m_constantBuffer.fogVector = Vec4((worldViewZ + wOffset) / (start - end));
					}
					m_constantBuffer.fogColor = (Col4)m_fogColor;
				}
				else
				{
					m_constantBuffer.fogVector = Vec4(g_XMZero);
					m_constantBuffer.fogColor = Vec4(g_XMZero);
				}
				//ライトの決定
				for (int i = 0; i < myLightSet->GetNumLights(); i++) {
					m_constantBuffer.lightDirection[i] = (Vec4)myLightSet->GetLight(i).m_directional;
					m_constantBuffer.lightDiffuseColor[i] = (Vec4)myLightSet->GetLight(i).m_diffuseColor;
					m_constantBuffer.lightSpecularColor[i] = (Vec4)myLightSet->GetLight(i).m_specularColor;
				}
				//ワールド行列
				m_constantBuffer.world = Mat4x4(world);
				m_constantBuffer.world.transpose();

				XMMATRIX worldInverse = XMMatrixInverse(nullptr, world);
				m_constantBuffer.worldInverseTranspose[0] = Vec4(worldInverse.r[0]);
				m_constantBuffer.worldInverseTranspose[1] = Vec4(worldInverse.r[1]);
				m_constantBuffer.worldInverseTranspose[2] = Vec4(worldInverse.r[2]);

				XMMATRIX viewInverse = XMMatrixInverse(nullptr, view);
				m_constantBuffer.eyePosition = Vec4(viewInverse.r[3]);

				Col4 diffuse = Col4(1.0f);
				Col4 alphaVector = (Col4)XMVectorReplicate(1.0f);
				Col4 emissiveColor = Col4(0.0f);
				Col4 ambientLightColor = (Col4)myLightSet->GetAmbient();

				// emissive と ambientとライトをマージする
				m_constantBuffer.emissiveColor = (emissiveColor + (ambientLightColor * diffuse)) * alphaVector;
				m_constantBuffer.specularColorAndPower = Col4(0, 0, 0, 1);

				// xyz = diffuse * alpha, w = alpha.
				m_constantBuffer.diffuseColor = Col4(XMVectorSelect(alphaVector, diffuse * alphaVector, g_XMSelect1110));

				auto mainLight = myLightSet->GetMainBaseLight();
				Vec3 calcLightDir = Vec3(mainLight.m_directional) * Vec3(-1.0f);

				Vec3 lightAt(myCamera->GetAt());

				Vec3 lightEye(calcLightDir);

				lightEye *= Vec3(Shadowmap::GetLightHeight());
				lightEye += lightAt;

				Vec4 LightEye4 = Vec4(lightEye, 1.0f);
				m_constantBuffer.lightPos = LightEye4;
				Vec4 eyePos4 = Vec4((Vec3)myCamera->GetEye(), 1.0f);
				m_constantBuffer.eyePos = eyePos4;
				XMMATRIX LightView, LightProj;
				//ライトのビューと射影を計算
				LightView = XMMatrixLookAtLH(
					Vec3(lightEye),
					Vec3(lightAt),
					Vec3(0, 1.0f, 0)
				);
				LightProj = XMMatrixOrthographicLH(Shadowmap::GetViewWidth(), Shadowmap::GetViewHeight(),
					Shadowmap::GetLightNear(), Shadowmap::GetLightFar());
				m_constantBuffer.lightView = Mat4x4(XMMatrixTranspose(LightView));
				m_constantBuffer.lightProjection = Mat4x4(XMMatrixTranspose(LightProj));

			}
		}
	}

	void BcSceneComp::OnCommitConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto pCurrentFrameResource = scene->GetCurrentFrameResource();
		//シーン
		memcpy(pCurrentFrameResource->m_baseConstantBufferSetVec[m_constantBufferIndex].m_pBaseConstantBufferWO,
			&m_constantBuffer, sizeof(m_constantBuffer));
	}



	void BcSceneComp::OnSceneDraw() {
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
			//PipelineState
			auto scenePipelineState = PipelineStatePool::GetPipelineState(L"BcPNTStaticShadow", true);
			pCommandList->SetPipelineState(scenePipelineState.Get());
			//Sampler
			UINT index = pBaseScene->GetSamplerIndex(L"LinearClamp");
			if (index == UINT_MAX) {
				throw BaseException(
					L"LinearClampサンプラーが特定できません。",
					L"Scene::ScenePass()"
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
