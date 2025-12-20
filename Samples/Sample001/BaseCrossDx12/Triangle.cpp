/*!
@file Triangle.cpp
@brief 三角形クラス実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#include "stdafx.h"

namespace basecross {

	IMPLEMENT_DX12SHADER(SpVSPCStatic, App::GetShadersDir() + L"SpVSPCStatic.cso")
	IMPLEMENT_DX12SHADER(SpPSPCStatic, App::GetShadersDir() + L"SpPSPCStatic.cso")

	void Triangle::OnCreate() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
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
		for (size_t i = 0; i < BaseDevice::FrameCount; i++) {
			m_constantBufferIndex =
				frameResources[i]->AddBaseConstantBufferSet<SimpleConstant>(pBaseDevice->GetD3D12Device());
		}
		//CPU側コンスタントバッファの作成
		CreateConstantBuffer();
		// シーン描画のパイプラインステートの作成
		ComPtr<ID3D12PipelineState> spPCPipelineState
			= PipelineStatePool::GetPipelineState(L"SpPCStatic");
		if(!spPCPipelineState)
		{

			CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
			//カリングしない
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
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&spPCPipelineState)));
			NAME_D3D12_OBJECT(spPCPipelineState);
			PipelineStatePool::AddPipelineState(L"SpPCStatic", spPCPipelineState);
		}
	}

	void Triangle::CreateConstantBuffer() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		auto& viewport = scene->GetViewport();

		auto camera = std::dynamic_pointer_cast<PerspecCamera>(scene->GetCamera());
		auto lightSet = scene->GetLightSet();

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
		Mat4x4 viewMat = camera->GetViewMatrix();
		//転置する
		viewMat.transpose();
		Mat4x4 projMat = camera->GetProjMatrix();
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
		auto index = lightSet->GetMainIndex();
		auto light = lightSet->GetLight(index);
		m_constantBuffer.lightDir = light.m_directional;
		m_constantBuffer.lightDir.w = 1.0f;
		m_constantBuffer.eyePos = camera->GetEye();
		m_constantBuffer.eyePos.w = 1.0f;
		m_constantBuffer.activeFlg.x = 0;
	}


	void Triangle::OnUpdate(double elapsedTime) {
		m_totalTime += elapsedTime;
		if (m_totalTime >= XM_2PI) {
			m_totalTime = 0.0;
		}
		Quat spanQt(Vec3(0.0f, 1.0f, 0.0f), (float)(elapsedTime * 2));
		Quat quaternion(m_param.quaternion);
		quaternion *= spanQt;
		m_param.quaternion = quaternion;
	}


	void Triangle::OnUpdateConstantBuffers() {
		Mat4x4 world;
		world.affineTransformation(
			m_param.scale,
			m_param.rotOrigin,
			m_param.quaternion,
			m_param.position
		);
		//転置する
		world.transpose();
		m_constantBuffer.world = world;
	}


	void Triangle::OnCommitConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto pCurrentFrameResource = scene->GetCurrentFrameResource();
		memcpy(pCurrentFrameResource->m_baseConstantBufferSetVec[m_constantBufferIndex].m_pBaseConstantBufferWO,
			&m_constantBuffer, sizeof(m_constantBuffer));
	}


	void Triangle::OnSceneDraw(ID3D12GraphicsCommandList* pCommandList)
	{
		//メッシュが有効な時のみ実行
		if (m_mesh) {
			//シーンの取り出し
			auto pBaseScene = BaseScene::Get();
			//現在のフレームの取り出し
			auto pCurrentFrameResource = pBaseScene->GetCurrentFrameResource();
			//ディスクプリタヒープ（CbvSrvUav用）の取り出し
			auto CbvSrvUavDescriptorHeap = pBaseScene->GetCbvSrvUavDescriptorHeap();
			//パイプラインステートの取得
			ComPtr<ID3D12PipelineState> defaultPipelineState
				= PipelineStatePool::GetPipelineState(L"SpPCStatic", true);
			//パイプラインステートの設定
			pCommandList->SetPipelineState(defaultPipelineState.Get());
			//テクスチャスロットにnullの設定
			CD3DX12_GPU_DESCRIPTOR_HANDLE cbvSrvGpuNullHandle(CbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
			pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t0"), cbvSrvGpuNullHandle);
			//コンスタントバッファをGPUスロット（b0）に設定
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
//namespace basecross
