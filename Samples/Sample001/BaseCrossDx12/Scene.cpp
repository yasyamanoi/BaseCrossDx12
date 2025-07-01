/*!
@file Scene.cpp
@brief シーンクラス　実体
*/
#include "stdafx.h"
#include "Project.h"

namespace basecross {

	IMPLEMENT_DX12SHADER(SpVSPCStatic, App::GetShadersDir() + L"SpVSPCStatic.cso")
		IMPLEMENT_DX12SHADER(SpPSPCStatic, App::GetShadersDir() + L"SpPSPCStatic.cso")

		//--------------------------------------------------------------------------------------
		// シーン
		//--------------------------------------------------------------------------------------
		Scene::Scene(UINT frameCount, PrimDevice* pPrimDevice) :
		BaseScene(frameCount, pPrimDevice),
		m_totalTime(0.0)
	{
	}
	Scene::~Scene()
	{
	}

	void Scene::CreateAssetResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) {
		//カメラとライトの設定
		m_camera = ObjectFactory::Create<PerspecCamera>();
		m_camera->SetEye(Vec3(0, 5.0f, -10.0f));
		m_camera->SetAt(Vec3(0, 0, 0));
		m_lightSet = ObjectFactory::Create<LightSet>();
		//パラメータの初期化
		m_param.scale = Vec3(5.0f, 5.0f, 1.0f);
		auto quat = XMQuaternionIdentity();
		m_param.quaternion = Quat(quat);
		m_param.position = Vec3(0.0f, 0.0f, 0.0f);
		//頂点の作成
		std::vector<VertexPositionColor> vertices = {
			{ VertexPositionColor(Vec3(-0.5f, -0.5f, 0.0f),Col4(1.0f, 0.0f, 0.0f,1.0f)) },
			{ VertexPositionColor(Vec3(0.0f, 0.5f, 0.0f), Col4(0.0f, 1.0f, 0.0f,1.0f)) },
			{ VertexPositionColor(Vec3(0.5f, -0.5f, 0.0f), Col4(0.0f, 0.0f, 1.0f,1.0f)) }
		};
		//インデックス配列
		std::vector<uint32_t> indices = { 0, 1, 2 };
		//メッシュの作成
		m_mesh = BaseMesh::CreateBaseMesh<VertexPositionColor>(pCommandList, vertices, indices);
		//コンスタントバッファの作成
		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		//デフォルトのコンスタントバッファ
		for (size_t i = 0; i < BaseDevice::FrameCount; i++) {
			m_constantBufferIndex =
				frameResources[i]->AddBaseConstantBufferSet<SimpleConstant>(pBaseDevice->GetD3D12Device());
		}
		// シーン描画のパイプラインステートの作成
		{
			ComPtr<ID3D12PipelineState> spPCPipelineState
				= PipelineStatePool::GetPipelineState(L"SpPCStatic");

			CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
			//カリング
			rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			ZeroMemory(&psoDesc, sizeof(psoDesc));
			psoDesc.InputLayout = { VertexPositionColor::GetVertexElement(), VertexPositionColor::GetNumElements() };
			psoDesc.pRootSignature = RootSignaturePool::GetRootSignature(L"BaseCrossDefault").Get();
			psoDesc.VS =
			{
				reinterpret_cast<UINT8*>(SpVSPCStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				SpVSPCStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
			};
			psoDesc.PS =
			{
				reinterpret_cast<UINT8*>(SpPSPCStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				SpPSPCStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
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
			//spPCPipelineStateパイプラインステート作成
			if (!spPCPipelineState) {
				ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&spPCPipelineState)));
				NAME_D3D12_OBJECT(spPCPipelineState);
				PipelineStatePool::AddPipelineState(L"SpPCStatic", spPCPipelineState);
			}
		}
	}

	void Scene::Update(double elapsedTime) {
		m_totalTime += elapsedTime;
		if (m_totalTime >= XM_2PI) {
			m_totalTime = 0.0;
		}
		Quat spanQt(Vec3(0.0f, 1.0f, 0.0f), (float)(elapsedTime * 2));
		Quat quaternion(m_param.quaternion);
		quaternion *= spanQt;
		m_param.quaternion = quaternion;
		UpdateConstantBuffers();
		CommitConstantBuffers();
	}

	void Scene::UpdateConstantBuffers() {
		//初期化
		m_constantBuffer = {};
		Mat4x4 world;
		world.affineTransformation(
			m_param.scale,
			m_param.rotOrigin,
			m_param.quaternion,
			m_param.position
		);
		//転置する
		world.transpose();
		//ビューと射影行列を得る
		Mat4x4 viewMat = m_camera->GetViewMatrix();
		//転置する
		viewMat.transpose();
		Mat4x4 projMat = m_camera->GetProjMatrix();
		//転置する
		projMat.transpose();
		m_constantBuffer.world = world;
		m_constantBuffer.view = viewMat;
		m_constantBuffer.projection = projMat;
		//エミッシブ
		m_constantBuffer.emissive = Col4(0.5f, 0.5f, 0.5f, 1.0f);
		//デフィーズ
		m_constantBuffer.diffuse = Col4(0.5f, 0.5f, 0.5f, 1.0f);
		//スペキュラー
		m_constantBuffer.specular = Col4(0.5f, 0.5f, 0.5f, 1.0f);
		//ライティング
		auto index = m_lightSet->GetMainIndex();
		auto light = m_lightSet->GetLight(index);
		m_constantBuffer.lightDir = light.m_directional;
		m_constantBuffer.lightDir.w = 1.0f;
		m_constantBuffer.eyePos = m_camera->GetEye();
		m_constantBuffer.eyePos.w = 1.0f;
		m_constantBuffer.activeFlg.x = 0;
	}

	void Scene::CommitConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto pCurrentFrameResource = scene->GetCurrentFrameResource();
		//シーン
		memcpy(pCurrentFrameResource->m_baseConstantBufferSetVec[m_constantBufferIndex].m_pBaseConstantBufferWO,
			&m_constantBuffer, sizeof(m_constantBuffer));
	}

	void Scene::UpdateUI(std::unique_ptr<UILayer>& uiLayer) {
		auto device = BaseDevice::GetBaseDevice();
		//1秒間に１回更新される安定したfpsを得る
		auto fps = device->GetStableFps();
		//1秒間に１回更新される安定したelapsedTimeを得る
		auto elapsedTime = device->GetStableElapsedTime();
		std::wstring uiText = L"";
		wchar_t buff[512];
		swprintf_s(buff, 500, L"FPS: %.1f\n", fps);
		uiText = buff;
		swprintf_s(buff, 500, L"ElapsedTime: %.6f\n", elapsedTime);
		uiText += buff;
		uiLayer->UpdateLabels(uiText);
	}

	void Scene::ScenePass(ID3D12GraphicsCommandList* pCommandList)
	{
		auto& viewport = GetViewport();
		auto& scissorRect = GetScissorRect();
		auto depthDsvs = GetDepthDsvs();
		auto pBaseScene = BaseScene::Get();
		auto pCurrentFrameResource = pBaseScene->GetCurrentFrameResource();
		//RootSignatureの設定
		auto rootSignature = RootSignaturePool::GetRootSignature(L"BaseCrossDefault", true);
		pCommandList->SetGraphicsRootSignature(rootSignature.Get());
		//ViewportsとScissorRectsの設定
		pCommandList->RSSetViewports(1, &viewport);
		pCommandList->RSSetScissorRects(1, &scissorRect);
		//RenderTargetsの設定
		pCommandList->OMSetRenderTargets(1, &GetCurrentBackBufferRtvCpuHandle(), FALSE, &depthDsvs[SceneEnums::DepthGenPass::Scene]);
		//オブジェクト描画
		auto CbvSrvUavDescriptorHeap = pBaseScene->GetCbvSrvUavDescriptorHeap();
		if (m_mesh) {
			//パイプラインステートの取得
			ComPtr<ID3D12PipelineState> defaultPipelineState
				= PipelineStatePool::GetPipelineState(L"SpPCStatic", true);
			pCommandList->SetPipelineState(defaultPipelineState.Get());
			//テクスチャスロットにnullの設定
			CD3DX12_GPU_DESCRIPTOR_HANDLE cbvSrvGpuNullHandle(CbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
			pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t0"), cbvSrvGpuNullHandle);
			//Cbv（シーンのコンスタントバッファ）の設定
			pCommandList->SetGraphicsRootConstantBufferView(pBaseScene->GetGpuSlotID(L"b0"),
				pCurrentFrameResource->m_baseConstantBufferSetVec[m_constantBufferIndex].m_baseConstantBuffer->GetGPUVirtualAddress());
			//描画
			pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			pCommandList->IASetVertexBuffers(0, 1, &m_mesh->GetVertexBufferView());
			pCommandList->IASetIndexBuffer(&m_mesh->GetIndexBufferView());
			pCommandList->DrawIndexedInstanced(m_mesh->GetNumIndices(), 1, 0, 0, 0);
		}
	}

}
// end namespace basecross
