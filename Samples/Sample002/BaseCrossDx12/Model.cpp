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
		m_baseMesh = BaseMesh::CreateSingleBoneModelMesh(
			pCommandList,
			App::GetRelativeAssetsDir(), L"SeaLife_Rigged\\Green_Sea_Turtle_Maya_2018.fbx"
		);

		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		//ベイシックコンスタントバッファ
		for (size_t i = 0; i < BaseDevice::FrameCount; i++) {
			m_constantBufferIndex = frameResources[i]->AddBaseConstantBufferSet<BasicConstant>(pBaseDevice->GetD3D12Device());
		}

		ComPtr<ID3D12PipelineState> defaultPipelineState
			= PipelineStatePool::GetPipelineState(L"BcPNTBone");
		CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
		//カリング
		rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		ZeroMemory(&psoDesc, sizeof(psoDesc));

		psoDesc.InputLayout = { VertexPositionNormalTextureSkinning::GetVertexElement(), VertexPositionNormalTextureSkinning::GetNumElements() };
		//			psoDesc.InputLayout = { VertexPositionNormalTexture::GetVertexElement(), VertexPositionNormalTexture::GetNumElements() };
		psoDesc.pRootSignature = RootSignaturePool::GetRootSignature(L"BaseCrossDefault").Get();
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(BcVSPNTBonePL::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			BcVSPNTBonePL::GetPtr()->GetShaderComPtr()->GetBufferSize()
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
			PipelineStatePool::AddPipelineState(L"BcPNTBone", defaultPipelineState);
		}
		//デフォルト影なし
		psoDesc.BlendState = BlendState::GetOpaqueBlend();
		//頂点シェーダー
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(BcVSPNTBonePL::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			BcVSPNTBonePL::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		//ピクセルシェーダー
		psoDesc.PS =
		{
			reinterpret_cast<UINT8*>(BcPSPNTPL::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			BcPSPNTPL::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};




	}

	void Model::OnUpdate(double elapsedTime) {
		m_totalTime += elapsedTime;
		if (m_totalTime >= 2.0) {
			m_totalTime = 0.0;
		}
		UpdateAnimation(m_totalTime);


	}

	bool Model::UpdateAnimation(double animeTime) {

		//アニメーション
		auto ptrBaseAssimp = m_baseMesh->GetBaseAssimp();
		m_BoneTransforms.clear();
		ptrBaseAssimp->GetBoneTransforms((float)animeTime, m_BoneTransforms);
		return true;
	}


	void Model::OnUpdateConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto& frameResources = scene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		auto& viewport = scene->GetViewport();
		std::shared_ptr<PerspecCamera> myCamera;
		std::shared_ptr<LightSet> myLightSet;

		myCamera =  std::dynamic_pointer_cast<PerspecCamera>(scene->GetCamera());
		myLightSet = scene->GetLightSet();

		auto scale = Vec3(0.1f);
		auto quaternion = Quat(Vec3(0, 1, 0), -XM_PI);
		auto position = Vec3(0.0f, 0.0f, 0.0f);

		//初期化
		m_constantBuffer = {};
		m_constantBuffer.activeFlg.y = 0;
		m_constantBuffer.activeFlg.x = 3;

		//ワールド行列の設定
		auto world = XMMatrixAffineTransformation(
			scale,
			Vec3(0),
			quaternion,
			position
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

	void Model::CommitConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto pCurrentFrameResource = scene->GetCurrentFrameResource();
		//シーン
		memcpy(pCurrentFrameResource->m_baseConstantBufferSetVec[m_constantBufferIndex].m_pBaseConstantBufferWO,
			&m_constantBuffer, sizeof(m_constantBuffer));
	}

	void Model::ScenePass(ID3D12GraphicsCommandList* pCommandList)
	{
		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pCurrentFrameResource = pBaseScene->GetCurrentFrameResource();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		auto& viewport = pBaseScene->GetViewport();
		auto& scissorRect = pBaseScene->GetScissorRect();
		auto depthDsvs = pBaseScene->GetDepthDsvs();
		auto depthGPUDsvs = pBaseScene->GetDepthSrvGpuHandles();

		auto CbvSrvUavDescriptorHeap = pBaseScene->GetCbvSrvUavDescriptorHeap();
		auto mesh = m_baseMesh;
		if (mesh) {
			ComPtr<ID3D12PipelineState> defaultPipelineState
				= PipelineStatePool::GetPipelineState(L"BcPNTBone", true);
			//null rv
			CD3DX12_GPU_DESCRIPTOR_HANDLE cbvSrvGpuNullHandle(CbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

			// set PipelineState and GetGpuSlotID(L"t0")
			pCommandList->SetPipelineState(defaultPipelineState.Get());
			pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t0"), cbvSrvGpuNullHandle);        // Set the shadow texture as an SRV.

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
			CD3DX12_GPU_DESCRIPTOR_HANDLE srvNullHandle(
				pBaseScene->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
			);
			pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t1"), srvNullHandle);
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
