/*!
@file Character.h
@brief さまざまなオブジェクト
*/

#include "stdafx.h"

using namespace bsm;
using namespace basecross;
namespace basecross {

	//--------------------------------------------------------------------------------------
	//	PcSpriteクラス
	//--------------------------------------------------------------------------------------
	//構築と破棄
	PcSprite::PcSprite() :
		GameObject()
	{
	}
	PcSprite::~PcSprite() {}

	//初期化（パラメータ付き）
	void PcSprite::OnCreate(ID3D12GraphicsCommandList* pCommandList, 
		const Vec3&  scale,
		const Vec3& position
	) {
		m_scale = scale;
		m_position = position;
		m_totalTime = 0.0;
		float helfSize = 0.5f;
		//頂点配列
		m_backupVertices = {
			{ VertexPositionColor(Vec3(-helfSize, helfSize, 0),Col4(1.0f,0.0f,0.0f,0.0f)) },
			{ VertexPositionColor(Vec3(helfSize, helfSize, 0), Col4(0.0f, 1.0f, 0.0f, 0.0f)) },
			{ VertexPositionColor(Vec3(-helfSize, -helfSize, 0), Col4(0.0f, 0.0f, 1.0f, 0.0f)) },
			{ VertexPositionColor(Vec3(helfSize, -helfSize, 0), Col4(0.0f, 0.0f, 0, 0.0f)) },
		};
		//インデックス配列
		std::vector<uint32_t> indices = { 0, 1, 2, 1, 3, 2 };
		m_spriteMesh = BaseMesh::CreateBaseMesh<VertexPositionColor>(pCommandList, m_backupVertices, indices);

		auto pBaseScene = Scene::Get();
		UINT frameCount = pBaseScene->GetFrameCount();
		m_constBuffParamVec.resize(frameCount);
		for (UINT i = 0; i < frameCount; i++) {
			m_constBuffParamVec[i] = pBaseScene->GetframeResources()[i]->CreateConstBuffParam<Base2DConstant>();
		}
	}

	void PcSprite::OnUpdate(double elapsedTime) {
		auto pBaseScene = Scene::Get();
		//トータル時間の変更
		m_totalTime += elapsedTime;
		if (m_totalTime > XM_2PI) {
			m_totalTime = 0.0;
		}
		//バックアップ頂点の変更
		for (auto& vertex : m_backupVertices) {
			vertex.color.x += (float)elapsedTime;
			if (vertex.color.x > 1.0f) {
				vertex.color.x = 0.0f;
			}
		}
		//コンスタントバッファのUpdate
		Base2DConstant constantBuffer = {};
		Mat4x4 worldMat;
		m_scale.z = 1.0f;
		m_position.x = (float)sin(m_totalTime) * 200.0f;
		worldMat.affineTransformation(
			m_scale,
			Vec3(0.0f),
			Vec3(0.0f),
			m_position
		);
		auto w = App::GetWidth();
		auto h = App::GetHeight();
		Mat4x4 ProjMatrix = XMMatrixOrthographicLH((float)w, (float)h, 0.0, 1.0);
		worldMat *= ProjMatrix;
		//2Dはtransposeしない
//		worldMat.transpose();
		constantBuffer.transform = worldMat;
		constantBuffer.emissive = Col4(0.5f);
		constantBuffer.diffuse = Col4(1.0f);
		//コンスタントバッファのコミット
		auto frameIndex = pBaseScene->GetFrameIndex();
		memcpy(
			m_constBuffParamVec[frameIndex].m_pConstantBufferWO,
			&constantBuffer,
			sizeof(constantBuffer)
		);
	}

	void PcSprite::OnSceneDraw(ID3D12GraphicsCommandList* pCommandList) {
		auto pBaseScene = Scene::Get();
		auto rootSignatures = pBaseScene->GetRootSignatures();
		auto frameIndex = pBaseScene->GetFrameIndex();
		//頂点情報の変更
		m_spriteMesh->UpdateVirtexBuffer<VertexPositionColor>(pCommandList, m_backupVertices);
		//これより描画処理
		//ルートシグネチャの設定
		pCommandList->SetGraphicsRootSignature(rootSignatures[RootSignature::SpritePass].Get());
		//パイプラインステートの設定
		auto scenePipeline = PipelineStatePool::GetPipelineState(L"PCSprite");
		pCommandList->SetPipelineState(scenePipeline.Get());
		//定数バッファのセット
		pCommandList->SetGraphicsRootConstantBufferView(
			pBaseScene->GetGpuSlotID(L"2d_b0"),
			m_constBuffParamVec[frameIndex].m_constantBuffer->GetGPUVirtualAddress()
		);
		//そのほかの必要な設定
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pCommandList->IASetVertexBuffers(0, 1, &m_spriteMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_spriteMesh->GetIndexBufferView());
		pCommandList->RSSetViewports(1, &pBaseScene->GetViewport());
		pCommandList->RSSetScissorRects(1, &pBaseScene->GetScissorRect());
		pCommandList->OMSetRenderTargets(1, &pBaseScene->GetCurrentBackBufferRtvCpuHandle(),
			FALSE, &pBaseScene->GetDepthDsvs()[DepthGenPass::Scene]);
		//PcSpriteの描画
		pCommandList->DrawIndexedInstanced(m_spriteMesh->GetNumIndices(), 1, 0, 0, 0);
	}



}
// end namespace basecross
