/*!
@file Model.cpp
@brief モデルクラス実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#include "stdafx.h"

namespace basecross {

	IMPLEMENT_DX12SHADER(BcVSPNTBonePL, App::GetShadersDir() + L"BcVSPNTBonePL.cso")
	IMPLEMENT_DX12SHADER(BcPSPNTPL, App::GetShadersDir() + L"BcPSPNTPL.cso")


	void Model::OnCreate() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		m_mesh = BaseMesh::CreateSingleBoneModelMesh(
			pCommandList,
			App::GetRelativeAssetsDir(), L"SeaLife_Rigged\\Green_Sea_Turtle_Maya_2018.fbx"
		);
		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		//ベイシックコンスタントバッファ作成
		for (size_t i = 0; i < BaseDevice::FrameCount; i++) {
			m_constantBufferIndex = frameResources[i]->AddBaseConstantBufferSet<BasicConstant>(pBaseDevice->GetD3D12Device());
		}
		//CPU側コンスタントバッファの作成
		CreateConstantBuffer();
		ComPtr<ID3D12PipelineState> defaultPipelineState
			= PipelineStatePool::GetPipelineState(L"BcPNTBone");
		if (!defaultPipelineState) {
			//defaultPipelineStateが見つからなければ作成
			//ラスタライザステート
			CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
			//カリング（裏側は描画しない）
			rasterizerStateDesc.CullMode = D3D12_CULL_MODE_BACK;
			//パイプラインステートの定義
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			ZeroMemory(&psoDesc, sizeof(psoDesc));
			//VertexPositionNormalTextureSkinningで作成
			psoDesc.InputLayout = { VertexPositionNormalTextureSkinning::GetVertexElement(), VertexPositionNormalTextureSkinning::GetNumElements() };
			//ルート認証はデフォルト
			psoDesc.pRootSignature = RootSignaturePool::GetRootSignature(L"BaseCrossDefault").Get();
			//頂点シェーダ
			psoDesc.VS =
			{
				reinterpret_cast<UINT8*>(BcVSPNTBonePL::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				BcVSPNTBonePL::GetPtr()->GetShaderComPtr()->GetBufferSize()
			};
			//ピクセルシェーダ
			psoDesc.PS =
			{
				reinterpret_cast<UINT8*>(BcPSPNTPL::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				BcPSPNTPL::GetPtr()->GetShaderComPtr()->GetBufferSize()
			};
			//ラスタライズステート
			psoDesc.RasterizerState = rasterizerStateDesc;
			//ブレンドステートは塗りつぶし
			psoDesc.BlendState = BlendState::GetOpaqueBlend();
			//デプスステンシルはデフォルト
			psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc.Count = 1;
			//デフォルト影無しのボーン描画
			if (!defaultPipelineState) {
				ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&defaultPipelineState)));
				NAME_D3D12_OBJECT(defaultPipelineState);
				PipelineStatePool::AddPipelineState(L"BcPNTBone", defaultPipelineState);
			}
		}
	}


	void Model::CreateConstantBuffer() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto& frameResources = scene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		auto& viewport = scene->GetViewport();
		std::shared_ptr<PerspecCamera> myCamera;
		std::shared_ptr<LightSet> myLightSet;
		//カメラとライト
		myCamera = std::dynamic_pointer_cast<PerspecCamera>(scene->GetCamera());
		myLightSet = scene->GetLightSet();
		//パラメータの初期化
		m_param.scale = Vec3(0.1f);
		m_param.quaternion = Quat(Vec3(0, 1, 0), -XM_PI);
		m_param.position = Vec3(0.0f, 0.0f, 0.0f);

		//初期化
		m_constantBuffer = {};
		m_constantBuffer.activeFlg.y = 0;
		//ライトは３つ
		m_constantBuffer.activeFlg.x = 3;

		//ワールド行列の設定
		auto world = XMMatrixAffineTransformation(
			m_param.scale,
			Vec3(0),
			m_param.quaternion,
			m_param.position
		);

		auto view = (XMMATRIX)((Mat4x4)myCamera->GetViewMatrix());
		auto proj = (XMMATRIX)((Mat4x4)myCamera->GetProjMatrix());
		auto worldView = world * view;
		m_constantBuffer.worldViewProj = Mat4x4(XMMatrixTranspose(XMMatrixMultiply(worldView, proj)));

		m_constantBuffer.fogVector = Vec4(g_XMZero);
		m_constantBuffer.fogColor = Vec4(g_XMZero);
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
		LightProj = XMMatrixOrthographicLH(1024, 1024,
			0.1f, 100);
		m_constantBuffer.lightView = Mat4x4(XMMatrixTranspose(LightView));
		m_constantBuffer.lightProjection = Mat4x4(XMMatrixTranspose(LightProj));

		//ボーン
		size_t BoneSz = m_BoneTransforms.size();
		if (BoneSz > 0) {
			UINT cb_count = 0;
			for (size_t b = 0; b < BoneSz; b++) {
				bsm::Mat4x4 mat = m_BoneTransforms[b];
				//	mat.transpose();
				m_constantBuffer.Bones[cb_count] = ((XMMATRIX)mat).r[0];
				m_constantBuffer.Bones[cb_count + 1] = ((XMMATRIX)mat).r[1];
				m_constantBuffer.Bones[cb_count + 2] = ((XMMATRIX)mat).r[2];
				cb_count += 3;
			}
		}
	}

	void Model::OnUpdate(double elapsedTime) {
		m_totalTime += elapsedTime;
		if (m_totalTime >= 2.0) {
			//2秒間のアニメーションなので2秒を超えたら0にする
			m_totalTime = 0.0;
		}
		UpdateAnimation(m_totalTime);
	}

	void Model::UpdateAnimation(double animeTime) {
		//BaseAssimpの取り出し
		auto ptrBaseAssimp = m_mesh->GetBaseAssimp();
		m_BoneTransforms.clear();
		//m_BoneTransformsにアニメーション情報の行列を代入
		ptrBaseAssimp->GetBoneTransforms((float)animeTime, m_BoneTransforms);
	}


	void Model::OnUpdateConstantBuffers() {
		//ボーンの更新
		size_t BoneSz = m_BoneTransforms.size();
		if (BoneSz > 0) {
			UINT cb_count = 0;
			for (size_t b = 0; b < BoneSz; b++) {
				bsm::Mat4x4 mat = m_BoneTransforms[b];
				m_constantBuffer.Bones[cb_count] = ((XMMATRIX)mat).r[0];
				m_constantBuffer.Bones[cb_count + 1] = ((XMMATRIX)mat).r[1];
				m_constantBuffer.Bones[cb_count + 2] = ((XMMATRIX)mat).r[2];
				cb_count += 3;
			}
		}
	}

	void Model::OnCommitConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto pCurrentFrameResource = scene->GetCurrentFrameResource();
		//コンスタントバッファのコピー
		memcpy(pCurrentFrameResource->m_baseConstantBufferSetVec[m_constantBufferIndex].m_pBaseConstantBufferWO,
			&m_constantBuffer, sizeof(m_constantBuffer));
	}

	void Model::ScenePass(ID3D12GraphicsCommandList* pCommandList)
	{
		auto pBaseScene = BaseScene::Get();
		auto pCurrentFrameResource = pBaseScene->GetCurrentFrameResource();
		auto CbvSrvUavDescriptorHeap = pBaseScene->GetCbvSrvUavDescriptorHeap();
		if (m_mesh) {
			ComPtr<ID3D12PipelineState> defaultPipelineState
				= PipelineStatePool::GetPipelineState(L"BcPNTBone", true);
			//nullのハンドルを取得
			CD3DX12_GPU_DESCRIPTOR_HANDLE cbvSrvGpuNullHandle(CbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
			//L"t0"（シャドウテクスチャ）にnullを設定
			pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t0"), cbvSrvGpuNullHandle);
			//パイプラインステートの設定
			pCommandList->SetPipelineState(defaultPipelineState.Get());
			//サンプラー1
			UINT index = pBaseScene->GetSamplerIndex(L"LinearClamp");
			if (index == UINT_MAX) {
				throw BaseException(
					L"LinearClampサンプラーが特定できません。",
					L"Model::ScenePass()"
				);
			}
			//サンプラー1のハンドルを作成
			CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle1(
				pBaseScene->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				index,
				pBaseScene->GetSamplerDescriptorHandleIncrementSize()
			);
			//L"s0"にサンプラー1のハンドルを設定
			pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"s0"), samplerHandle1);
			//サンプラー2
			index = pBaseScene->GetSamplerIndex(L"ComparisonLinear");
			if (index == UINT_MAX) {
				throw BaseException(
					L"ComparisonLinearサンプラーが特定できません。",
					L"SModel::ScenePass()"
				);
			}
			//サンプラー2のハンドルを作成
			CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle2(
				pBaseScene->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				index,
				pBaseScene->GetSamplerDescriptorHandleIncrementSize()
			);
			//L"s1"にサンプラー2のハンドルを設定
			pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"s1"), samplerHandle2);
			//シェーダリソース（テクスチャ）のハンドルの設定(null)
			CD3DX12_GPU_DESCRIPTOR_HANDLE srvNullHandle(
				pBaseScene->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
			);
			//テクスチャは使わないのでL"t1"にnullを設定
			pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t1"), srvNullHandle);
			//コンスタントバッファビュー
			//L"b0"に現在のフレームのコンスタントバッファを設定
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
