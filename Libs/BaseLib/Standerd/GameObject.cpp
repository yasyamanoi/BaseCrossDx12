/*!
@file GameObject.cpp
@brief ゲームオブジェクトクラス
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	GameObject::GameObject(const shared_ptr<BaseStage>& stage) :
		m_stage(stage),
		m_UpdateActive(true),
		m_renderActive(true),
		m_AlphaActive(false),
		m_paramIndex(0) 
	{}

	GameObject::~GameObject() {}



	shared_ptr<BaseStage> GameObject::GetBaseStage(bool exceptionActive) const {
		auto shptr = m_stage.lock();
		if (shptr) {
			return shptr;
		}
		else {
			if (exceptionActive) {
				throw BaseException(
					L"所属ステージがnullです。自分自身がステージではありませんか？",
					L"if (!shptr)",
					L"GameObject::GetBaseStage()"
				);
			}
			else {
				return nullptr;
			}
		}
		// 所属ステージがnullだった
		// 自分自身がステージの可能性がある
		return nullptr;
	}


	void GameObject::SetConstants(BasicConstants& constants, const shared_ptr<Transform>& transform) {
		//初期化
		constants = {};
		constants.activeFlg.x = 3;
		constants.activeFlg.y = 1;
		auto stage = GetBaseStage();
		auto world = transform->GetWorldMatrix();
		auto view = stage->GetActiveCamera()->GetViewMatrix();
		auto proj = stage->GetActiveCamera()->GetProjectionMatrix();
		auto worldView = world * view;
		constants.worldViewProj = XMMatrixTranspose(XMMatrixMultiply(worldView, proj));
		constants.fogVector = g_XMZero;
		constants.fogColor = g_XMZero;
		//ライトの決定
		auto lightSet = stage->GetActiveLightSet();
		for (int i = 0; i < lightSet->GetNumLights(); i++) {
			constants.lightDirection[i] = lightSet->GetLight(i).m_directional;
			constants.lightDiffuseColor[i] = lightSet->GetLight(i).m_diffuseColor;
			constants.lightSpecularColor[i] = lightSet->GetLight(i).m_specularColor;
		}
		//ワールド行列
		constants.world = XMMatrixTranspose(world);

		XMMATRIX worldInverse = XMMatrixInverse(nullptr, world);
		constants.worldInverseTranspose[0] = worldInverse.r[0];
		constants.worldInverseTranspose[1] = worldInverse.r[1];
		constants.worldInverseTranspose[2] = worldInverse.r[2];

		XMMATRIX viewInverse = XMMatrixInverse(nullptr, view);
		constants.eyePosition = viewInverse.r[3];

		XMVECTOR diffuse = Col4(1.0f);
		XMVECTOR alphaVector = XMVectorReplicate(1.0);
		XMVECTOR emissiveColor = Col4(0.0f);
		XMVECTOR ambientLightColor = lightSet->GetAmbient();
		// emissive と ambientとライトをマージする
		constants.emissiveColor = (emissiveColor + ambientLightColor * diffuse) * alphaVector;
		constants.specularColorAndPower = Col4(0, 0, 0, 1);

		// xyz = diffuse * alpha, w = alpha.
		constants.diffuseColor = XMVectorSelect(alphaVector, diffuse * alphaVector, g_XMSelect1110);

	}

	void GameObject::WriteConstantBuffers(FrameResource* pFrameResource) {
		BasicConstants constants;
		SetConstants(constants, GetComponent<Transform>());
		memcpy(pFrameResource->m_frameParamVec[m_paramIndex].m_pConstantBuffer, &constants, sizeof(BasicConstants));
	}

	void GameObject::OnInitFrame(FrameResource* pFrameResource) {
		auto pDefaultDev = App::GetDefaultDevice();
		auto pDevice = App::GetID3D12Device();

		// コンスタントバッファのサイズは256バイト境界ごとに作成する
		UINT constsize = (sizeof(BasicConstants) + 255) & ~255;

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
		param.m_constBuffIndex = pDefaultDev->GetCbvSrvUavNextIndex();
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
		m_paramIndex = pFrameResource->m_frameParamVec.size();
		pFrameResource->m_frameParamVec.push_back(param);
	}

	void GameObject::OnRender() {
		ComponentRender();
	}
	
	void GameObject::TransformInit() {
		auto Transptr = GetComponent<Transform>();
		Transptr->SetToBefore();
	}

	void GameObject::OnPreInit() {
		//Transform必須
		AddComponent<Transform>();
	}


	void GameObject::ComponentUpdate() {
		auto transPtr = GetComponent<Transform>();
		auto veloPtr = GetComponent<Velocity>(false);
//		auto RightPtr = GetComponent<Rigidbody>(false);
		//マップを検証してUpdate
		list<type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			map<type_index, shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			if (it2 != m_compMap.end()) {
				//指定の型のコンポーネントが見つかった
				if (it2->second->IsUpdateActive()
					&& (it2->second != transPtr)
					&& (it2->second != veloPtr)
//					&& (it2->second != RightPtr)
					) {
					it2->second->OnUpdate();
				}
			}
			it++;
		}
//		if (RightPtr && RightPtr->IsUpdateActive()) {
//			RightPtr->OnUpdate();
//		}
		if (veloPtr && veloPtr->IsUpdateActive()) {
			veloPtr->OnUpdate();
		}
		//TransformのUpdate
		if (transPtr->IsUpdateActive()) {
			transPtr->OnUpdate();
		}
	}

/*
	void GameObject::ComponentRender() {
		//Transformがなければ例外
		auto transptr = GetComponent<Transform>();
//		auto RightPtr = GetComponent<Rigidbody>(false);
		//マップを検証してDraw
		list<type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			map<type_index, shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			//指定の型のコンポーネントが見つかった
			if (it2 != m_compMap.end()) {
				if (it2->second->IsRenderActive()
					&& (it2->second != transptr)
//					&& (it2->second != RightPtr)
					) {
					it2->second->OnRender();
				}
			}
			it++;
		}
//		if (RightPtr && RightPtr->IsDrawActive()) {
			//RigidbodyがあればDraw()
//			RightPtr->OnDraw();
//		}
		if (transptr->IsRenderActive()) {
			transptr->OnRender();
		}

	}

*/
	void GameObject::ComponentRender() {
		//Transformがなければ例外
		auto transptr = GetComponent<Transform>();
		//マップを検証してRender
		list<type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			map<type_index, shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			//指定の型のコンポーネントが見つかった
			if (it2 != m_compMap.end()) {
				if (it2->second->IsRenderActive()) {
					it2->second->OnRender();
				}
			}
			it++;
		}
	}


	void GameObject::ComponentDestroy() {
		auto transptr = GetComponent<Transform>();
		//マップを検証してUpdate
		list<type_index>::iterator it = m_compOrder.begin();
		while (it != m_compOrder.end()) {
			map<type_index, shared_ptr<Component> >::const_iterator it2;
			it2 = m_compMap.find(*it);
			if (it2 != m_compMap.end()) {
				//指定の型のコンポーネントが見つかった
				if (it2->second != transptr)
				{
					it2->second->OnDestroy();
				}
			}
			it++;
		}
		//TransformのOnDestroy
		transptr->OnDestroy();
	}




}
//end basecross
