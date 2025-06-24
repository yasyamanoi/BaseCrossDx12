//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

/*!
@file BaseScene.cpp
@brief シーン親クラス　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {


	
	using namespace SceneEnums;


	BaseScene* BaseScene::s_baseScene = nullptr;
	double BaseScene::s_elapsedTime(0.0);
	const float BaseScene::s_clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	BaseScene::BaseScene(UINT frameCount, PrimDevice* pPrimDevice) :
		m_frameCount(frameCount),
		m_fogDensity(0.04f),
		m_frameIndex(0),
		m_viewport(0.0f, 0.0f, 0.0f, 0.0f),
		m_scissorRect(0, 0, 0L, 0L),
		m_rtvDescriptorSize(0),
		m_cbvSrvDescriptorSize(0),
		m_pCurrentFrameResource(nullptr),
		m_pPrimDevice(pPrimDevice),
		m_srvSendIndex(m_srvStartIndex),
		m_cbvUavSendIndex(m_cbvUavStartIndex),
		m_samplerSendIndex(0)
	{
		s_baseScene = this;

		m_renderTargets.resize(m_frameCount);
		m_frameResources.resize(m_frameCount);

	}

	BaseScene::~BaseScene()
	{
		PxCloseExtensions();
		m_pScene->release();
		m_pDispatcher->release();
		m_pPhysics->release();
		m_pFoundation->release();
	}

	UINT BaseScene::GetSrvNextIndex() {
		if (m_srvSendIndex >= m_srvMax) {
			throw BaseException(
				L"これ以上シェーダリソースは増やせません。\n",
				L"Scene::GetSrvNextIndex()"
			);
		}
		return m_srvSendIndex++;
	}

	UINT BaseScene::GetCbvUavNextIndex() {
		if (m_cbvUavSendIndex >= m_cbvUavMax) {
			throw BaseException(
				L"これ以上コンスタントバッファとUAVは増やせません。\n",
				L"Scene::GetCbvUavNextIndex()"
			);
		}
		return m_cbvUavSendIndex++;
	}

	UINT BaseScene::GetSamplerNextIndex() {
		if (m_samplerSendIndex >= m_samplerMax) {
			throw BaseException(
				L"これ以上サンプラーは増やせません。",
				L"BaseScene::GetSamplerNextIndex()"
			);
		}
		return m_samplerSendIndex++;
	}

	UINT BaseScene::GetSamplerIndex(const std::wstring& key) {
		auto it = m_samplerMap.find(key);
		if (it != m_samplerMap.end()) {
			return it->second;
		}
		throw BaseException(
			L"指定のサンプラーは見つかりません。",
			key,
			L"BaseScene::GetSamplerNextIndex()"
		);
		return UINT_MAX;
	}

	void BaseScene::Initialize(ID3D12Device* pDevice, ID3D12CommandQueue* pDirectCommandQueue, ID3D12GraphicsCommandList* pCommandList, UINT frameIndex)
	{
		CreateDescriptorHeaps(pDevice);
		CreateRootSignatures(pDevice);
		CreatePostprocessPassResources(pDevice);
		CreateSamplers(pDevice);
		CreateFrameResources(pDevice, pDirectCommandQueue);
		CreateCommandLists(pDevice);
		CreateBasicResources(pDevice, pCommandList);
		CreateDefaultResources(pDevice, pCommandList);
		CreateAssetResources(pDevice, pCommandList);
		SetFrameIndex(frameIndex);
	}

	// Load the rendering pipeline dependencies.
	void BaseScene::CreateDescriptorHeaps(ID3D12Device* pDevice)
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = GetNumRtvDescriptors();
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
		NAME_D3D12_OBJECT(m_rtvHeap);

		// Describe and create a depth stencil view (DSV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = _countof(m_depthDsvs);
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));
		NAME_D3D12_OBJECT(m_dsvHeap);

		// Describe and create a shader resource view (SRV) and constant 
		// buffer view (CBV) descriptor heap.  
		// Heap layout: 
		// 1) null views, 
		// 2) depth buffer views,
		// Note that we use root constant buffer views, so we don't need descriptor 
		// heap space for each frames's constant buffer.
		D3D12_DESCRIPTOR_HEAP_DESC cbvSrvHeapDesc = {};
		cbvSrvHeapDesc.NumDescriptors = GetNumCbvSrvUavDescriptors(); //4096
		cbvSrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvSrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowIfFailed(pDevice->CreateDescriptorHeap(&cbvSrvHeapDesc, IID_PPV_ARGS(&m_cbvSrvHeap)));
		NAME_D3D12_OBJECT(m_cbvSrvHeap);

		// Describe and create a sampler descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
		samplerHeapDesc.NumDescriptors = GetNumSamplerDescriptors(); // 128 One clamp and one wrap sampler.
		samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowIfFailed(pDevice->CreateDescriptorHeap(&samplerHeapDesc, IID_PPV_ARGS(&m_samplerHeap)));
		NAME_D3D12_OBJECT(m_samplerHeap);

		//インクリメントサイズの取得
		m_rtvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		m_dsvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		m_cbvSrvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_samplerDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

	}

	void BaseScene::CreateRootSignatures(ID3D12Device* pDevice)
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

		// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

		if (FAILED(pDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		// Create a root signature for the basecross scene and shadow
		{

			CD3DX12_DESCRIPTOR_RANGE1 ranges[12]; // Perfomance TIP: Order from most frequent to least frequent.

			ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);	// t1.
			ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);	// t2.
			ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);	// t3.
			ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);	// t4.
			ranges[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);	//b0
			ranges[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);	//b1
			ranges[6].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);	//  t0.
			ranges[7].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);	// s0
			ranges[8].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);	// s1
			ranges[9].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 2, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);	// s2
			ranges[10].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 3, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE); // s3
			ranges[11].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 4, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE); // s4

			CD3DX12_ROOT_PARAMETER1 rootParameters[12];


			rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL); //t1
			rootParameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL); //t2
			rootParameters[2].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_PIXEL); //t3
			rootParameters[3].InitAsDescriptorTable(1, &ranges[3], D3D12_SHADER_VISIBILITY_PIXEL); //t4
			// 1 frequently changed constant buffer.
			rootParameters[4].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_ALL);
			rootParameters[5].InitAsDescriptorTable(1, &ranges[5], D3D12_SHADER_VISIBILITY_ALL); //b1
			rootParameters[6].InitAsDescriptorTable(1, &ranges[6], D3D12_SHADER_VISIBILITY_PIXEL); //t0
			rootParameters[7].InitAsDescriptorTable(1, &ranges[7], D3D12_SHADER_VISIBILITY_PIXEL); //s0
			rootParameters[8].InitAsDescriptorTable(1, &ranges[8], D3D12_SHADER_VISIBILITY_PIXEL); //s1
			rootParameters[9].InitAsDescriptorTable(1, &ranges[9], D3D12_SHADER_VISIBILITY_PIXEL); //s2
			rootParameters[10].InitAsDescriptorTable(1, &ranges[10], D3D12_SHADER_VISIBILITY_PIXEL); //s3
			rootParameters[11].InitAsDescriptorTable(1, &ranges[11], D3D12_SHADER_VISIBILITY_PIXEL); //s4

			SetGpuSlot(L"t0", 6);
			SetGpuSlot(L"t1", 0);
			SetGpuSlot(L"t2", 1);
			SetGpuSlot(L"t3", 2);
			SetGpuSlot(L"t4", 3);
			SetGpuSlot(L"s0", 7);
			SetGpuSlot(L"s1", 8);
			SetGpuSlot(L"s2", 9);
			SetGpuSlot(L"s3", 10);
			SetGpuSlot(L"s4", 11);
			SetGpuSlot(L"b0", 4);
			SetGpuSlot(L"b1", 5);

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
			//HULL_SHADERとDOMAIN_SHADERとGEOMETRY_SHADERをアクセスできない設定
			D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

			rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;

			ComPtr<ID3D12RootSignature> rootSignature;

			ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
			ThrowIfFailed(pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
			NAME_D3D12_OBJECT(rootSignature);
			//プールに登録
			RootSignaturePool::AddRootSignature(L"BaseCrossDefault", rootSignature);

		}

		// Create a root signature for the post-process pass.
		{
			CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
			ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
			ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

			CD3DX12_ROOT_PARAMETER1 rootParameters[3];
			rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL); // 1 depth texture - starting in register t0.
			rootParameters[1].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_ALL); // 1 frequently changed constant buffer.
			rootParameters[2].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL); // 1 static sampler.

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr,
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;

			ComPtr<ID3D12RootSignature> rootSignature;

			ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
			ThrowIfFailed(pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
			NAME_D3D12_OBJECT(rootSignature);

			//プールに登録
			RootSignaturePool::AddRootSignature(L"PostProcess", rootSignature);
		}

	}


	void BaseScene::CreateSamplers(ID3D12Device* pDevice)
	{
		//LinearWrap
		// 通常描画リニアラップサンプラー
		UINT index = GetSamplerNextIndex();
		D3D12_SAMPLER_DESC base_wrapSamplerDesc = {};
		base_wrapSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		base_wrapSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		base_wrapSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		base_wrapSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		base_wrapSamplerDesc.MinLOD = 0;
		base_wrapSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		base_wrapSamplerDesc.MipLODBias = 0.0f;
		base_wrapSamplerDesc.MaxAnisotropy = 1;
		base_wrapSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		base_wrapSamplerDesc.BorderColor[0]
			= base_wrapSamplerDesc.BorderColor[1]
			= base_wrapSamplerDesc.BorderColor[2]
			= base_wrapSamplerDesc.BorderColor[3]
			= 0;
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle1(
			m_samplerHeap->GetCPUDescriptorHandleForHeapStart(),
			index,
			m_samplerDescriptorSize
		);
		pDevice->CreateSampler(&base_wrapSamplerDesc, handle1);
		m_samplerMap[L"LinearWrap"] = index;


		//PointClamp
		index = GetSamplerNextIndex();
		D3D12_SAMPLER_DESC base_clampSamplerDesc = {};
		base_clampSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		base_clampSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		base_clampSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		base_clampSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		base_clampSamplerDesc.MipLODBias = 0.0f;
		base_clampSamplerDesc.MaxAnisotropy = 1;
		base_clampSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		base_clampSamplerDesc.BorderColor[0]
			= base_clampSamplerDesc.BorderColor[1]
			= base_clampSamplerDesc.BorderColor[2]
			= base_clampSamplerDesc.BorderColor[3]
			= 0;
		base_clampSamplerDesc.MinLOD = 0;
		base_clampSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle0(
			m_samplerHeap->GetCPUDescriptorHandleForHeapStart(),
			index,
			m_samplerDescriptorSize
		);
		pDevice->CreateSampler(&base_clampSamplerDesc, handle0);
		m_samplerMap[L"PointClamp"] = index;


		//LinearClamp
		// 通常描画リニアクランプサンプラー
		index = GetSamplerNextIndex();
		D3D12_SAMPLER_DESC linearClampSamplerDesc = {};
		linearClampSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		linearClampSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		linearClampSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		linearClampSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		linearClampSamplerDesc.MinLOD = 0;
		linearClampSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		linearClampSamplerDesc.MipLODBias = 0.0f;
		linearClampSamplerDesc.MaxAnisotropy = 1;
		linearClampSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		linearClampSamplerDesc.BorderColor[0]
			= linearClampSamplerDesc.BorderColor[1]
			= linearClampSamplerDesc.BorderColor[2]
			= linearClampSamplerDesc.BorderColor[3]
			= 0;
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle2(
			m_samplerHeap->GetCPUDescriptorHandleForHeapStart(),
			index,
			m_samplerDescriptorSize
		);
		pDevice->CreateSampler(&linearClampSamplerDesc, handle2);
		m_samplerMap[L"LinearClamp"] = index;
		//ComparisonLinear
		//影描画用コンパージョンリニア
		index = GetSamplerNextIndex();
		D3D12_SAMPLER_DESC ComparisonLinearSamplerDesc = {};
		ComparisonLinearSamplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		ComparisonLinearSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		ComparisonLinearSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		ComparisonLinearSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		ComparisonLinearSamplerDesc.MinLOD = 0;
		ComparisonLinearSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		ComparisonLinearSamplerDesc.MipLODBias = 0.0f;
		ComparisonLinearSamplerDesc.MaxAnisotropy = 0;
		ComparisonLinearSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		ComparisonLinearSamplerDesc.BorderColor[0]
			= ComparisonLinearSamplerDesc.BorderColor[1]
			= ComparisonLinearSamplerDesc.BorderColor[2]
			= ComparisonLinearSamplerDesc.BorderColor[3]
			= 1.0;
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle3(
			m_samplerHeap->GetCPUDescriptorHandleForHeapStart(),
			index,
			m_samplerDescriptorSize
		);
		pDevice->CreateSampler(&ComparisonLinearSamplerDesc, handle3);
		m_samplerMap[L"ComparisonLinear"] = index;
	}



	void BaseScene::CreateCommandLists(ID3D12Device* pDevice)
	{
		// Temporarily use a frame resource's command allocator to create command lists.
		ID3D12CommandAllocator* pCommandAllocator = m_frameResources[0]->m_commandAllocator.Get();

		for (UINT i = 0; i < CommandListCount; i++)
		{
			ThrowIfFailed(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator, nullptr, IID_PPV_ARGS(&m_commandLists[i])));
			ThrowIfFailed(m_commandLists[i]->Close());
			NAME_D3D12_OBJECT_INDEXED(m_commandLists, i);
		}

		ThrowIfFailed(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator, nullptr, IID_PPV_ARGS(&m_shadowCommandList)));
		ThrowIfFailed(m_shadowCommandList->Close());
		NAME_D3D12_OBJECT(m_shadowCommandList);

		ThrowIfFailed(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator, nullptr, IID_PPV_ARGS(&m_sceneCommandList)));
		ThrowIfFailed(m_sceneCommandList->Close());
		NAME_D3D12_OBJECT(m_sceneCommandList);

		// Batch up command lists for execution later.
		const UINT batchSize = 1 + 1 + CommandListCount; // m_sceneCommandList + m_shadowCommandList + CommandListCount

		m_batchSubmit[0] = m_commandLists[CommandListPre].Get();
		memcpy(m_batchSubmit + 1, m_shadowCommandList.GetAddressOf(), 1 * sizeof(ID3D12CommandList*));
		m_batchSubmit[1 + 1] = m_commandLists[CommandListMid].Get();
		memcpy(m_batchSubmit + 1 + 2, m_sceneCommandList.GetAddressOf(), 1 * sizeof(ID3D12CommandList*));
		m_batchSubmit[batchSize - 1] = m_commandLists[CommandListPost].Get();
	}


	void BaseScene::CreateBasicResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) {

		// Get a handle to the start of the descriptor heap.
		CD3DX12_CPU_DESCRIPTOR_HANDLE cbvSrvCpuHandle(m_cbvSrvHeap->GetCPUDescriptorHandleForHeapStart());
		CD3DX12_GPU_DESCRIPTOR_HANDLE cbvSrvGpuHandle(m_cbvSrvHeap->GetGPUDescriptorHandleForHeapStart());

		{
			// Describe and create 2 null SRVs. Null descriptors are needed in order 
			// to achieve the effect of an "unbound" resource.
			D3D12_SHADER_RESOURCE_VIEW_DESC nullSrvDesc = {};
			nullSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			nullSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			nullSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			nullSrvDesc.Texture2D.MipLevels = 1;
			nullSrvDesc.Texture2D.MostDetailedMip = 0;
			nullSrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

			for (UINT i = 0; i < NumNullSrvs; i++)
			{
				pDevice->CreateShaderResourceView(nullptr, &nullSrvDesc, cbvSrvCpuHandle);
				cbvSrvCpuHandle.Offset(m_cbvSrvDescriptorSize);
				cbvSrvGpuHandle.Offset(m_cbvSrvDescriptorSize);
			}
		}

		// Save the descriptor handles for the depth buffer views.
		for (UINT i = 0; i < _countof(m_depthSrvCpuHandles); i++)
		{
			m_depthSrvCpuHandles[i] = cbvSrvCpuHandle;
			m_depthSrvGpuHandles[i] = cbvSrvGpuHandle;
			cbvSrvCpuHandle.Offset(m_cbvSrvDescriptorSize);
			cbvSrvGpuHandle.Offset(m_cbvSrvDescriptorSize);
		}
	}


	void BaseScene::CreateDefaultResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) {
		m_pTgtCommandList = pCommandList;
		//PhysX関連
		m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_defaultAllocator, m_defaultErrorCallback);
		if (!m_pFoundation) {
			return;
		}
		m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, physx::PxTolerancesScale(), true);
		if (!m_pPhysics) {
			return;
		}
		m_pDispatcher = physx::PxDefaultCpuDispatcherCreate(8);
		physx::PxSceneDesc scene_desc(m_pPhysics->getTolerancesScale());
		scene_desc.gravity = physx::PxVec3(0, -9, 0);
		scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;
		scene_desc.cpuDispatcher = m_pDispatcher;
		m_pScene = m_pPhysics->createScene(scene_desc);

		auto mesh = BaseMesh::CreateCube(pCommandList, 1.0f);
		RegisterMesh(L"DEFAULT_CUBE", mesh);
		mesh = BaseMesh::CreateSphere(pCommandList, 1.0f, 18);
		RegisterMesh(L"DEFAULT_SPHERE", mesh);



	}


	void BaseScene::CreatePostprocessPassResources(ID3D12Device* pDevice)
	{
/*
		// Create the vertex buffer.
		// Define the screen space quad geometry.
		Vertex triangleVertices[] =
		{
			{ { -1.0f, -1.0f, 0.0f, 1.0f } },
			{ { -1.0f, 1.0f, 0.0f, 1.0f } },
			{ { 1.0f, -1.0f, 0.0f, 1.0f } },
			{ { 1.0f, 1.0f, 0.0f, 1.0f } },
		};

		const UINT vertexBufferSize = sizeof(triangleVertices);

		// Note: using upload heaps to transfer static data like vert buffers is not 
		// recommended. Every time the GPU needs it, the upload heap will be marshalled 
		// over. Please read up on Default Heap usage. An upload heap is used here for 
		// code simplicity and because there are very few verts to actually transfer.
		ThrowIfFailed(pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_vertexBuffers[VertexBuffer::ScreenQuad])));
		NAME_D3D12_OBJECT(m_vertexBuffers[VertexBuffer::ScreenQuad]);

		// Copy the triangle data to the vertex buffer.
		UINT8* pVertexDataBegin;
		const CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_vertexBuffers[VertexBuffer::ScreenQuad]->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
		memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
		m_vertexBuffers[VertexBuffer::ScreenQuad]->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		m_vertexBufferViews[VertexBuffer::ScreenQuad].BufferLocation = m_vertexBuffers[VertexBuffer::ScreenQuad]->GetGPUVirtualAddress();
		m_vertexBufferViews[VertexBuffer::ScreenQuad].StrideInBytes = sizeof(Vertex);
		m_vertexBufferViews[VertexBuffer::ScreenQuad].SizeInBytes = vertexBufferSize;
*/
	}

	void BaseScene::CreateFrameResources(ID3D12Device* pDevice, ID3D12CommandQueue* pCommandQueue)
	{
		for (UINT i = 0; i < m_frameCount; i++)
		{
			m_frameResources[i] = std::make_unique<FrameResource>(pDevice, pCommandQueue);
		}
	}

	// Load resources that are dependent on the size of the main window.
	void BaseScene::LoadSizeDependentResources(ID3D12Device* pDevice, ComPtr<ID3D12Resource>* ppRenderTargets, UINT width, UINT height)
	{

		m_viewport.Width = static_cast<float>(width);
		m_viewport.Height = static_cast<float>(height);

		m_scissorRect.left = 0;
		m_scissorRect.top = 0;
		m_scissorRect.right = static_cast<LONG>(width);
		m_scissorRect.bottom = static_cast<LONG>(height);

		// Create render target views (RTVs).
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvCpuHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
			for (UINT i = 0; i < m_frameCount; i++)
			{
				m_renderTargets[i] = ppRenderTargets[i];
				pDevice->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvCpuHandle);
				rtvCpuHandle.Offset(1, m_rtvDescriptorSize);
				NAME_D3D12_OBJECT_INDEXED(m_renderTargets, i);
			}
		}

		// Create the depth stencil views (DSVs).
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE dsvCpuHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
			const UINT dsvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

			// Shadow depth resource.
			m_depthDsvs[DepthGenPass::Shadow] = dsvCpuHandle;
			ThrowIfFailed(CreateDepthStencilTexture2D(pDevice, width, height, DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_D32_FLOAT, DXGI_FORMAT_R32_FLOAT, &m_depthTextures[DepthGenPass::Shadow], m_depthDsvs[DepthGenPass::Shadow], m_depthSrvCpuHandles[DepthGenPass::Shadow]));

			NAME_D3D12_OBJECT(m_depthTextures[DepthGenPass::Shadow]);

			dsvCpuHandle.Offset(dsvDescriptorSize);

			// Scene depth resource.
			m_depthDsvs[DepthGenPass::Scene] = dsvCpuHandle;
			ThrowIfFailed(CreateDepthStencilTexture2D(pDevice, width, height, DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_D32_FLOAT, DXGI_FORMAT_R32_FLOAT, &m_depthTextures[DepthGenPass::Scene], m_depthDsvs[DepthGenPass::Scene], m_depthSrvCpuHandles[DepthGenPass::Scene]));
			NAME_D3D12_OBJECT(m_depthTextures[DepthGenPass::Scene]);

		}
	}

	// Release sample's D3D objects
	void BaseScene::ReleaseSizeDependentResources()
	{
		m_depthTextures[DepthGenPass::Shadow].Reset();
		m_depthTextures[DepthGenPass::Scene].Reset();

		for (UINT i = 0; i < m_frameCount; i++)
		{
			m_renderTargets[i].Reset();
		}
	}

	// Release sample's D3D objects
	void BaseScene::ReleaseD3DObjects()
	{
		ResetComPtrArray(&m_renderTargets);
		RootSignaturePool::ReleaseRootSignature();
		PipelineStatePool::ReleasePipelineStates();
		m_rtvHeap.Reset();
		m_dsvHeap.Reset();
		m_cbvSrvHeap.Reset();
		m_samplerHeap.Reset();
		ResetUniquePtrArray(&m_frameResources);
		m_pCurrentFrameResource = nullptr;
	}

	// Caller is expected to enforce frame synchronization and that the GPU is done with frameIndex frame before being set as current frame again.
	void BaseScene::SetFrameIndex(UINT frameIndex)
	{
		m_frameIndex = frameIndex;
		m_pCurrentFrameResource = m_frameResources[m_frameIndex].get();
	}

	void BaseScene::KeyUp(UINT8 key)
	{
		switch (key)
		{
		case VK_LEFT:
			break;
		case VK_RIGHT:
			break;
		case VK_UP:
			break;
		case VK_DOWN:
			break;
		default:
			break;
		}
	}

	void BaseScene::KeyDown(UINT8 key)
	{
		switch (key)
		{
		case VK_LEFT:
			break;
		case VK_RIGHT:
			break;
		case VK_UP:
			break;
		case VK_DOWN:
			break;
		case VK_SPACE:
			break;
		default:
			break;
		}
	}


	void BaseScene::RegisterMesh(const std::wstring& key, const std::shared_ptr<BaseMesh>& mesh, bool keyCheck) {
		if (keyCheck) {
			auto it = m_meshMap.find(key);
			if (it != m_meshMap.end()) {
				throw BaseException(
					L"指定のキーのメッシュがすでに存在します",
					key,
					L"BaseScene::RegisterMesh()"
				);
			}
		}
		m_meshMap[key] = mesh;
	}

	std::shared_ptr<BaseMesh> BaseScene::GetMesh(const std::wstring& key) {
		auto it = m_meshMap.find(key);
		if (it != m_meshMap.end()) {
			return it->second;
		}
		else {
			throw BaseException(
				L"指定のキーのメッシュが見つかりません",
				key,
				L"BaseScene::GetMesh()"
			);
		}
		return nullptr;
	}

	void BaseScene::RegisterTexture(const std::wstring& key, const std::shared_ptr<BaseTexture>& texture, bool keyCheck) {
		if (keyCheck) {
			auto it = m_textureMap.find(key);
			if (it != m_textureMap.end()) {
				throw BaseException(
					L"指定のキーのテクスチャがすでに存在します",
					key,
					L"BaseScene::AddTexture()"
				);
			}
		}
		m_textureMap[key] = texture;
		//textureに名前を付ける
		auto texRes = texture->GetTexture();
		texRes->SetName(key.c_str());
	}

	std::shared_ptr<BaseTexture> BaseScene::GetTexture(const std::wstring& key) {
		auto it = m_textureMap.find(key);
		if (it != m_textureMap.end()) {
			return it->second;
		}
		else {
			throw BaseException(
				L"指定のキーのテクスチャが見つかりません",
				key,
				L"BaseScene::GetTexture()"
			);
		}
		return nullptr;
	}


	// Render the scene.
	void BaseScene::Render(ID3D12CommandQueue* pCommandQueue, bool setBackbufferReadyForPresent)
	{
		BeginFrame();
		// Shadow generation and scene render passes
		WorkerThread();
		MidFrame();
		EndFrame(setBackbufferReadyForPresent);
		pCommandQueue->ExecuteCommandLists(_countof(m_batchSubmit), m_batchSubmit);
	}

	// Apply a postprocess pass with a light scattering effect. 
	void BaseScene::PostprocessPass(ID3D12GraphicsCommandList* pCommandList)
	{
		// Set necessary state.
		//pCommandList->SetGraphicsRootSignature(m_rootSignatures[RootSignature::PostprocessPass].Get());
		//pCommandList->SetPipelineState(m_pipelineStates[RenderPass::Postprocess].Get());

		//DrawInScattering(pCommandList, GetCurrentBackBufferRtvCpuHandle());
	}

	void BaseScene::DrawInScattering(ID3D12GraphicsCommandList* pCommandList, const D3D12_CPU_DESCRIPTOR_HANDLE& renderTargetHandle)
	{
/*
		// Set necessary state.
		pCommandList->SetGraphicsRootDescriptorTable(0, m_depthSrvGpuHandles[DepthGenPass::Scene]); // Set scene depth as an SRV.
		pCommandList->SetGraphicsRootConstantBufferView(1, m_pCurrentFrameResource->GetConstantBufferGPUVirtualAddress(RenderPass::Postprocess)); // Set postprocess constant buffer.
		pCommandList->SetGraphicsRootDescriptorTable(2, m_samplerHeap->GetGPUDescriptorHandleForHeapStart()); // Set samplers.

		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		pCommandList->IASetVertexBuffers(0, 1, &m_vertexBufferViews[VertexBuffer::ScreenQuad]);
		pCommandList->RSSetViewports(1, &m_viewport);
		pCommandList->RSSetScissorRects(1, &m_scissorRect);
		pCommandList->OMSetRenderTargets(1, &renderTargetHandle, FALSE, nullptr); // No depth stencil needed for in-scattering.

		// Draw.
		pCommandList->DrawInstanced(4, 1, 0, 0);
*/
	}

	// Record the CommandListPre command list.
	void BaseScene::BeginFrame()
	{
		m_pCurrentFrameResource->InitFrame();

		ID3D12GraphicsCommandList* pCommandListPre = m_commandLists[CommandListPre].Get();

		// Reset the command list.
		ThrowIfFailed(pCommandListPre->Reset(m_pCurrentFrameResource->m_commandAllocator.Get(), nullptr));

		// Transition back-buffer to a writable state for rendering.
		pCommandListPre->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		// Clear the render target and depth stencils.
		pCommandListPre->ClearRenderTargetView(GetCurrentBackBufferRtvCpuHandle(), s_clearColor, 0, nullptr);
		pCommandListPre->ClearDepthStencilView(m_depthDsvs[DepthGenPass::Shadow], D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		pCommandListPre->ClearDepthStencilView(m_depthDsvs[DepthGenPass::Scene], D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		// Close the command list.
		ThrowIfFailed(pCommandListPre->Close());
	}

	// Record the CommandListMid command list.
	void BaseScene::MidFrame()
	{
		ID3D12GraphicsCommandList* pCommandListMid = m_commandLists[CommandListMid].Get();

		// Reset the command list.
		ThrowIfFailed(pCommandListMid->Reset(m_pCurrentFrameResource->m_commandAllocator.Get(), nullptr));

		// Transition our shadow map to a readable state for scene rendering.
		pCommandListMid->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_depthTextures[DepthGenPass::Shadow].Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		// Close the command list.
		ThrowIfFailed(pCommandListMid->Close());
	}

	// Record the CommandListPost command list.
	void BaseScene::EndFrame(bool setBackbufferReadyForPresent)
	{
		ID3D12GraphicsCommandList* pCommandListPost = m_commandLists[CommandListPost].Get();

		// Reset the command list.
		ThrowIfFailed(pCommandListPost->Reset(m_pCurrentFrameResource->m_commandAllocator.Get(), nullptr));

		// Set descriptor heaps.
		ID3D12DescriptorHeap* ppHeaps[] = { m_cbvSrvHeap.Get(), m_samplerHeap.Get() };
		pCommandListPost->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		// Transition scene depth to a readable state for post-processing.
		pCommandListPost->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_depthTextures[DepthGenPass::Scene].Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		PostprocessPass(pCommandListPost);

		// Transition depth buffers back to a writable state for the next frame
		// and conditionally indicate that the back buffer will now be used to present.
	   // Performance tip: Batch resource barriers into as few API calls as possible to minimize the amount of work the GPU does.
		D3D12_RESOURCE_BARRIER barriers[3];
		barriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(m_depthTextures[DepthGenPass::Shadow].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		barriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(m_depthTextures[DepthGenPass::Scene].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		barriers[2] = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		pCommandListPost->ResourceBarrier(setBackbufferReadyForPresent ? _countof(barriers) : _countof(barriers) - 1, barriers);

		// Close the command list.
		ThrowIfFailed(pCommandListPost->Close());
	}

	// Worker thread body. workerIndex is an integer from 0 to NumContexts 
	// describing the worker's thread index.
	void BaseScene::WorkerThread()
	{
		//
		// Shadow pass
		//
		{
			ID3D12GraphicsCommandList* pShadowCommandList = m_shadowCommandList.Get();
			// Reset the command list.(nullptr)
			ThrowIfFailed(pShadowCommandList->Reset(m_pCurrentFrameResource->m_contextCommandAllocator.Get(),nullptr));
			// Performance tip: Only set descriptor heaps if you need access to them.
			ShadowPass(pShadowCommandList);
			// Close the command list.
			ThrowIfFailed(pShadowCommandList->Close());
		}

		//
		// Scene pass
		//
		{
			ID3D12GraphicsCommandList* pSceneCommandList = m_sceneCommandList.Get();
			// Reset the command list.(nullptr)
			ThrowIfFailed(pSceneCommandList->Reset(m_pCurrentFrameResource->m_contextCommandAllocator.Get(), nullptr));
			// Set descriptor heaps.
			ID3D12DescriptorHeap* ppHeaps[] = { m_cbvSrvHeap.Get(), m_samplerHeap.Get() };
			pSceneCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
			ScenePass(pSceneCommandList);
			// Close the command list.
			ThrowIfFailed(pSceneCommandList->Close());

		}
	}
}
// end namespace basecross



