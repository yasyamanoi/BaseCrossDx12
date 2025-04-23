/*!
@file MyObject.cpp
@brief ゲームオブジェクトクラス　実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	using namespace std;

	void MyObject::UpdateConstantBuffers(Scene* scene) {
		//シーンのコンスタントバッファ
		{
			auto& frameResources = scene->GetFrameResources();
			auto pBaseDevice = BaseDevice::GetBaseDevice();
			auto& viewport = scene->GetViewport();

			auto myCamera = scene->GetMyCamera();
			auto myLightSet = scene->GetLightSet();

			//初期化
			m_constantBuffer = {};
			m_constantBuffer.activeFlg.x = 3;
			m_constantBuffer.activeFlg.y = 1;
			//ワールド行列の設定
			auto world = bsm::affineTransformation(m_param);
			auto view = bsm::makeXMMat(myCamera->GetViewMatrix());
			auto proj = bsm::makeXMMat(myCamera->GetProjMatrix());
			auto worldView = world * view;
			m_constantBuffer.worldViewProj = bsm::makeF4x4(XMMatrixTranspose(XMMatrixMultiply(worldView, proj)));
			//フォグの設定
			if (m_fogEnabled)
			{
				auto start = m_fogStart;
				auto end = m_fogEnd;
				if (start == end)
				{
					// Degenerate case: force everything to 100% fogged if start and end are the same.
					static const XMVECTORF32 fullyFogged = { 0, 0, 0, 1 };
					m_constantBuffer.fogVector = bsm::makeF4(fullyFogged);
				}
				else
				{
					XMMATRIX worldViewTrans = worldView;
					// _13, _23, _33, _43
					XMVECTOR worldViewZ = XMVectorMergeXY(XMVectorMergeZW(worldViewTrans.r[0], worldViewTrans.r[2]),
						XMVectorMergeZW(worldViewTrans.r[1], worldViewTrans.r[3]));
					XMVECTOR wOffset = XMVectorSwizzle<1, 2, 3, 0>(XMLoadFloat(&start));
					m_constantBuffer.fogVector = bsm::makeF4((worldViewZ + wOffset) / (start - end));
				}
				m_constantBuffer.fogColor = m_fogColor;
			}
			else
			{
				m_constantBuffer.fogVector = bsm::makeF4(g_XMZero);
				m_constantBuffer.fogColor = bsm::makeF4(g_XMZero);
			}

			//ライトの決定
			for (int i = 0; i < myLightSet->GetNumLights(); i++) {
				m_constantBuffer.lightDirection[i] = bsm::F3ToF4(myLightSet->GetLight(i).m_directional);
				m_constantBuffer.lightDiffuseColor[i] = myLightSet->GetLight(i).m_diffuseColor;
				m_constantBuffer.lightSpecularColor[i] = myLightSet->GetLight(i).m_specularColor;
			}
			//ワールド行列
			m_constantBuffer.world = bsm::makeF4x4(XMMatrixTranspose(world));

			XMMATRIX worldInverse = XMMatrixInverse(nullptr, world);
			m_constantBuffer.worldInverseTranspose[0] = bsm::makeF4(worldInverse.r[0]);
			m_constantBuffer.worldInverseTranspose[1] = bsm::makeF4(worldInverse.r[1]);
			m_constantBuffer.worldInverseTranspose[2] = bsm::makeF4(worldInverse.r[2]);

			XMMATRIX viewInverse = XMMatrixInverse(nullptr, view);
			m_constantBuffer.eyePosition = bsm::makeF4(viewInverse.r[3]);

			XMVECTOR diffuse = bsm::makeXM(1);
			XMVECTOR alphaVector = XMVectorReplicate(1.0);
			XMVECTOR emissiveColor = bsm::makeXM(0.0f);
			XMVECTOR ambientLightColor = bsm::makeXM(myLightSet->GetAmbient());
			// emissive と ambientとライトをマージする
			m_constantBuffer.emissiveColor = bsm::makeF4((emissiveColor + ambientLightColor * diffuse) * alphaVector);
			m_constantBuffer.specularColorAndPower = XMFLOAT4(0, 0, 0, 1);

			// xyz = diffuse * alpha, w = alpha.
			m_constantBuffer.diffuseColor = bsm::makeF4(XMVectorSelect(alphaVector, diffuse * alphaVector, g_XMSelect1110));

			auto mainLight = myLightSet->GetMainBaseLight();
			XMFLOAT3 calcLightDir = bsm::f3multi(mainLight.m_directional, -1.0f);
			XMFLOAT3 lightAt(myCamera->GetAt());
			XMFLOAT3 lightEye(calcLightDir);
			lightEye = bsm::f3multi(lightEye, Shadowmap::GetLightHeight());
			lightEye = bsm::f3add(lightAt, lightEye);

			XMFLOAT4 LightEye4 = bsm::F3ToF4(lightEye, 1.0f);
			LightEye4.w = 1.0f;
			m_constantBuffer.lightPos = LightEye4;
			XMFLOAT4 eyePos4 = bsm::F3ToF4(myCamera->GetEye(), 1.0f);
			eyePos4.w = 1.0f;
			m_constantBuffer.eyePos = eyePos4;
			XMMATRIX LightView, LightProj;
			//ライトのビューと射影を計算
			LightView = XMMatrixLookAtLH(
				bsm::makeXM(lightEye),
				bsm::makeXM(lightAt),
				bsm::makeXM(0, 1.0f, 0)
			);
			LightProj = XMMatrixOrthographicLH(Shadowmap::GetViewWidth(), Shadowmap::GetViewHeight(),
				Shadowmap::GetLightNear(), Shadowmap::GetLightFar());
			m_constantBuffer.lightView = bsm::makeF4x4(XMMatrixTranspose(LightView));
			m_constantBuffer.lightProjection = bsm::makeF4x4(XMMatrixTranspose(LightProj));
		}
		//シャドウマップのコンスタントバッファ
		{
			auto& frameResources = scene->GetFrameResources();
			auto pBaseDevice = BaseDevice::GetBaseDevice();
			auto& viewport = scene->GetViewport();

			auto myCamera = scene->GetMyCamera();
			myCamera->CalculateMatrix();
			auto myLightSet = scene->GetLightSet();

			m_shadowConstantBuffer = {};

			auto lights = myLightSet;
			auto light = lights->GetMainBaseLight();

			//位置の取得
			XMFLOAT3 pos = m_param.position;
			XMFLOAT3 posSpan = light.m_directional;
			posSpan = bsm::f3multi(posSpan, Shadowmap::GetPosAdjustment());
			pos = bsm::f3add(pos, posSpan);
			//ワールド行列の決定
			auto param_tmp = m_param;
			param_tmp.position = pos;
			XMMATRIX world = bsm::affineTransformation(param_tmp);


			XMFLOAT3 lightDir = bsm::f3multi(light.m_directional, -1.0f);

			auto camera = myCamera;
			XMFLOAT3 lightAt = camera->GetAt();
			XMFLOAT3 lightDirTmp = bsm::f3multi(lightDir, Shadowmap::GetLightHeight());
			XMFLOAT3 lightEye = bsm::f3add(lightAt, lightDirTmp);
			auto width = viewport.Width;
			auto height = viewport.Height;

			auto lightView 
				= XMMatrixLookAtLH(bsm::makeXM(lightEye), bsm::makeXM(lightAt), bsm::makeXM(XMFLOAT3(0, 1.0f, 0)));

			auto lightProj = 
				XMMatrixOrthographicLH(
					Shadowmap::GetViewWidth(),
					Shadowmap::GetViewHeight(), 
					Shadowmap::GetLightNear(),
					Shadowmap::GetLightFar());
			m_shadowConstantBuffer.world = bsm::makeF4x4(XMMatrixTranspose(world));
			m_shadowConstantBuffer.view = bsm::makeF4x4(XMMatrixTranspose(lightView));
			m_shadowConstantBuffer.projection = bsm::makeF4x4(XMMatrixTranspose(lightProj));


		}
	}


	void MyObject::CommitConstantBuffers(Scene* scene) {
		auto pCurrentFrameResource = scene->GetCurrentFrameResource();
		//シーン
		memcpy(pCurrentFrameResource->m_baseConstantBufferSetVec[m_constantBufferIndex].m_pBaseConstantBufferWO,
			&m_constantBuffer, sizeof(m_constantBuffer));
		//シャドウマップ
		memcpy(pCurrentFrameResource->m_baseConstantBufferSetVec[m_shadowConstantBufferIndex].m_pBaseConstantBufferWO,
			&m_shadowConstantBuffer, sizeof(m_shadowConstantBuffer));
	}

	void MyObject::OnCreate(ID3D12GraphicsCommandList* pCommandList) {
		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		//デフォルトのコンスタントバッファ
		for (size_t i = 0; i < BaseDevice::FrameCount; i++) {
			m_constantBufferIndex = 
				frameResources[i]->AddBaseConstantBufferSet<BaseConstantBuffer>(pBaseDevice->GetD3D12Device());
		}
		//シャドウマップのコンスタントバッファ
		for (size_t i = 0; i < BaseDevice::FrameCount; i++) {
			m_shadowConstantBufferIndex =
				frameResources[i]->AddBaseConstantBufferSet<ShadowConstant>(pBaseDevice->GetD3D12Device());
		}
	}


	void MyObject::OnShadowDraw(ID3D12GraphicsCommandList* pCommandList) {
		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pCurrentFrameResource = pBaseScene->GetCurrentFrameResource();

		//Cbv
		pCommandList->SetGraphicsRootConstantBufferView(pBaseScene->GetGpuSlotID(L"b0"),
			pCurrentFrameResource->m_baseConstantBufferSetVec[m_shadowConstantBufferIndex].m_baseConstantBuffer->GetGPUVirtualAddress()); // Set shadow constant buffer.

		pCommandList->IASetVertexBuffers(0, 1, &m_mesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_mesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_mesh->GetNumIndices(), 1, 0, 0, 0);
	
	}
	void MyObject::OnSceneDraw(ID3D12GraphicsCommandList* pCommandList) {

		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pCurrentFrameResource = pBaseScene->GetCurrentFrameResource();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		auto& viewport = pBaseScene->GetViewport();
		auto& scissorRect = pBaseScene->GetScissorRect();
		auto depthDsvs = pBaseScene->GetDepthDsvs();

		//シェーダリソースのハンドルの設定
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
	void MyObject::OnPostprocessDraw(ID3D12GraphicsCommandList* pCommandList) {}
	void MyObject::OnDestroy() {}



	//--------------------------------------------------------------------------------------
	// 四角のオブジェクト
	//--------------------------------------------------------------------------------------
	WallBox::WallBox(const TransParam& param):
		MyObject(param),
		m_totalTime(0.0)
	{}
	WallBox::~WallBox(){}

	void WallBox::OnCreate(ID3D12GraphicsCommandList* pCommandList){
		MyObject::OnCreate(pCommandList);
		//メッシュ
		m_mesh = BaseMesh::CreateCube(pCommandList, 1.0f);
		//テクスチャ
		auto texFile = App::GetRelativeAssetsDir() + L"wall.jpg";
		m_texture = BaseTexture::CreateTextureFlomFile(pCommandList, texFile);
	}

	void WallBox::OnUpdate(double elapsedTime) {
		m_totalTime += elapsedTime;
		if (m_totalTime >= XM_2PI) {
			m_totalTime = 0.0;
		}
		XMFLOAT3 f(1, 1, 0);
		auto axis = XMLoadFloat3(&f);
		auto spanQt = XMQuaternionRotationAxis(axis, (float)(elapsedTime * 4));
		XMVECTOR quaternion = bsm::makeXM(m_param.quaternion);
		quaternion = XMQuaternionMultiply(quaternion, spanQt);
		m_param.quaternion = bsm::makeF4(quaternion);
		m_param.position.x = (float)sin(m_totalTime) * 2.0f;
	}

	//--------------------------------------------------------------------------------------
	// ステージのオブジェクト
	//--------------------------------------------------------------------------------------
	SkyStage::SkyStage(const TransParam& param) :
		MyObject(param)
	{
	}
	SkyStage::~SkyStage() {}


	void SkyStage::OnCreate(ID3D12GraphicsCommandList* pCommandList) {
		MyObject::OnCreate(pCommandList);
		m_mesh = BaseMesh::CreateCube(pCommandList, 1.0f);
		auto texFile = App::GetRelativeAssetsDir() + L"sky.jpg";
		m_texture = BaseTexture::CreateTextureFlomFile(pCommandList, texFile);
	}

	void SkyStage::OnUpdate(double elapsedTime) {}

}
// end namespace basecross
