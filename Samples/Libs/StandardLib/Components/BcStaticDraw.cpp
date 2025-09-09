/*!
@file BcStaticDraw.cpp
@brief ベイシックスタティック描画コンポーネント　実体
*/

#include "stdafx.h"

namespace basecross {

	IMPLEMENT_DX12SHADER(BcVSPNTStaticPL, App::GetShadersDir() + L"BcVSPNTStaticPL.cso")
	IMPLEMENT_DX12SHADER(BcPSPNTPL, App::GetShadersDir() + L"BcPSPNTPL.cso")

	IMPLEMENT_DX12SHADER(BcVSPNTStaticPLShadow, App::GetShadersDir() + L"BcVSPNTStaticPLShadow.cso")
	IMPLEMENT_DX12SHADER(BcPSPNTPLShadow, App::GetShadersDir() + L"BcPSPNTPLShadow.cso")



	BcPNTStaticDraw::BcPNTStaticDraw(const std::shared_ptr<GameObject>& gameObjectPtr) :
		Component(gameObjectPtr),
		m_ownShadowActive(false),
		m_fogEnabled(true),
		m_fogStart(-25.0f),
		m_fogEnd(-40.0f),
		m_fogColor(0.8f, 0.8f, 0.8f, 1.0f),
		m_fogVector(0.0, 0.0, 1.0f)

	{
	}

	void BcPNTStaticDraw::OnCreate() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		//ベイシックコンスタントバッファ
		for (size_t i = 0; i < BaseDevice::FrameCount; i++) {
			m_constantBufferIndex =
				frameResources[i]->AddBaseConstantBufferSet<BasicConstant>(pBaseDevice->GetD3D12Device());
		}
		// シーンパイプラインステート
		{

			ComPtr<ID3D12PipelineState> defaultPipelineState
				= PipelineStatePool::GetPipelineState(L"BcPNTStatic");
			ComPtr<ID3D12PipelineState> defaultShadowPipelineState
				= PipelineStatePool::GetPipelineState(L"BcPNTStaticShadow");
			ComPtr<ID3D12PipelineState> alphaPipelineState
				= PipelineStatePool::GetPipelineState(L"BcPNTStaticAlpha");
			ComPtr<ID3D12PipelineState> alphaShadowPipelineState
				= PipelineStatePool::GetPipelineState(L"BcPNTStaticAlphaShadow");


			CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
			//カリング
			rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			ZeroMemory(&psoDesc, sizeof(psoDesc));
			psoDesc.InputLayout = { VertexPositionNormalTexture::GetVertexElement(), VertexPositionNormalTexture::GetNumElements() };
			psoDesc.pRootSignature = RootSignaturePool::GetRootSignature(L"BaseCrossDefault").Get();
			psoDesc.VS =
			{
				reinterpret_cast<UINT8*>(BcVSPNTStaticPL::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				BcVSPNTStaticPL::GetPtr()->GetShaderComPtr()->GetBufferSize()
			};
			psoDesc.PS =
			{
				reinterpret_cast<UINT8*>(BcPSPNTPL::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				BcPSPNTPL::GetPtr()->GetShaderComPtr()->GetBufferSize()
			};
			psoDesc.RasterizerState = rasterizerStateDesc;
			psoDesc.BlendState = BlendState::GetOpaqueBlend();
			psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc.Count = 1;
			//デフォルト影無し
			if (!defaultPipelineState) {
				ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&defaultPipelineState)));
				NAME_D3D12_OBJECT(defaultPipelineState);
				PipelineStatePool::AddPipelineState(L"BcPNTStatic", defaultPipelineState);
			}
			//アルファ影なし
			psoDesc.BlendState = BlendState::GetAlphaBlendEx();
			if (!alphaPipelineState) {
				ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&alphaPipelineState)));
				NAME_D3D12_OBJECT(alphaPipelineState);
				PipelineStatePool::AddPipelineState(L"BcPNTStaticAlpha", alphaPipelineState);
			}
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
			if (!defaultShadowPipelineState) {
				ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&defaultShadowPipelineState)));
				NAME_D3D12_OBJECT(defaultShadowPipelineState);
				PipelineStatePool::AddPipelineState(L"BcPNTStaticShadow", defaultShadowPipelineState);
			}
			psoDesc.BlendState = BlendState::GetAlphaBlendEx();
			//アルファ影あり
			if (!alphaShadowPipelineState) {
				ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&alphaShadowPipelineState)));
				NAME_D3D12_OBJECT(alphaShadowPipelineState);
				PipelineStatePool::AddPipelineState(L"BcPNTStaticAlphaShadow", alphaShadowPipelineState);
			}

		}
	}

	void BcPNTStaticDraw::OnUpdateConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto stage = std::dynamic_pointer_cast<Stage>(scene->GetActiveStage(true));
		auto& frameResources = scene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		auto& viewport = scene->GetViewport();
		std::shared_ptr<PerspecCamera> myCamera;
		std::shared_ptr<LightSet> myLightSet;
		if (!stage) {
			return;
		}
		auto gameObject = m_gameObject.lock();
		if (gameObject) {
			myCamera = std::dynamic_pointer_cast<PerspecCamera>(gameObject->GetCamera());
			myLightSet = gameObject->GetLightSet();


			//Transformコンポーネントを取り出す
			auto ptrTrans = gameObject->GetComponent<Transform>();
			auto& param = ptrTrans->GetTransParam();
			//シーンのコンスタントバッファ
			{

				//初期化
				m_constantBuffer = {};
				if (GetBaseTexture(0)) {
					m_constantBuffer.activeFlg.y = 1;
				}
				else {
					m_constantBuffer.activeFlg.y = 0;
				}
				m_constantBuffer.activeFlg.x = 3;

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

	void BcPNTStaticDraw::OnCommitConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto pCurrentFrameResource = scene->GetCurrentFrameResource();
		//シーン
		memcpy(pCurrentFrameResource->m_baseConstantBufferSetVec[m_constantBufferIndex].m_pBaseConstantBufferWO,
			&m_constantBuffer, sizeof(m_constantBuffer));
	}



	void BcPNTStaticDraw::OnSceneDraw() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pCurrentFrameResource = pBaseScene->GetCurrentFrameResource();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		auto& viewport = pBaseScene->GetViewport();
		auto& scissorRect = pBaseScene->GetScissorRect();
		auto depthDsvs = pBaseScene->GetDepthDsvs();
		auto depthGPUDsvs = pBaseScene->GetDepthSrvGpuHandles();
		
		auto CbvSrvUavDescriptorHeap = pBaseScene->GetCbvSrvUavDescriptorHeap();
		auto mesh = GetBaseMesh(0);
		auto texture = GetBaseTexture(0);
		if (!texture) {
			int a = 0;
		}
		if (mesh) {
			ComPtr<ID3D12PipelineState> defaultPipelineState
				= PipelineStatePool::GetPipelineState(L"BcPNTStatic", true);
			ComPtr<ID3D12PipelineState> defaultShadowPipelineState
				= PipelineStatePool::GetPipelineState(L"BcPNTStaticShadow", true);
			ComPtr<ID3D12PipelineState> alphaPipelineState
				= PipelineStatePool::GetPipelineState(L"BcPNTStaticAlpha", true);
			ComPtr<ID3D12PipelineState> alphaShadowPipelineState
				= PipelineStatePool::GetPipelineState(L"BcPNTStaticAlphaShadow", true);
			//null rv
			CD3DX12_GPU_DESCRIPTOR_HANDLE cbvSrvGpuNullHandle(CbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

			// set PipelineState and GetGpuSlotID(L"t0")
			if (GetGameObject()->IsAlphaActive()) {
				if (IsOwnShadowActive()) {
					pCommandList->SetPipelineState(alphaShadowPipelineState.Get());
					pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t0"), depthGPUDsvs[SceneEnums::DepthGenPass::Shadow]);        // Set the shadow texture as an SRV.
				}
				else {
					pCommandList->SetPipelineState(alphaPipelineState.Get());
					pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t0"), cbvSrvGpuNullHandle);        // Set the shadow texture as an SRV.
				}
			}
			else {
				if (IsOwnShadowActive()) {
					pCommandList->SetPipelineState(defaultShadowPipelineState.Get());
					pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t0"), depthGPUDsvs[SceneEnums::DepthGenPass::Shadow]);        // Set the shadow texture as an SRV.
				}
				else {
					pCommandList->SetPipelineState(defaultPipelineState.Get());
					pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t0"), cbvSrvGpuNullHandle);        // Set the shadow texture as an SRV.
				}

			}
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
			pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"s0"), samplerHandle);

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
			pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"s1"), samplerHandle2);
			//シェーダリソース（テクスチャ）のハンドルの設定
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
			//Cbv
			// Set scene constant buffer.
			pCommandList->SetGraphicsRootConstantBufferView(pBaseScene->GetGpuSlotID(L"b0"),
				pCurrentFrameResource->m_baseConstantBufferSetVec[m_constantBufferIndex].m_baseConstantBuffer->GetGPUVirtualAddress());
			//Draw
			pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			pCommandList->IASetVertexBuffers(0, 1, &mesh->GetVertexBufferView());
			pCommandList->IASetIndexBuffer(&mesh->GetIndexBufferView());
			pCommandList->DrawIndexedInstanced(mesh->GetNumIndices(), 1, 0, 0, 0);
		}

	}


}
// end namespace basecross
