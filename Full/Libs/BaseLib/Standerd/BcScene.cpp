/*!
@file BcSceneComp.cpp
@brief ベイシックシーンコンポーネント　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	using namespace bsm;

	IMPLEMENT_DX12SHADER(BcVSPNTStaticPL, App::GetShadersDir() + L"BcVSPNTStaticPL.cso")
	IMPLEMENT_DX12SHADER(BcPSPNTPL, App::GetShadersDir() + L"BcPSPNTPL.cso")

	IMPLEMENT_DX12SHADER(BcVSPNTStaticPLShadow, App::GetShadersDir() + L"BcVSPNTStaticPLShadow.cso")
	IMPLEMENT_DX12SHADER(BcPSPNTPLShadow, App::GetShadersDir() + L"BcPSPNTPLShadow.cso")



	BcScene::BcScene(const std::shared_ptr<GameObject>& gameObjectPtr) :
		Component(gameObjectPtr)
	{
		m_fogColor = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		m_fogVector = XMFLOAT3(0.0, 0.0, 1.0f);
	}

	void BcScene::OnCreate() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		//ベイシックコンスタントバッファ
		for (size_t i = 0; i < BaseDevice::FrameCount; i++) {
			m_constantBufferIndex =
				frameResources[i]->AddBaseConstantBufferSet<BasicConstantBuffer>(pBaseDevice->GetD3D12Device());
		}
		// シーンパイプラインステート
		{
			ComPtr<ID3D12PipelineState> PNTPipelineState
				= PipelineStatePool::GetPipelineState(L"BcPNTStaticShadow");
			//ラスタライザステート
			CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
			//カリング
			rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;
			//シーンパイプラインステート
			ComPtr<ID3D12PipelineState> bcPNTStaticShadowPipelineState;
			auto rootSignature = RootSignaturePool::GetRootSignature(L"BaseCrossDefault");
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			ZeroMemory(&psoDesc, sizeof(psoDesc));
			psoDesc.InputLayout = { VertexPositionNormalTexture::GetVertexElement(), VertexPositionNormalTexture::GetNumElements() };
			psoDesc.pRootSignature = rootSignature.Get();
			//デフォルト影あり
			psoDesc.BlendState = BlendState::GetOpaqueBlend();
			psoDesc.VS =
			{
				reinterpret_cast<UINT8*>(BcVSPNTStaticPLShadow::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				BcVSPNTStaticPLShadow::GetPtr()->GetShaderComPtr()->GetBufferSize()
			};
			psoDesc.PS =
			{
				reinterpret_cast<UINT8*>(BcPSPNTPLShadow::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				BcPSPNTPLShadow::GetPtr()->GetShaderComPtr()->GetBufferSize()
			};
			psoDesc.RasterizerState = rasterizerStateDesc;
			psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc.Count = 1;
			if (!PNTPipelineState) {
				ThrowIfFailed(pBaseDevice->GetD3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&bcPNTStaticShadowPipelineState)));
				NAME_D3D12_OBJECT(bcPNTStaticShadowPipelineState);
				PipelineStatePool::AddPipelineState(L"BcPNTStaticShadow", bcPNTStaticShadowPipelineState);
			}
		}
	}

	void BcScene::OnUpdateConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto gameStage = std::dynamic_pointer_cast<GameStage>(scene->GetActiveStage());
		auto& frameResources = scene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		auto& viewport = scene->GetViewport();
		std::shared_ptr<PerspecCamera> myCamera;
		std::shared_ptr<LightSet> myLightSet;
		if (gameStage) {
			myCamera = std::dynamic_pointer_cast<PerspecCamera>(gameStage->GetCamera());
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

				auto world = ptrTrans->GetWorldMatrix();
				auto view = myCamera->GetViewMatrix();
				auto proj = myCamera->GetProjMatrix();
				auto worldView = world * view;
				m_constantBuffer.worldViewProj = XMMatrixTranspose(XMMatrixMultiply(worldView, proj));
				//フォグの設定
				if (m_fogEnabled)
				{
					auto start = m_fogStart;
					auto end = m_fogEnd;
					if (start == end)
					{
						// Degenerate case: force everything to 100% fogged if start and end are the same.
						static const XMVECTORF32 fullyFogged = { 0, 0, 0, 1 };
						m_constantBuffer.fogVector = fullyFogged;
					}
					else
					{
						XMMATRIX worldViewTrans = worldView;
						// _13, _23, _33, _43
						XMVECTOR worldViewZ = XMVectorMergeXY(XMVectorMergeZW(worldViewTrans.r[0], worldViewTrans.r[2]),
							XMVectorMergeZW(worldViewTrans.r[1], worldViewTrans.r[3]));
						XMVECTOR wOffset = XMVectorSwizzle<1, 2, 3, 0>(XMLoadFloat(&start));
						m_constantBuffer.fogVector = (worldViewZ + wOffset) / (start - end);
					}
					m_constantBuffer.fogColor = m_fogColor;
				}
				else
				{
					m_constantBuffer.fogVector = g_XMZero;
					m_constantBuffer.fogColor = g_XMZero;
				}
				//ライトの決定
				for (int i = 0; i < myLightSet->GetNumLights(); i++) {
					m_constantBuffer.lightDirection[i] = myLightSet->GetLight(i).m_directional;
					m_constantBuffer.lightDiffuseColor[i] = myLightSet->GetLight(i).m_diffuseColor;
					m_constantBuffer.lightSpecularColor[i] = myLightSet->GetLight(i).m_specularColor;
				}
				//ワールド行列
				m_constantBuffer.world = XMMatrixTranspose(world);

				XMMATRIX worldInverse = XMMatrixInverse(nullptr, world);
				m_constantBuffer.worldInverseTranspose[0] = worldInverse.r[0];
				m_constantBuffer.worldInverseTranspose[1] = worldInverse.r[1];
				m_constantBuffer.worldInverseTranspose[2] = worldInverse.r[2];

				XMMATRIX viewInverse = XMMatrixInverse(nullptr, view);
				m_constantBuffer.eyePosition = viewInverse.r[3];

				XMVECTOR diffuse = Col4(1.0f);
				XMVECTOR alphaVector = XMVectorReplicate(1.0);
				XMVECTOR emissiveColor = Col4(1.0f);
				XMVECTOR ambientLightColor = myLightSet->GetAmbient();
				// emissive と ambientとライトをマージする
				m_constantBuffer.emissiveColor = (emissiveColor + ambientLightColor * diffuse) * alphaVector;
				m_constantBuffer.specularColorAndPower = Col4(0, 0, 0, 1);

				
				auto camaraPtr = myCamera;
				auto mainLight = myLightSet->GetMainBaseLight();
				bsm::Vec3 calcLightDir(mainLight.m_directional * -1.0);
				bsm::Vec3 lightAt(camaraPtr->GetAt());
				bsm::Vec3 lightEye(calcLightDir);
				lightEye *= Shadowmap::GetLightHeight();
				lightEye = lightAt + lightEye;
				bsm::Vec4 LightEye4(lightEye, 1.0f);
				LightEye4.w = 1.0f;
				m_constantBuffer.lightPos = LightEye4;
				bsm::Vec4 eyePos4(camaraPtr->GetEye(), 1.0f);
				eyePos4.w = 1.0f;
				m_constantBuffer.eyePos = eyePos4;
				bsm::Mat4x4 LightView, LightProj;
				//ライトのビューと射影を計算
				LightView = XMMatrixLookAtLH(lightEye, lightAt, bsm::Vec3(0, 1.0f, 0));
				LightProj = XMMatrixOrthographicLH(Shadowmap::GetViewWidth(), Shadowmap::GetViewHeight(),
					Shadowmap::GetLightNear(), Shadowmap::GetLightFar());
				m_constantBuffer.lightView = LightView.transpose();
				m_constantBuffer.lightProjection = LightProj.transpose();
			}
		}
	}

	void BcScene::OnCommitConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto pCurrentFrameResource = scene->GetCurrentFrameResource();
		//シーン
		memcpy(pCurrentFrameResource->m_baseConstantBufferSetVec[m_constantBufferIndex].m_pBaseConstantBufferWO,
			&m_constantBuffer, sizeof(m_constantBuffer));
	}



	void BcScene::OnSceneDraw() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pCurrentFrameResource = pBaseScene->GetCurrentFrameResource();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		auto& viewport = pBaseScene->GetViewport();
		auto& scissorRect = pBaseScene->GetScissorRect();
		auto depthDsvs = pBaseScene->GetDepthDsvs();
		auto mesh = GetBaseMesh(0);
		auto texture = GetBaseTexture(0);
		if (mesh && texture) {
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
