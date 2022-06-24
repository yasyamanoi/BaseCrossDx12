/*!
@file BaseDevice.cpp
@brief デバイスクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/
#include "stdafx.h"

namespace basecross {

	using namespace basecross;

	IMPLEMENT_DX12SHADER(VSPNTStaticInit, App::GetShadersDir() + L"SpVSPNTStaticShadow.cso")
	IMPLEMENT_DX12SHADER(PSPNTStaticInit, App::GetShadersDir() + L"SpPSPNTStaticShadow.cso")

	BaseDevice::BaseDevice(UINT width, UINT height, const wstring& title) :
		m_width(width),
		m_height(height),
		m_title(title),
		m_quiteEscapeKey(true),
		m_useWarpDevice(false),
		m_frameIndex(0),
		m_fenceEvent(nullptr),
		m_fenceValue(0),
		m_viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
		m_scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)),
		m_clearColor(0, 0, 0, 1),
		m_rtvDescriptorIncrementSize(0),
		m_dsvDescriptorIncrementSize(0),
		m_samplerSendIndex(0),
		m_srvSendIndex(m_srvStartIndex),
		m_cbvUavSendIndex(m_cbvUavStartIndex),
		m_process(process::pipelineInit),
		m_stageReCreated(false)
	{
		m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
		ThrowIfFailed(DXGIDeclareAdapterRemovalSupport());
	}

	BaseDevice::~BaseDevice()
	{
	}




	_Use_decl_annotations_
	void BaseDevice::ParseCommandLineArgs(WCHAR* argv[], int argc)
	{
		for (int i = 1; i < argc; ++i)
		{
			if (_wcsnicmp(argv[i], L"-warp", wcslen(argv[i])) == 0 ||
				_wcsnicmp(argv[i], L"/warp", wcslen(argv[i])) == 0)
			{
				m_useWarpDevice = true;
				m_title = m_title + L" (WARP)";
			}
		}
	}

	_Use_decl_annotations_
	void BaseDevice::GetHardwareAdapter(
			IDXGIFactory1* pFactory,
			IDXGIAdapter1** ppAdapter,
			bool requestHighPerformanceAdapter)
	{
		*ppAdapter = nullptr;

		ComPtr<IDXGIAdapter1> adapter;

		ComPtr<IDXGIFactory6> factory6;
		if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
		{
			for (
				UINT adapterIndex = 0;
				SUCCEEDED(factory6->EnumAdapterByGpuPreference(
					adapterIndex,
					requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
					IID_PPV_ARGS(&adapter)));
				++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					// If you want a software adapter, pass in "/warp" on the command line.
					continue;
				}

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}
		}

		if (adapter.Get() == nullptr)
		{
			for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					// If you want a software adapter, pass in "/warp" on the command line.
					continue;
				}

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}
		}
		*ppAdapter = adapter.Detach();
	}



	void BaseDevice::OnCreate() {
		LoadPipeline();
		LoadAssets();
	}


	//パイプラインオブジェクトの作成
	void BaseDevice::LoadPipeline() {

		//Dx12デバッグ環境の設定
		UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
		{
			ComPtr<ID3D12Debug> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();

				// Enable additional debug layers.
				dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			}
		}
#endif
		ComPtr<IDXGIFactory4> factory;
		ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

		//デバイスの作成
		if (m_useWarpDevice)
		{
			//ラップモード
			ComPtr<IDXGIAdapter> warpAdapter;
			ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

			ThrowIfFailed(D3D12CreateDevice(
				warpAdapter.Get(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&m_device)
			));
		}
		else
		{
			ComPtr<IDXGIAdapter1> hardwareAdapter;
			GetHardwareAdapter(factory.Get(), &hardwareAdapter, true);

			ThrowIfFailed(D3D12CreateDevice(
				hardwareAdapter.Get(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&m_device)
			));
		}

		//コマンドキューの作成
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
		NAME_D3D12_OBJECT(m_commandQueue);

		// スワップチェーンの作成
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = m_frameCount;
		swapChainDesc.Width = m_width;
		swapChainDesc.Height = m_height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;
		ComPtr<IDXGISwapChain1> swapChain;
		ThrowIfFailed(factory->CreateSwapChainForHwnd(
			m_commandQueue.Get(),
			App::GetHwnd(),
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain
		));
		// ALT + ENTERによるフルスクリーンはサポートしない
		ThrowIfFailed(factory->MakeWindowAssociation(App::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));

		//スワップチェーンをバージョンアップして保存
		ThrowIfFailed(swapChain.As(&m_swapChain));
		//現在のバックバッファ番号を取得
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		//デスクプリタヒープの作成
		{
			//Rtv、フレーム数だけ作成する
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = m_frameCount;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

			// dsvのディスクプリタヒープはデプスステンシルとシャドウマップ用にフレーム数と同じ数
			// そしてシーンが自分自身の描画用に1つ作成する
			D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
			// 0: 描画用dsv
			// 1から3: シャドウマップ用dsv
			// 全部で4つ
			dsvHeapDesc.NumDescriptors = 1 + m_frameCount * 1;
			dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			ThrowIfFailed(m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));

			// CBVとSRVとUAV用のデスクプリタヒープ
			D3D12_DESCRIPTOR_HEAP_DESC cbvSrvUavHeapDesc = {};
			cbvSrvUavHeapDesc.NumDescriptors = m_cbvSrvUavMax; //全部で4096個
			cbvSrvUavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			cbvSrvUavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			ThrowIfFailed(m_device->CreateDescriptorHeap(&cbvSrvUavHeapDesc, IID_PPV_ARGS(&m_cbvSrvUavHeap)));
			NAME_D3D12_OBJECT(m_cbvSrvUavHeap);

			//サンプラー用.
			D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
			samplerHeapDesc.NumDescriptors = m_samplerMax;        // 最大512
			samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			ThrowIfFailed(m_device->CreateDescriptorHeap(&samplerHeapDesc, IID_PPV_ARGS(&m_samplerHeap)));
			NAME_D3D12_OBJECT(m_samplerHeap);

			//RTV用のインクリメントサイズ
			m_rtvDescriptorIncrementSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			//DSV用のインクリメントサイズ
			m_dsvDescriptorIncrementSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			//サンプラー用のインクリメントサイズ
			m_samplerDescriptorIncrementSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
			//CBV_SRV_UAV用のインクリメントサイズ
			m_cbvSrvUavDescriptorIncrementSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		}
		//コマンドアロケーター
		ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
	}

	UINT BaseDevice::GetSrvNextIndex() {
		if (m_srvSendIndex >= m_srvMax) {
			throw BaseException(
				L"これ以上シェーダリソースは増やせません。\n",
				L"BaseDevice::GetSrvNextIndex()"
			);
		}
		return m_srvSendIndex++;
	}

	UINT BaseDevice::GetCbvUavNextIndex() {
		if (m_cbvUavSendIndex >= m_cbvUavMax) {
			throw BaseException(
				L"これ以上コンスタントバッファとUAVは増やせません。\n",
				L"BaseDevice::GetCbvUavNextIndex()"
			);
		}
		return m_cbvUavSendIndex++;
	}

	UINT BaseDevice::GetSamplerNextIndex() {
		if (m_samplerSendIndex >= m_samplerMax) {
			throw BaseException(
				L"これ以上サンプラーは増やせません。\n",
				L"BaseDevice::GetSamplerNextIndex()"
			);
		}
		return m_samplerSendIndex++;
	}

	UINT BaseDevice::GetSamplerIndex(const wstring& key) {
		auto it = m_samplerMap.find(key);
		if (it != m_samplerMap.end()) {
			return it->second;
		}
		return UINT_MAX;
	}


	ComPtr<ID3D12GraphicsCommandList> BaseDevice::GetComandList()const {
		switch (m_process) {
		case process::pipelineInit:
			return m_initCommandList;
			break;
		case process::update:
			return GetCurrentBaseFrame()->m_updateCommandList;
			break;
		case process::begin:
			return GetCurrentBaseFrame()->m_commandLists[m_commandListPre];
			break;
		case process::shadowmap:
			return GetCurrentBaseFrame()->m_shadowCommandList;
			break;
		case process::scene:
			return GetCurrentBaseFrame()->m_sceneCommandList;
			break;
		case process::mid:
			return GetCurrentBaseFrame()->m_commandLists[m_commandListMid];
			break;
		case process::end:
			return GetCurrentBaseFrame()->m_commandLists[m_commandListPost];
			break;
		}
		throw BaseException(
			L"コマンドリストのプロセスが特定できません。",
			L"BaseDevice::GetComandList()"
		);
		return nullptr;

	}


	//アセットオブジェクトの作成
	void BaseDevice::LoadAssets() {
		// Create the root signature.
		CreateRootSignature();
		// Create the pipeline state, which includes loading shaders.
		CreatePipelineStates();
		// Create temporary command list for initial GPU setup.
		auto pipeState = BasicPipelineStatePool::GetPipelineState(L"initPipelineState");
		if (!pipeState) {
			throw BaseException(
				L"初期化用のパイプラインステートが取得できません",
				L"BaseDevice::LoadAssets()"
			);
		}
		ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), pipeState.Get(), IID_PPV_ARGS(&m_initCommandList)));
		m_process = process::pipelineInit;
		// Create render target views (RTVs).
		CreateRenderTargetViews();
		// Create the depth stencil.
		CreateDepthStencil();
		// Create the samplers.
		CreateSamplers();
		//シーンオブジェクト構築
		auto pScene = App::GetBaseScene();
		pScene->OnPreCreate();
		pScene->OnCreate();
		// Close the command list and use it to execute the initial GPU setup.
		ThrowIfFailed(m_initCommandList->Close());
		ID3D12CommandList* ppCommandLists[] = { m_initCommandList.Get() };
		m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		// Create frame resources.
		CreateFrameResources();
		// Create synchronization objects and wait until assets have been uploaded to the GPU.
		CreateSynchronizationObjects();

	}

	//ルート署名の作成
	void BaseDevice::CreateRootSignature() {
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

		if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		CD3DX12_DESCRIPTOR_RANGE1 ranges[12]; // Perfomance TIP: Order from most frequent to least frequent.

		//ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);	// 1 frequently changed diffuse texture using register t1.
		//ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);	// 1 frequently changed normal texture using register t2.
		//ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);	// 1 frequently changed constant buffer.
		//ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);												// 1 infrequently changed shadow texture - starting in register t0.
		//ranges[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);						// 1 static samplers.
		//ranges[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 1);						// 1 static samplers.


		//ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);	// 1 frequently changed diffuse texture using register t1.
		//ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);	// 1 frequently changed normal texture using register t2.
		//ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);	// 1 frequently changed constant buffer.
		//ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);												// 1 infrequently changed shadow texture - starting in register t0.
		//ranges[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);						// 1 static samplers.
		//ranges[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);						// 1 static samplers.

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
		rootParameters[4].InitAsDescriptorTable(1, &ranges[4], D3D12_SHADER_VISIBILITY_ALL); //b0
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
		//以下はシンプルなもの
//		rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;


		ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
		ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
		NAME_D3D12_OBJECT(m_rootSignature);

	}

	void BaseDevice::CreatePipelineStates() {

		ComPtr<ID3D12PipelineState> initPipelineState;
		ComPtr<ID3D12PipelineState> shadowMapPipelineState;


		CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		depthStencilDesc.StencilEnable = FALSE;

		// Describe and create the PSO for rendering the scene.
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { VertexPositionNormalTexture::GetVertexElement(), VertexPositionNormalTexture::GetNumElements() };;
		psoDesc.pRootSignature = m_rootSignature.Get();
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(VSPNTStaticInit::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			VSPNTStaticInit::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<UINT8*>(PSPNTStaticInit::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			PSPNTStaticInit::GetPtr()->GetShaderComPtr()->GetBufferSize()
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

		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&initPipelineState)));
		NAME_D3D12_OBJECT(initPipelineState);
		//保存するパイプ来ステートに追加
		BasicPipelineStatePool::AddPipelineState(L"initPipelineState", initPipelineState);


		// Alter the description and create the PSO for rendering
		// the shadow map.  The shadow map does not use a pixel
		// shader or render targets.
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(0, 0);
		psoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
		psoDesc.NumRenderTargets = 0;

		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&shadowMapPipelineState)));
		NAME_D3D12_OBJECT(shadowMapPipelineState);

		//保存するパイプ来ステートに追加
		BasicPipelineStatePool::AddPipelineState(L"shadowMapPipelineState", shadowMapPipelineState);

	}

	void BaseDevice::CreateRenderTargetViews() {
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < m_frameCount; i++)
		{
			ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));
			m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorIncrementSize);
			NAME_D3D12_OBJECT_INDEXED(m_renderTargets, i);
		}
	}

	void BaseDevice::CreateDepthStencil() {
		CD3DX12_RESOURCE_DESC shadowTextureDesc(
			D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			0,
			static_cast<UINT>(m_viewport.Width),
			static_cast<UINT>(m_viewport.Height),
			1,
			1,
			DXGI_FORMAT_D32_FLOAT,
			1,
			0,
			D3D12_TEXTURE_LAYOUT_UNKNOWN,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE
		);

		D3D12_CLEAR_VALUE clearValue;
		clearValue.Format = DXGI_FORMAT_D32_FLOAT;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;

		ThrowIfFailed(m_device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&shadowTextureDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(&m_depthStencil))
		);
		NAME_D3D12_OBJECT(m_depthStencil);

		m_device->CreateDepthStencilView(m_depthStencil.Get(), nullptr, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
	}


	void BaseDevice::CreateSamplers() {
		//LinearWrap
		// 通常描画リニアラップサンプラー
		UINT index = GetSamplerNextIndex();
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
		wrapSamplerDesc.BorderColor[0]
			= wrapSamplerDesc.BorderColor[1]
			= wrapSamplerDesc.BorderColor[2]
			= wrapSamplerDesc.BorderColor[3]
			= 0;
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle1(
			m_samplerHeap->GetCPUDescriptorHandleForHeapStart(),
			index,
			m_samplerDescriptorIncrementSize
		);
		m_device->CreateSampler(&wrapSamplerDesc, handle1);
		m_samplerMap[L"LinearWrap"] = index;


		//PointClamp
		index = GetSamplerNextIndex();
		D3D12_SAMPLER_DESC clampSamplerDesc = {};
		clampSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		clampSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		clampSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		clampSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		clampSamplerDesc.MipLODBias = 0.0f;
		clampSamplerDesc.MaxAnisotropy = 1;
		clampSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		clampSamplerDesc.BorderColor[0]
			= clampSamplerDesc.BorderColor[1]
			= clampSamplerDesc.BorderColor[2]
			= clampSamplerDesc.BorderColor[3]
			= 0;
		clampSamplerDesc.MinLOD = 0;
		clampSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle0(
			m_samplerHeap->GetCPUDescriptorHandleForHeapStart(),
			index,
			m_samplerDescriptorIncrementSize
		);
		m_device->CreateSampler(&clampSamplerDesc, handle0);
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
			m_samplerDescriptorIncrementSize
		);
		m_device->CreateSampler(&linearClampSamplerDesc, handle2);
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
			m_samplerDescriptorIncrementSize
		);
		m_device->CreateSampler(&ComparisonLinearSamplerDesc, handle3);
		m_samplerMap[L"ComparisonLinear"] = index;

	}

	void BaseDevice::CreateFrameResources() {
		auto pipeState = BasicPipelineStatePool::GetPipelineState(L"initPipelineState");
		if (!pipeState) {
			throw BaseException(
				L"初期化用のパイプラインステートが取得できません",
				L"BaseDevice::CreateFrameResources()"
			);
		}
		auto shadowPipeState = BasicPipelineStatePool::GetPipelineState(L"shadowMapPipelineState");
		if (!shadowPipeState) {
			throw BaseException(
				L"シャドウマップ用のパイプラインステートが取得できません",
				L"BaseDevice::CreateFrameResources()"
			);
		}
		for (int i = 0; i < m_frameCount; i++)
		{
			m_baseFrames[i] = new BaseFrame(m_device.Get(), pipeState.Get(), shadowPipeState.Get(), m_dsvHeap.Get(), m_cbvSrvUavHeap.Get(), &m_viewport, i);
			App::GetBaseScene()->OnInitFrame(m_baseFrames[i]);
			App::GetBaseScene()->WriteConstantBuffers(m_baseFrames[i]);
		}
		m_currentBaseFrameIndex = 0;
		m_pCurrentBaseFrame = m_baseFrames[m_currentBaseFrameIndex];

	}

	void BaseDevice::CreateSynchronizationObjects() {
		ThrowIfFailed(m_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
		m_fenceValue++;

		// Create an event handle to use for frame synchronization.
		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_fenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}

		// Wait for the command list to execute; we are reusing the same command 
		// list in our main loop but for now, we just want to wait for setup to 
		// complete before continuing.

		// Signal and increment the fence value.
		const UINT64 fenceToWaitFor = m_fenceValue;
		ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fenceToWaitFor));
		m_fenceValue++;

		// Wait until the fence is completed.
		ThrowIfFailed(m_fence->SetEventOnCompletion(fenceToWaitFor, m_fenceEvent));
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}


	//スレッド用の変数
	static bool g_threadEnded;
	static string g_msg;
	//ミューテックス
	static std::mutex g_mtx;

	//メッセージボックス用のスレッド
	static void MsgBoxFunc() {
		g_mtx.lock();
		g_threadEnded = false;
		g_mtx.unlock();

		MessageBoxA(nullptr, g_msg.c_str(), "エラー", MB_OK);
		g_mtx.lock();
		g_threadEnded = true;
		g_mtx.unlock();
	}


	//ランタイムルーチン
	//エラー表示用に、スレッドを新しく切ってメッセージボックスを表示
	void BaseDevice::OnUpdateDraw() {

		int retCode = 0;
		try {
#if defined(_DEBUG)
			static bool loopStart = false;
			if (!loopStart) {
				OutputDebugStringA("loopStart\n");
				loopStart = true;

			}
#endif
			App::GetInputDevice().ResetControlerState();
			OnUpdate();
			OnDraw();
		}
		catch (BaseException& e) {
			//デバッグ出力をする。
			string str = e.what_m() + "\n";
			OutputDebugStringA(str.c_str());
			//メッセージボックス
			g_msg = e.what_m() + "\n";
			//メッセージボックススレッドのスタート
			std::thread MsgThread(MsgBoxFunc);
			MsgThread.join();
			retCode = 1;
		}
		catch (runtime_error& e) {
			//デバッグ出力をする。
			string str(e.what());
			str += "\n";
			OutputDebugStringA(str.c_str());
			//メッセージボックス
			g_msg = e.what();
			g_msg += "\n";
			//メッセージボックススレッドのスタート
			std::thread MsgThread(MsgBoxFunc);
			MsgThread.join();
			retCode = 1;
		}
		catch (exception& e) {
			//STLエラー
			string str(e.what());
			str += "\n";
			OutputDebugStringA(str.c_str());
			//メッセージボックス
			g_msg = e.what();
			g_msg += "\n";
			//メッセージボックススレッドのスタート
			std::thread MsgThread(MsgBoxFunc);
			MsgThread.join();

			retCode = 1;
		}
		catch (...) {
			OutputDebugStringA("原因不明のエラー\n");

			//メッセージボックス
			g_msg = "原因不明のエラー\n";
			//メッセージボックススレッドのスタート
			std::thread MsgThread(MsgBoxFunc);
			MsgThread.join();

			retCode = 1;
		}
		if (retCode) {
			PostQuitMessage(1);
		}
	}


	void BaseDevice::OnUpdate(){
		m_timer.Tick(NULL);
		const UINT64 lastCompletedFence = m_fence->GetCompletedValue();
		m_currentBaseFrameIndex = (m_currentBaseFrameIndex + 1) % m_frameCount;
		m_pCurrentBaseFrame = m_baseFrames[m_currentBaseFrameIndex];
		if (GetCurrentBaseFrame()->m_fenceValue != 0 && GetCurrentBaseFrame()->m_fenceValue > lastCompletedFence)
		{
			HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (eventHandle == nullptr)
			{
				ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
			}
			else {
				ThrowIfFailed(m_fence->SetEventOnCompletion(GetCurrentBaseFrame()->m_fenceValue, eventHandle));
				WaitForSingleObject(eventHandle, INFINITE);
				CloseHandle(eventHandle);
			}
		}

		auto pipelineState = BasicPipelineStatePool::GetPipelineState(L"initPipelineState");
		if (!pipelineState) {
			throw BaseException(
				L"初期化更新用のパイプラインステートが取得できません",
				L"BaseDevice::OnUpdate()"
			);
		}

		//コマンドリスト開始。
		ThrowIfFailed(GetCurrentBaseFrame()->m_updateCommandAllocator->Reset());
		ThrowIfFailed(GetCurrentBaseFrame()->m_updateCommandList->Reset(GetCurrentBaseFrame()->m_updateCommandAllocator.Get(), pipelineState.Get()));
		m_process = process::update;
		//シーンのイベントを実行
		App::GetBaseScene()->DispatchDelayedEvent();
		if (IsStageReCreated()) {
			//コンスタンとバッファ領域を初期化
			m_cbvUavSendIndex = m_cbvUavStartIndex;

			for (int i = 0; i < m_frameCount; i++)
			{
				App::GetBaseScene()->OnInitFrame(m_baseFrames[i]);
				App::GetBaseScene()->WriteConstantBuffers(m_baseFrames[i]);
			}
			SetStageReCreated(false);
		}
		//シーンのOnUpdate
		App::GetBaseScene()->OnUpdate();
		App::GetBaseScene()->WriteConstantBuffers(GetCurrentBaseFrame());
		ThrowIfFailed(GetCurrentBaseFrame()->m_updateCommandList->Close());

		ID3D12CommandList* ppCommandLists[] = {
			GetCurrentBaseFrame()->m_updateCommandList.Get()
		};
		//updateコマンドリストの実行
		m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);


	}

	void BaseDevice::BeginFrame()
	{
		m_pCurrentBaseFrame->Init();
		m_process = process::begin;

		// Indicate that the back buffer will be used as a render target.
		m_pCurrentBaseFrame->m_commandLists[m_commandListPre]->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		// Clear the render target and depth stencil.
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorIncrementSize);
		m_pCurrentBaseFrame->m_commandLists[m_commandListPre]->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		m_pCurrentBaseFrame->m_commandLists[m_commandListPre]->ClearDepthStencilView(m_dsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		ThrowIfFailed(m_pCurrentBaseFrame->m_commandLists[m_commandListPre]->Close());

	}

	void BaseDevice::MidFrame() {
		m_pCurrentBaseFrame->SwapBarriers();
		m_process = process::mid;

		ThrowIfFailed(m_pCurrentBaseFrame->m_commandLists[m_commandListMid]->Close());

	}

	void BaseDevice::EndFrame()
	{
		m_pCurrentBaseFrame->Finish();

		// Indicate that the back buffer will now be used to present.
		m_pCurrentBaseFrame->m_commandLists[m_commandListPost]->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
		m_process = process::end;

		ThrowIfFailed(m_pCurrentBaseFrame->m_commandLists[m_commandListPost]->Close());

	}


	void BaseDevice::WorkerThread() {
			ID3D12GraphicsCommandList* pShadowCommandList = m_pCurrentBaseFrame->m_shadowCommandList.Get();
			ID3D12GraphicsCommandList* pSceneCommandList = m_pCurrentBaseFrame->m_sceneCommandList.Get();

			//
			// Shadow pass
			//
			m_process = process::shadowmap;

			// Populate the command list.
			SetCommonPipelineState(pShadowCommandList);
			m_pCurrentBaseFrame->Bind(pShadowCommandList, FALSE, nullptr, nullptr);    // No need to pass RTV or DSV descriptor heap.

			//シーンのシャドウマップ描画を行う
			App::GetBaseScene()->PopulateShadowmapCommandList(m_pCurrentBaseFrame);

			ThrowIfFailed(pShadowCommandList->Close());


			//
			// Scene pass
			// 
			m_process = process::scene;

			// Populate the command list.  These can only be sent after the shadow 
			// passes for this frame have been submitted.
			SetCommonPipelineState(pSceneCommandList);
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorIncrementSize);
			CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
			m_pCurrentBaseFrame->Bind(pSceneCommandList, TRUE, &rtvHandle, &dsvHandle);

			//シーンの描画を行う
			App::GetBaseScene()->PopulateCommandList(m_pCurrentBaseFrame);

			ThrowIfFailed(pSceneCommandList->Close());

	}

	void BaseDevice::SetCommonPipelineState(ID3D12GraphicsCommandList* pCommandList) {
		pCommandList->SetGraphicsRootSignature(m_rootSignature.Get());

		ID3D12DescriptorHeap* ppHeaps[] = { m_cbvSrvUavHeap.Get(), m_samplerHeap.Get() };
		pCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		pCommandList->RSSetViewports(1, &m_viewport);
		pCommandList->RSSetScissorRects(1, &m_scissorRect);

	}

	void BaseDevice::OnDraw() {

		BeginFrame();
		WorkerThread();
		MidFrame();
		EndFrame();
		//描画系のコマンドリストを集める
		ID3D12CommandList* ppCommandLists[] = {
			GetCurrentBaseFrame()->m_commandLists[m_commandListPre].Get(),
			GetCurrentBaseFrame()->m_shadowCommandList.Get(),
			GetCurrentBaseFrame()->m_sceneCommandList.Get(),
			GetCurrentBaseFrame()->m_commandLists[m_commandListMid].Get(),
			GetCurrentBaseFrame()->m_commandLists[m_commandListPost].Get(),
		};
		m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		// Present and update the frame index for the next frame.
		ThrowIfFailed(m_swapChain->Present(1, 0));
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
		// Signal and increment the fence value.
		GetCurrentBaseFrame()->m_fenceValue = m_fenceValue;
		ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValue));
		m_fenceValue++;
	}
	void BaseDevice::OnDestroy() {
#if defined(_DEBUG)
		OutputDebugStringA("deviceDestroyStart\n");
#endif
		//シーンの開放
		App::GetBaseScene()->OnDestroy();
		// GPUリソースの開放
		{
			const UINT64 fence = m_fenceValue;
			const UINT64 lastCompletedFence = m_fence->GetCompletedValue();

			ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValue));
			m_fenceValue++;

			if (lastCompletedFence < fence)
			{
				ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
				WaitForSingleObject(m_fenceEvent, INFINITE);
			}
			CloseHandle(m_fenceEvent);
		}

		for (int i = 0; i < m_frameCount; i++)
		{
			delete m_baseFrames[i];
		}
#if defined(_DEBUG)
		OutputDebugStringA("deviceDestroyEnd\n");
#endif

	}
	void BaseDevice::OnKeyDown(UINT8 key) {
		App::GetBaseScene()->OnKeyDown(key);
	}
	void BaseDevice::OnKeyUp(UINT8 key) {
		App::GetBaseScene()->OnKeyUp(key);
	}

}
// end namespace basecross
