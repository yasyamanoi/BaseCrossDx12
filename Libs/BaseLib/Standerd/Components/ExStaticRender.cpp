/*!
@file ExStaticRender.cpp
@brief Extraスタティック描画コンポーネント群
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	IMPLEMENT_DX12SHADER(ExVSPNTTn, App::GetShadersDir() + L"ExVSPNTTn.cso")
	IMPLEMENT_DX12SHADER(ExPSPNTTn, App::GetShadersDir() + L"ExPSPNTTn.cso")

	//--------------------------------------------------------------------------------------
	///	ExStaticRender描画コンポーネント
	//--------------------------------------------------------------------------------------

	ExStaticRender::ExStaticRender(const shared_ptr<GameObject>& gameObjectPtr) :
		Component(gameObjectPtr)
	{
		SetRenderActive(true);
	}

	void ExStaticRender::OnInitFrame(BaseFrame* pBaseFrame) {
		auto pDefaultDev = App::GetBaseDevice();
		auto pDevice = App::GetID3D12Device();

		// コンスタントバッファのサイズは256バイト境界ごとに作成する
		UINT constsize = (sizeof(ExConstant) + 255) & ~255;

		FrameParam param;
		// アップロードヒープ
		ThrowIfFailed(pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(constsize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&param.m_cbvUploadHeap)));
		//マップ
		CD3DX12_RANGE readRange(0, 0);
		ThrowIfFailed(param.m_cbvUploadHeap->Map(0, &readRange, reinterpret_cast<void**>(&param.m_pConstantBuffer)));
		//ハンドルの作成
		param.m_constBuffIndex = pDefaultDev->GetCbvUavNextIndex();
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle(
			pDefaultDev->GetCbvSrvUavDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
			param.m_constBuffIndex,
			pDefaultDev->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		//コンスタントバッファビューの作成
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = param.m_cbvUploadHeap->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = constsize;
		pDevice->CreateConstantBufferView(&cbvDesc, handle);
		m_frameParamIndex = pBaseFrame->m_frameParamVec.size();
		pBaseFrame->m_frameParamVec.push_back(param);


	}

	void ExStaticRender::SetConstant(ExConstant& constants, const shared_ptr<Transform>& transform) {
		//初期化
		constants = {};
		Mat4x4 world = transform->GetWorldMatrix();
		//転置する
		world.transpose();

		//ライトの決定
		auto lightSet = GetStage()->GetActiveLightSet();
		auto lightCamera = ObjectFactory::Create<BaseCamera>();
		auto lightCamera2 = ObjectFactory::Create<BaseCamera>();
		auto tgt= lightSet->GetMainIndex();
		for (int i = 0; i < 3; i++) {
			ExLightState lightState = {};
			lightState.direction.x = lightSet->GetLight(i).m_directional.x;
			lightState.direction.y = lightSet->GetLight(i).m_directional.y;
			lightState.direction.z = lightSet->GetLight(i).m_directional.z;
			Vec3 pos = transform->GetWorldPosition();
			pos += lightSet->GetLight(i).m_directional * 100.0f;
			lightState.position.x = pos.x;
			lightState.position.y = pos.y;
			lightState.position.z = pos.z;
			lightState.color = Col4(1.0f);
			lightCamera->SetEye(pos);
			lightCamera->SetAt(transform->GetWorldPosition());
			lightCamera->SetUp(Vec3(0, 1, 0));
			lightState.view = lightCamera->GetViewMatrix();
			lightState.projection = lightCamera->GetProjMatrix();

			if (i == tgt) {
				lightCamera2 = lightCamera;
			}

			constants.lights[i] = lightState;
		}


		auto cameraPtr = GetGameObject()->GetActiveCamera();
		//ビューと射影行列を得る
		Mat4x4 viewMat = cameraPtr->GetViewMatrix();
//		viewMat = lightCamera2->GetViewMatrix() * viewMat;
		//転置する
		viewMat.transpose();
		Mat4x4 projMat = cameraPtr->GetProjMatrix();
//		projMat = lightCamera2->GetProjMatrix() * projMat;
		//転置する
		projMat.transpose();



		constants.model = world;
		constants.view = viewMat;
		constants.projection = projMat;
		constants.ambientColor = Col4(1.0f);
		constants.sampleShadowMap = FALSE;




/*

		//すべて0に初期化
		constant = {};
		Mat4x4 world = transform->GetWorldMatrix();
		//転置する
		world.transpose();
		auto cameraPtr = GetGameObject()->GetActiveCamera();
		//ビューと射影行列を得る
		Mat4x4 viewMat = cameraPtr->GetViewMatrix();
		//転置する
		viewMat.transpose();
		Mat4x4 projMat = cameraPtr->GetProjMatrix();
		//転置する
		projMat.transpose();
		constant.world = world;
		constant.view = viewMat;
		constant.projection = projMat;
		//エミッシブ
		constant.emissive = GetEmissive();
		//デフィーズ
		constant.diffuse = GetDiffuse();
		//スペキュラー
		constant.specular = GetSpecular();
		//ライティング
		auto activeLights = GetGameObject()->GetActiveLightSet();
		auto index = activeLights->GetMainIndex();
		auto light = activeLights->GetLight(index);
		constant.lightDir = light.m_directional;
		constant.lightDir.w = 1.0f;
		constant.eyePos = cameraPtr->GetEye();
		constant.eyePos.w = 1.0f;
		//テクスチャがあった場合
		if (GetTexture()) {
			constant.activeFlg.x = 1;
		}


*/

/*

		SceneConstantBuffer sceneConsts = {};
		SceneConstantBuffer shadowConsts = {};

		// Scale down the world a bit.
		::XMStoreFloat4x4(&sceneConsts.model, XMMatrixScaling(0.1f, 0.1f, 0.1f));
		::XMStoreFloat4x4(&shadowConsts.model, XMMatrixScaling(0.1f, 0.1f, 0.1f));

		// The scene pass is drawn from the camera.
		pSceneCamera->Get3DViewProjMatrices(&sceneConsts.view, &sceneConsts.projection, 90.0f, pViewport->Width, pViewport->Height);

		// The light pass is drawn from the first light.
		lightCams[0].Get3DViewProjMatrices(&shadowConsts.view, &shadowConsts.projection, 90.0f, pViewport->Width, pViewport->Height);

		for (int i = 0; i < DefaultDevice::m_NumLights; i++)
		{
			memcpy(&sceneConsts.lights[i], &lights[i], sizeof(LightState));
			memcpy(&shadowConsts.lights[i], &lights[i], sizeof(LightState));
		}

		// The shadow pass won't sample the shadow map, but rather write to it.
		shadowConsts.sampleShadowMap = FALSE;

		// The scene pass samples the shadow map.
		sceneConsts.sampleShadowMap = TRUE;

		shadowConsts.ambientColor = sceneConsts.ambientColor = { 0.1f, 0.2f, 0.3f, 1.0f };

		memcpy(mp_sceneConstantBufferWO, &sceneConsts, sizeof(SceneConstantBuffer));
		memcpy(mp_shadowConstantBufferWO, &shadowConsts, sizeof(SceneConstantBuffer));



*/



	}


	void ExStaticRender::WriteConstantBuffers(BaseFrame* pBaseFrame) {
		ExConstant constants;
		SetConstant(constants, GetGameObject()->GetComponent<Transform>());
		memcpy(pBaseFrame->m_frameParamVec[m_frameParamIndex].m_pConstantBuffer, &constants, sizeof(ExConstant));
	}



	void ExStaticRender::CreatePipelineStates() {
		auto pDevice = App::GetBaseDevice();

		CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		depthStencilDesc.StencilEnable = FALSE;

		// Describe and create the PSO for rendering the scene.
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { VertexPositionNormalTextureTangent::GetVertexElement(), VertexPositionNormalTextureTangent::GetNumElements() };;
		psoDesc.pRootSignature = pDevice->GetRootSignature().Get();
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(ExVSPNTTn::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			ExVSPNTTn::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<UINT8*>(ExPSPNTTn::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			ExPSPNTTn::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState = depthStencilDesc;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleDesc.Count = 1;
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_defaultPipelineState)));
	}

	void ExStaticRender::OnCreate() {
		CreatePipelineStates();

	}





	void ExStaticRender::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();
		auto pCommandList = pDevice->GetComandList().Get();
		pCommandList->SetPipelineState(m_defaultPipelineState.Get());
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//Srv
		if (m_baseTexture && m_baseTexture2) {


			CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(
				pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				m_baseTexture->GetSrvIndex(),
				pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
			);
			pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"t1"), srvHandle);

			CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle2(
				pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				m_baseTexture2->GetSrvIndex(),
				pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
			);
			pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"t2"), srvHandle2);


			//Sampler
			UINT index = pDevice->GetSamplerIndex(L"LinearWrap");
			if (index == UINT_MAX) {
				throw BaseException(
					L"LinearWrapサンプラーが特定できません。",
					L"ExStaticRender::PopulateCommandList()"
				);
			}
			CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle(
				pDevice->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				index,
				pDevice->GetSamplerDescriptorHandleIncrementSize()
			);
			pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"s0"), samplerHandle);
		}
		//Cbv
		auto paramIndex = GetFrameParamIndex();
		CD3DX12_GPU_DESCRIPTOR_HANDLE cvbvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_frameParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), cvbvHandle);

		if (!m_baseMesh) {
			throw BaseException(
				L"メッシュが特定できません。",
				L"ExStaticRender::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);

	}



}
// end namespace basecross
