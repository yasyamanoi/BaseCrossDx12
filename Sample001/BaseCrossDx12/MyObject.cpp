/*!
@file MyObject.cpp
@brief ゲームオブジェクトクラス　実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	using namespace std;
	using namespace bsm;

	void MyObject::OnUpdateConstantBuffers(Scene* scene, shared_ptr<Stage>& stage) {
		auto& frameResources = scene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		auto& viewport = scene->GetViewport();
		shared_ptr<PerspecCamera> myCamera;
		shared_ptr<LightSet> myLightSet;
		auto gameStage = dynamic_pointer_cast<GameStage>(stage);
		if (gameStage) {
			myCamera = gameStage->GetMyCamera();
			myLightSet = gameStage->GetLightSet();

		}
		else {
			return;
		}
		//Transformコンポーネントを取り出す
		auto ptrTrans = GetComponent<Transform>();
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
		//シャドウマップのコンスタントバッファ
		{

			m_shadowConstantBuffer = {};

			auto lights = myLightSet;
			auto light = lights->GetMainBaseLight();

			//位置の取得
			Vec3 pos = param.position;
			Vec3 posSpan = (Vec3)light.m_directional;
			posSpan *= Vec3(Shadowmap::GetPosAdjustment());
			pos += posSpan;
			//ワールド行列の決定
			auto param_tmp = param;
			param_tmp.position = pos;

			auto world = XMMatrixAffineTransformation(
				param_tmp.scale,
				param_tmp.rotOrigin,
				param_tmp.quaternion,
				param_tmp.position
			);

			Vec3 lightDir = Vec3(light.m_directional) * Vec3(-1.0f);

			auto camera = myCamera;
			Vec3 lightAt(camera->GetAt());
			Vec3 lightDirTmp(lightDir* Vec3(Shadowmap::GetLightHeight()));
			Vec3 lightEye(lightAt + lightDirTmp);
			auto width = viewport.Width;
			auto height = viewport.Height;

			auto lightView
				= XMMatrixLookAtLH(lightEye, lightAt, Vec3(0, 1.0f, 0));

			auto lightProj =
				XMMatrixOrthographicLH(
					Shadowmap::GetViewWidth(),
					Shadowmap::GetViewHeight(),
					Shadowmap::GetLightNear(),
					Shadowmap::GetLightFar());
			m_shadowConstantBuffer.world = Mat4x4(XMMatrixTranspose(world));
			m_shadowConstantBuffer.view = Mat4x4(XMMatrixTranspose(lightView));
			m_shadowConstantBuffer.projection = Mat4x4(XMMatrixTranspose(lightProj));
		}
	}


	void MyObject::OnCommitConstantBuffers(Scene* scene, shared_ptr<Stage>& stage) {
		auto pCurrentFrameResource = scene->GetCurrentFrameResource();
		//シーン
		memcpy(pCurrentFrameResource->m_baseConstantBufferSetVec[m_constantBufferIndex].m_pBaseConstantBufferWO,
			&m_constantBuffer, sizeof(m_constantBuffer));
		//シャドウマップ
		memcpy(pCurrentFrameResource->m_baseConstantBufferSetVec[m_shadowConstantBufferIndex].m_pBaseConstantBufferWO,
			&m_shadowConstantBuffer, sizeof(m_shadowConstantBuffer));
	}


	void MyObject::OnCreate() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		//デフォルトのコンスタントバッファ
		for (size_t i = 0; i < BaseDevice::FrameCount; i++) {
			m_constantBufferIndex =
				frameResources[i]->AddBaseConstantBufferSet<BasicConstantBuffer>(pBaseDevice->GetD3D12Device());
		}
		//シャドウマップのコンスタントバッファ
		for (size_t i = 0; i < BaseDevice::FrameCount; i++) {
			m_shadowConstantBufferIndex =
				frameResources[i]->AddBaseConstantBufferSet<ShadowConstantBuffer>(pBaseDevice->GetD3D12Device());
		}
	}

	void MyObject::OnShadowDraw() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		auto pBaseScene = BaseScene::Get();
		auto pCurrentFrameResource = pBaseScene->GetCurrentFrameResource();
		//Cbv
		// Set shadow constant buffer.
		pCommandList->SetGraphicsRootConstantBufferView(pBaseScene->GetGpuSlotID(L"b0"),
			pCurrentFrameResource->m_baseConstantBufferSetVec[m_shadowConstantBufferIndex].m_baseConstantBuffer->GetGPUVirtualAddress());
		// Draw
		pCommandList->IASetVertexBuffers(0, 1, &m_mesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_mesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_mesh->GetNumIndices(), 1, 0, 0, 0);
	}
	void MyObject::OnSceneDraw() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pCurrentFrameResource = pBaseScene->GetCurrentFrameResource();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		auto& viewport = pBaseScene->GetViewport();
		auto& scissorRect = pBaseScene->GetScissorRect();
		auto depthDsvs = pBaseScene->GetDepthDsvs();
		//シェーダリソース（テクスチャ）のハンドルの設定
		CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(
			pBaseScene->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			m_texture->GetSrvIndex(),
			pBaseScene->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t1"), srvHandle);
		//Cbv
		// Set scene constant buffer.
		pCommandList->SetGraphicsRootConstantBufferView(pBaseScene->GetGpuSlotID(L"b0"),
			pCurrentFrameResource->m_baseConstantBufferSetVec[m_constantBufferIndex].m_baseConstantBuffer->GetGPUVirtualAddress());
		//Draw
		pCommandList->IASetVertexBuffers(0, 1, &m_mesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_mesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_mesh->GetNumIndices(), 1, 0, 0, 0);
	}


}
// end namespace basecross
