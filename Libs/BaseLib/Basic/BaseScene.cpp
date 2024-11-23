/*!
@file BaseScene.cpp
@brief シーン親クラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/

#include "stdafx.h"
namespace basecross {

//	using namespace SceneEnums;

	BaseScene* BaseScene::pBaseScene = nullptr;
	const float BaseScene::s_clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	BaseScene::BaseScene(UINT frameCount, PrimDevice * pPrimDevice) :
		m_frameCount(frameCount),
		m_fogDensity(0.04f),
		m_frameIndex(0),
		m_viewport(0.0f, 0.0f, 0.0f, 0.0f),
		m_scissorRect(0, 0, 0L, 0L),
		m_rtvDescriptorSize(0),
		m_cbvSrvDescriptorSize(0),
		m_pCurrentBaseFrame(nullptr),
		m_srvSendIndex(m_srvStartIndex),
		m_cbvUavSendIndex(m_cbvUavStartIndex),
		m_samplerSendIndex(0)
	{
		pBaseScene = this;
		m_renderTargets.resize(m_frameCount);
		m_frameResources.resize(m_frameCount);
	}

	BaseScene::~BaseScene()
	{
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

	void BaseScene::CreateDescriptorHeaps(ID3D12Device* pDevice) {
		// RTVディスクプリタヒープ作成（フレームの数作成）
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = GetNumRtvDescriptors();
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
		NAME_D3D12_OBJECT(m_rtvHeap);

		// デプスステンシルビュー用のディスクプリタヒープ作成（2個）
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = _countof(m_depthDsvs); //2個
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));
		NAME_D3D12_OBJECT(m_dsvHeap);

		// CBVとSRVとUAV用のデスクプリタヒープ
		D3D12_DESCRIPTOR_HEAP_DESC cbvSrvUavHeapDesc = {};
		cbvSrvUavHeapDesc.NumDescriptors = m_cbvSrvUavMax; //全部で4096個
		cbvSrvUavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvSrvUavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowIfFailed(pDevice->CreateDescriptorHeap(&cbvSrvUavHeapDesc, IID_PPV_ARGS(&m_cbvSrvUavHeap)));
		NAME_D3D12_OBJECT(m_cbvSrvUavHeap);

		//サンプラー用.
		D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
		samplerHeapDesc.NumDescriptors = m_samplerMax; // 128 One clamp and one wrap sampler.
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

	void BaseScene::CreateCommandLists(ID3D12Device* pDevice) {
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
		const UINT batchSize = 1 + 1 + CommandListCount; //Scene + Shadow + CommandListCount == 5
		m_batchSubmit[0] = m_commandLists[CommandListPre].Get();
		m_batchSubmit[1] = m_shadowCommandList.Get();
		m_batchSubmit[2] = m_commandLists[CommandListMid].Get();
		m_batchSubmit[3] = m_sceneCommandList.Get();
		m_batchSubmit[4] = m_commandLists[CommandListPost].Get();
	}

	void BaseScene::CreateRootSignatures(ID3D12Device* pDevice) {
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

		// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

		if (FAILED(pDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}


		// Create a root signature for the shadow pass.
		{
			CD3DX12_ROOT_PARAMETER1 rootParameters[1]; // Performance tip: Order root parameters from most frequently accessed to least frequently accessed.
			rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX); // 1 frequently changed constant buffer.

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr,
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | // Performance tip: Limit root signature access when possible.
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
			ThrowIfFailed(pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignatures[RootSignature::ShadowPass])));
			NAME_D3D12_OBJECT(m_rootSignatures[RootSignature::ShadowPass]);
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
			ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
			ThrowIfFailed(pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignatures[RootSignature::PostprocessPass])));
			NAME_D3D12_OBJECT(m_rootSignatures[RootSignature::PostprocessPass]);
		}



		// Create a root signature for the scene pass.
		{
			CD3DX12_DESCRIPTOR_RANGE1 ranges[5];

			ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);	// 1 frequently changed diffuse texture using register t0.
			ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);	// 1 frequently changed normal texture using register t1.
			ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);	// 1 infrequently changed shadow texture t2.
			ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);	// 1 static samplers.s0
			ranges[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 1);	// 1 static samplers.s1

			CD3DX12_ROOT_PARAMETER1 rootParameters[6];
			rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL); // diffuse t0.
			rootParameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL); // diffuse t1.
			rootParameters[2].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_ALL); // 1 frequently changed constant buffer.
			rootParameters[3].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_PIXEL); // shadow texture t2.
			rootParameters[4].InitAsDescriptorTable(1, &ranges[3], D3D12_SHADER_VISIBILITY_PIXEL); // 1 static sampler s0.
			rootParameters[5].InitAsDescriptorTable(1, &ranges[4], D3D12_SHADER_VISIBILITY_PIXEL); // 1 static sampler s1.

			SetGpuSlot(L"3d_t0", 0);
			SetGpuSlot(L"3d_t1", 1);
			SetGpuSlot(L"3d_b0", 2);
			SetGpuSlot(L"3d_t2", 3);
			SetGpuSlot(L"3d_s0", 4);
			SetGpuSlot(L"3d_s1", 5);


			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr,
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | // Performance tip: Limit root signature access when possible.
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
			ThrowIfFailed(pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignatures[RootSignature::ScenePass])));
			NAME_D3D12_OBJECT(m_rootSignatures[RootSignature::ScenePass]);
		}


		// Create a root signature for the sprite pass.
		{

			CD3DX12_DESCRIPTOR_RANGE1 ranges[2];

			ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);	// 1 frequently changed diffuse texture using register t0.
			ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);	// 1 static samplers.s0

			CD3DX12_ROOT_PARAMETER1 rootParameters[3];
			rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL); // diffuse t0.
			rootParameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL); // 1 static sampler s0.
			rootParameters[2].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_ALL); // 1 frequently changed constant buffer.

			SetGpuSlot(L"2d_t0", 0);
			SetGpuSlot(L"2d_s0", 1);
			SetGpuSlot(L"2d_b0", 2);


			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr,
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | // Performance tip: Limit root signature access when possible.
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
			ThrowIfFailed(pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignatures[RootSignature::SpritePass])));
			NAME_D3D12_OBJECT(m_rootSignatures[RootSignature::SpritePass]);
		}


	}

	void BaseScene::CreatePostprocessPassResources(ID3D12Device* pDevice) {
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
			D3D12_RESOURCE_STATE_COMMON,
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
	}

	void BaseScene::CreateSamplers(ID3D12Device* pDevice) {
		// Describe and create the point clamping sampler, which is 
		// used for the shadow map.
		UINT index = GetSamplerNextIndex();
		D3D12_SAMPLER_DESC clampSamplerDesc = {};
		clampSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		clampSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		clampSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		clampSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		clampSamplerDesc.MipLODBias = 0.0f;
		clampSamplerDesc.MaxAnisotropy = 1;
		clampSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		clampSamplerDesc.BorderColor[0] = clampSamplerDesc.BorderColor[1] = clampSamplerDesc.BorderColor[2] = clampSamplerDesc.BorderColor[3] = 0;
		clampSamplerDesc.MinLOD = 0;
		clampSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle1(
			m_samplerHeap->GetCPUDescriptorHandleForHeapStart(),
			index,
			m_samplerDescriptorSize
		);
		pDevice->CreateSampler(&clampSamplerDesc, handle1);
		m_samplerMap[L"Clamp"] = index;

		index = GetSamplerNextIndex();
		// Describe and create the wrapping sampler, which is used for 
		// sampling diffuse/normal maps.
		D3D12_SAMPLER_DESC wrapSamplerDesc = {};
		wrapSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		wrapSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		wrapSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		wrapSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		wrapSamplerDesc.MinLOD = 0;
		wrapSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		wrapSamplerDesc.MipLODBias = 0.0f;
		wrapSamplerDesc.MaxAnisotropy = 1;
		wrapSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		wrapSamplerDesc.BorderColor[0] = wrapSamplerDesc.BorderColor[1] = wrapSamplerDesc.BorderColor[2] = wrapSamplerDesc.BorderColor[3] = 0;
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle2(
			m_samplerHeap->GetCPUDescriptorHandleForHeapStart(),
			index,
			m_samplerDescriptorSize
		);
		pDevice->CreateSampler(&wrapSamplerDesc, handle1);
		m_samplerMap[L"Wrap"] = index;
	}



	void BaseScene::CreateBaseFrames(ID3D12Device* pDevice, ID3D12CommandQueue* pCommandQueue) {
		for (UINT i = 0; i < m_frameCount; i++)
		{
			m_frameResources[i] = std::make_unique<BaseFrame>(pDevice, pCommandQueue);
		}
	}

	void BaseScene::CreateAssetResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) {
		// Create shader resources.
		{
			// Get a handle to the start of the descriptor heap.
			CD3DX12_CPU_DESCRIPTOR_HANDLE cbvSrvCpuHandle(m_cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart());
			CD3DX12_GPU_DESCRIPTOR_HANDLE cbvSrvGpuHandle(m_cbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart());

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
		CreateSceneResources(pDevice, pCommandList);
	}




	void BaseScene::Initialize(ID3D12Device * pDevice, ID3D12CommandQueue * pDirectCommandQueue, ID3D12GraphicsCommandList * pCommandList, UINT frameIndex)
	{
		CreateDescriptorHeaps(pDevice);
		CreateRootSignatures(pDevice);
		CreatePipelineStates(pDevice);
		CreatePostprocessPassResources(pDevice);
		CreateSamplers(pDevice);
		CreateBaseFrames(pDevice, pDirectCommandQueue);
		CreateCommandLists(pDevice);
		CreateAssetResources(pDevice, pCommandList);
		SetFrameIndex(frameIndex);
	}

	// Load resources that are dependent on the size of the main window.
	void BaseScene::LoadSizeDependentResources(ID3D12Device * pDevice, ComPtr<ID3D12Resource>*ppRenderTargets, UINT width, UINT height)
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
		ResetComPtrArray(&m_rootSignatures);
		PipelineStatePool::ResetComPtrs();
//		ResetComPtrArray(&m_pipelineStates);
		ResetComPtrArray(&m_vertexBuffers);
		ResetComPtrArray(&m_rootSignatures);
		m_rtvHeap.Reset();
		m_dsvHeap.Reset();
		m_cbvSrvUavHeap.Reset();
		m_samplerHeap.Reset();
		ResetUniquePtrArray(&m_frameResources);
		m_pCurrentBaseFrame = nullptr;
	}

	// Caller is expected to enforce frame synchronization and that the GPU is done with frameIndex frame before being set as current frame again.
	void BaseScene::SetFrameIndex(UINT frameIndex)
	{
		m_frameIndex = frameIndex;
		m_pCurrentBaseFrame = m_frameResources[m_frameIndex].get();
	}

	void BaseScene::KeyUp(UINT8 key)
	{
	}

	void BaseScene::KeyDown(UINT8 key)
	{
	}

	//フレーム処理の準備
	void BaseScene::BeginFrame() {
		m_pCurrentBaseFrame->InitFrame();

		ID3D12GraphicsCommandList* pCommandListPre = m_commandLists[CommandListPre].Get();

		// Reset the command list.
		ThrowIfFailed(pCommandListPre->Reset(m_pCurrentBaseFrame->m_commandAllocator.Get(), nullptr));

		m_pCurrentBaseFrame->BeginFrame(pCommandListPre);

		// Transition back-buffer to a writable state for rendering.
		pCommandListPre->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		// Clear the render target and depth stencils.
		pCommandListPre->ClearRenderTargetView(GetCurrentBackBufferRtvCpuHandle(), s_clearColor, 0, nullptr);
		pCommandListPre->ClearDepthStencilView(m_depthDsvs[DepthGenPass::Shadow], D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		pCommandListPre->ClearDepthStencilView(m_depthDsvs[DepthGenPass::Scene], D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		// Close the command list.
		ThrowIfFailed(pCommandListPre->Close());

	}
	//フレームのリセット
	void BaseScene::MidFrame() {
		ID3D12GraphicsCommandList* pCommandListMid = m_commandLists[CommandListMid].Get();

		// Reset the command list.
		ThrowIfFailed(pCommandListMid->Reset(m_pCurrentBaseFrame->m_commandAllocator.Get(), nullptr));

		// Transition our shadow map to a readable state for scene rendering.
		pCommandListMid->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_depthTextures[DepthGenPass::Shadow].Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		// Close the command list.
		ThrowIfFailed(pCommandListMid->Close());

	}
	//フレームの終了
	void BaseScene::EndFrame(bool setBackbufferReadyForPresent) {
		ID3D12GraphicsCommandList* pCommandListPost = m_commandLists[CommandListPost].Get();

		// Reset the command list.
		ThrowIfFailed(pCommandListPost->Reset(m_pCurrentBaseFrame->m_commandAllocator.Get(), nullptr));

		// Set descriptor heaps.
		ID3D12DescriptorHeap* ppHeaps[] = { m_cbvSrvUavHeap.Get(), m_samplerHeap.Get() };
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

		m_pCurrentBaseFrame->EndFrame(pCommandListPost);

		// Close the command list.
		ThrowIfFailed(pCommandListPost->Close());
	}

	void BaseScene::PostprocessPass(ID3D12GraphicsCommandList* pCommandList) {
		// Set necessary state.
		pCommandList->SetGraphicsRootSignature(m_rootSignatures[RootSignature::PostprocessPass].Get());

		auto postprosessPipeline = PipelineStatePool::GetPipelineState(L"defaultPostprocess");

		pCommandList->SetPipelineState(postprosessPipeline.Get());

		auto currentBackBufferRtvCpuHandle = GetCurrentBackBufferRtvCpuHandle();
		// Set necessary state.
		pCommandList->SetGraphicsRootDescriptorTable(0, m_depthSrvGpuHandles[DepthGenPass::Scene]); // Set scene depth as an SRV.
		pCommandList->SetGraphicsRootConstantBufferView(1, m_pCurrentBaseFrame->GetConstantBufferGPUVirtualAddress(RenderPass::Postprocess)); // Set postprocess constant buffer.
		pCommandList->SetGraphicsRootDescriptorTable(2, m_samplerHeap->GetGPUDescriptorHandleForHeapStart()); // Set samplers.

		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		pCommandList->IASetVertexBuffers(0, 1, &m_vertexBufferViews[VertexBuffer::ScreenQuad]);
		pCommandList->RSSetViewports(1, &m_viewport);
		pCommandList->RSSetScissorRects(1, &m_scissorRect);
		pCommandList->OMSetRenderTargets(1, &currentBackBufferRtvCpuHandle, FALSE, nullptr); // No depth stencil needed for in-scattering.
		// Draw.
		pCommandList->DrawInstanced(4, 1, 0, 0);
	}

	//シャドウとシーンの描画
	void BaseScene::WorkerThread()
	{
		//
		// Shadow pass
		//
		{
			ID3D12GraphicsCommandList* pShadowCommandList = m_shadowCommandList.Get();

			auto shadowPipeline = PipelineStatePool::GetPipelineState(L"defaultShadow");
			// Reset コマンドライン
			ThrowIfFailed(pShadowCommandList->Reset(m_pCurrentBaseFrame->m_contextCommandAllocator.Get(), shadowPipeline.Get()));

			// シャドウの描画
			ShadowPass(pShadowCommandList);

			// クローズ
			ThrowIfFailed(pShadowCommandList->Close());
		}

		//
		// Scene pass
		//
		{
			ID3D12GraphicsCommandList* pSceneCommandList = m_sceneCommandList.Get();
			// コマンドリストのリセット
			//各オブジェクトで描画方法が違うのでnullptrでリセット
			ThrowIfFailed(pSceneCommandList->Reset(m_pCurrentBaseFrame->m_contextCommandAllocator.Get(), nullptr));
			//ディスクプリタヒープのセット（cbvSrvUavおよびサンプラー）
			ID3D12DescriptorHeap* ppHeaps[] = { m_cbvSrvUavHeap.Get(), m_samplerHeap.Get() };
			pSceneCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
			//シーンの描画
			ScenePass(pSceneCommandList);
			// クローズ
			ThrowIfFailed(pSceneCommandList->Close());

		}
	}



	// Render the scene.
	void BaseScene::Render(ID3D12CommandQueue * pCommandQueue, bool setBackbufferReadyForPresent)
	{
		BeginFrame();
		// Shadow generation and scene render passes
		WorkerThread();
		MidFrame();
		EndFrame(setBackbufferReadyForPresent);
		pCommandQueue->ExecuteCommandLists(_countof(m_batchSubmit), m_batchSubmit);
		//pCommandQueue->ExecuteCommandLists(_countof(m_pCurrentBaseFrame->m_batchSubmit), m_pCurrentBaseFrame->m_batchSubmit);
	}



}
// end namespace basecross 
