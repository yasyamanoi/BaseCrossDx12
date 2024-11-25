/*!
@file Character.h
@brief さまざまなオブジェクト
*/

#include "stdafx.h"

using namespace bsm;
using namespace basecross;
namespace basecross {
	//--------------------------------------------------------------------------------------
	//	FixedBoxクラス
	//--------------------------------------------------------------------------------------
	//構築と破棄
	FixedBox::FixedBox(
	) :GameObject()
	{
	}
	FixedBox::~FixedBox() {}

	//初期化（パラメータ付き）
	void FixedBox::OnCreate(ID3D12GraphicsCommandList* pCommandList, 
		const Vec3& scale, 
		const Quat& quaternion,
		const Vec3& position) {
		//メンバ変数の初期化
		m_scale = scale;
		m_quaternion = quaternion;
		m_firstPosition = m_position = position;
		m_totalTime = 0.0;
		//回転軸を乱数で決定
		Vec3 qtVec(
			Util::RandZeroToOne() - 0.5f,
			Util::RandZeroToOne() - 0.5f,
			Util::RandZeroToOne() - 0.5f
			);
		qtVec.normalize();
		m_spanQuat = Quat(qtVec, (Util::RandZeroToOne() - 0.5f) * 0.01f);
		m_spanQuat.normalize();
		//立方体作成の準備
		std::vector<VertexPositionNormalTexture> vertices;
		std::vector<uint32_t> indices;
		//立方体を作り出す
		MeshUtill::CreateCube(1.0f, vertices, indices);
		//メッシュ作成
		m_box = BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(pCommandList, vertices, indices);
		//テクスチャ作成
		auto texFile = App::GetRelativeAssetsDir() + L"wall.jpg";
		m_texture = BaseTexture::CreateTextureFlomFile(pCommandList, texFile);
		//シーンのポインタを取得
		auto pScene = BaseDevice::GetScene();
		UINT frameCount = pScene->GetFrameCount();
		m_constBuffParamVec.resize(frameCount);
		//定数バッファの作成
		for (UINT i = 0; i < frameCount; i++) {
			m_constBuffParamVec[i] = pScene->GetframeResources()[i]->CreateConstBuffParam<BaseConstant>();
		}
	}

	void FixedBox::OnUpdate(double elapsedTime) {
		//シーンのポインタを取得
		auto pScene = BaseDevice::GetScene();
		//トータル時間の変更
		m_totalTime += (float)elapsedTime;
		if (m_totalTime > XM_2PI) {
			m_totalTime = 0.0;
		}
		//行ったり来たりの計算
		m_position.x = (float)sin(m_totalTime * 2) + m_firstPosition.x;
		//現在の回転に追加
		Quat q = m_spanQuat;
		q.z = (float)elapsedTime;
		q.normalize();
		m_quaternion *= q;
		//定数バッファのUpdate
		auto perspecCamera = pScene->m_perspecCamera;
		auto lightSet = pScene->m_lightSet;
		//3D用の定数バッファ
		BaseConstant sceneConstantBuffer = {};
		//ワールド行列の取得
		Mat4x4 worldMat;
		worldMat.affineTransformation(
			m_scale,
			Vec3(0.0f),
			m_quaternion,
			m_position
		);
		sceneConstantBuffer.world = worldMat;
		//ワールド行列の転置
		sceneConstantBuffer.world.transpose();
		//ビュー行列の取得
		sceneConstantBuffer.view = perspecCamera->GetViewMatrix();
		//ビュー行列の転置
		sceneConstantBuffer.view.transpose();
		//射影行列の取得
		sceneConstantBuffer.projection = perspecCamera->GetProjMatrix();
		//射影行列の転置
		sceneConstantBuffer.projection.transpose();
		sceneConstantBuffer.emissive = Col4(0.5f);
		sceneConstantBuffer.diffuse = Col4(1.0f);
		sceneConstantBuffer.specular = Col4(0.0f);
		//テクスチャを使用する設定
		sceneConstantBuffer.activeFlg.x = 1;
		//ライトの設定
		auto mainLight = lightSet->GetMainBaseLight();
		sceneConstantBuffer.lightDir = mainLight.m_directional;
		sceneConstantBuffer.lightPos = Vec3(2.0f, 10.0f, 5.0f);
		sceneConstantBuffer.eyePos = perspecCamera->GetEye();
		//定数バッファのコミット
		auto frameIndex = pScene->GetFrameIndex();
		memcpy(
			m_constBuffParamVec[frameIndex].m_pConstantBufferWO,
			&sceneConstantBuffer,
			sizeof(sceneConstantBuffer)
		);
	}

	void FixedBox::OnSceneDraw(ID3D12GraphicsCommandList* pCommandList) {
		//シーンのポインタを取得
		auto pScene = BaseDevice::GetScene();
		//ルートシグネチャを取得
		auto rootSignatures = pScene->GetRootSignatures();
		auto frameIndex = pScene->GetFrameIndex();
		//これより描画処理
		//ルートシグネチャの設定
		pCommandList->SetGraphicsRootSignature(rootSignatures[RootSignature::ScenePass].Get());
		//パイプラインステートの設定
		auto scenePipeline = PipelineStatePool::GetPipelineState(L"PNTStatic");
		pCommandList->SetPipelineState(scenePipeline.Get());
		//定数バッファのセット
		pCommandList->SetGraphicsRootConstantBufferView(
			pScene->GetGpuSlotID(L"3d_b0"),
			m_constBuffParamVec[frameIndex].m_constantBuffer->GetGPUVirtualAddress()
		);
		//サンプラーの設定
		UINT index = pScene->GetSamplerIndex(L"Clamp");
		CD3DX12_GPU_DESCRIPTOR_HANDLE s0Handle(
			pScene->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			index,
			pScene->GetSamplerDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pScene->GetGpuSlotID(L"3d_s0"), s0Handle);
		//テクスチャの設定
		CD3DX12_GPU_DESCRIPTOR_HANDLE t0Handle(
			pScene->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			m_texture->GetSrvIndex(),
			pScene->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pScene->GetGpuSlotID(L"3d_t0"), t0Handle);
		//そのほかの必要な設定
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pCommandList->IASetVertexBuffers(0, 1, &m_box->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_box->GetIndexBufferView());
		pCommandList->RSSetViewports(1, &pScene->GetViewport());
		pCommandList->RSSetScissorRects(1, &pScene->GetScissorRect());
		pCommandList->OMSetRenderTargets(1, &pScene->GetCurrentBackBufferRtvCpuHandle(),
			FALSE, &pScene->GetDepthDsvs()[DepthGenPass::Scene]);
		//FixedBoxの描画
		pCommandList->DrawIndexedInstanced(m_box->GetNumIndices(), 1, 0, 0, 0);
	}


}
// end namespace basecross
