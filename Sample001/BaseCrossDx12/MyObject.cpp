/*!
@file MyObject.cpp
@brief ゲームオブジェクトクラス　実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	using namespace std;
	using namespace bsm;

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
			auto world = XMMatrixAffineTransformation(
				Vec3(m_param.scale),
				Vec3(m_param.rotOrigin),
				Quat(m_param.quaternion),
				Vec3(m_param.position)
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
				m_constantBuffer.fogColor = m_fogColor;
			}
			else
			{
				m_constantBuffer.fogVector = Vec4(g_XMZero);
				m_constantBuffer.fogColor = Vec4(g_XMZero);
			}
			//ライトの決定
			for (int i = 0; i < myLightSet->GetNumLights(); i++) {
				m_constantBuffer.lightDirection[i] = Vec4(myLightSet->GetLight(i).m_directional);
				m_constantBuffer.lightDiffuseColor[i] = myLightSet->GetLight(i).m_diffuseColor;
				m_constantBuffer.lightSpecularColor[i] = myLightSet->GetLight(i).m_specularColor;
			}
			//ワールド行列
			m_constantBuffer.world = Mat4x4(XMMatrixTranspose(world));

			XMMATRIX worldInverse = XMMatrixInverse(nullptr, world);
			m_constantBuffer.worldInverseTranspose[0] = Vec4(worldInverse.r[0]);
			m_constantBuffer.worldInverseTranspose[1] = Vec4(worldInverse.r[1]);
			m_constantBuffer.worldInverseTranspose[2] = Vec4(worldInverse.r[2]);

			XMMATRIX viewInverse = XMMatrixInverse(nullptr, view);
			m_constantBuffer.eyePosition = Vec4(viewInverse.r[3]);

			XMVECTOR diffuse = Col4(1.0f);
			XMVECTOR alphaVector = XMVectorReplicate(1.0f);
			XMVECTOR emissiveColor = Col4(0.0f);
			XMVECTOR ambientLightColor = Col4(myLightSet->GetAmbient());
			// emissive と ambientとライトをマージする
			m_constantBuffer.emissiveColor = Col4((emissiveColor + ambientLightColor * diffuse) * alphaVector);
			m_constantBuffer.specularColorAndPower = XMFLOAT4(0, 0, 0, 1);

			// xyz = diffuse * alpha, w = alpha.
			m_constantBuffer.diffuseColor = Col4(XMVectorSelect(alphaVector, diffuse * alphaVector, g_XMSelect1110));

			auto mainLight = myLightSet->GetMainBaseLight();
			XMFLOAT3 calcLightDir = (XMFLOAT3)(Vec3(mainLight.m_directional) * Vec3(-1.0f));

			XMFLOAT3 lightAt(myCamera->GetAt());
			XMFLOAT3 lightEye(calcLightDir);
			lightEye = (XMFLOAT3)(Vec3(lightEye) * Vec3(Shadowmap::GetLightHeight()));
			lightEye = (XMFLOAT3)(Vec3(lightAt) + Vec3(lightEye));

			XMFLOAT4 LightEye4 = Vec4((Vec3)lightEye, 1.0f);
			LightEye4.w = 1.0f;
			m_constantBuffer.lightPos = LightEye4;
			XMFLOAT4 eyePos4 = Vec4((Vec3)myCamera->GetEye(), 1.0f);
			eyePos4.w = 1.0f;
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
			m_constantBuffer.lightView = bsm::Mat4x4(XMMatrixTranspose(LightView));
			m_constantBuffer.lightProjection = bsm::Mat4x4(XMMatrixTranspose(LightProj));

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
			posSpan = (XMFLOAT3)(Vec3(posSpan) * Vec3(Shadowmap::GetPosAdjustment()));
			pos = (XMFLOAT3)(Vec3(pos) +  Vec3(posSpan));
			//ワールド行列の決定
			auto param_tmp = m_param;
			param_tmp.position = pos;

			auto world = XMMatrixAffineTransformation(
				Vec3(param_tmp.scale),
				Vec3(param_tmp.rotOrigin),
				Quat(param_tmp.quaternion),
				Vec3(param_tmp.position)
			);

			XMFLOAT3 lightDir = (XMFLOAT3)(Vec3(light.m_directional) * Vec3(-1.0f));

			auto camera = myCamera;
			XMFLOAT3 lightAt = camera->GetAt();
			XMFLOAT3 lightDirTmp = (XMFLOAT3)(Vec3(lightDir) * Vec3(Shadowmap::GetLightHeight()));
			XMFLOAT3 lightEye = (XMFLOAT3)(Vec3(lightAt) + Vec3(lightDirTmp));
			auto width = viewport.Width;
			auto height = viewport.Height;

			auto lightView
				= XMMatrixLookAtLH(Vec3(lightEye), Vec3(lightAt), Vec3(XMFLOAT3(0, 1.0f, 0)));

			auto lightProj =
				XMMatrixOrthographicLH(
					Shadowmap::GetViewWidth(),
					Shadowmap::GetViewHeight(),
					Shadowmap::GetLightNear(),
					Shadowmap::GetLightFar());
			m_shadowConstantBuffer.world = bsm::Mat4x4(XMMatrixTranspose(world));
			m_shadowConstantBuffer.view = bsm::Mat4x4(XMMatrixTranspose(lightView));
			m_shadowConstantBuffer.projection = bsm::Mat4x4(XMMatrixTranspose(lightProj));

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
		Quat spanQt(Vec3(1.0f, 1.0f, 0.0f), (float)(elapsedTime * 4));
		Quat quaternion(m_param.quaternion);
		quaternion *= spanQt;
		m_param.quaternion = quaternion;
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
