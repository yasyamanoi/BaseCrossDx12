/*!
@file BaseDevice.cpp
@brief �f�o�C�X�N���X
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/
#include "stdafx.h"

namespace basecross {

	IMPLEMENT_DX12SHADER(VSPNTStaticInit, App::GetShadersDir() + L"BcVSPNTStaticPL.cso")
	IMPLEMENT_DX12SHADER(PSPNTStaticInit, App::GetShadersDir() + L"BcPSPNTPL.cso")


	using namespace basecross;

	BaseDevice::BaseDevice(UINT width, UINT height, const wstring& title) :
		m_width(width),
		m_height(height),
		m_title(title),
		m_QuiteEscapeKey(true),
		m_useWarpDevice(false),
		m_viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
		m_scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)),
		m_clearColor(0, 0, 0, 1),
		m_frameIndex(0),
		m_fenceEvent(nullptr),
		m_fenceValue(0),
		m_isInited(false),
		m_process(process::init),
		m_rtvDescriptorIncrementSize(0),
		m_dsvDescriptorIncrementSize(0),
		m_samplerSendIndex(0),
		m_srvSendIndex(m_srvStartIndex),
		m_cbvUavSendIndex(m_cbvUavStartIndex)
	{
		m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
		ThrowIfFailed(DXGIDeclareAdapterRemovalSupport());
	}

	BaseDevice::~BaseDevice()
	{
	}

	UINT BaseDevice::GetSamplerNextIndex() {
		if (m_samplerSendIndex >= m_samplerMax) {
			throw BaseException(
				L"����ȏ�T���v���[�͑��₹�܂���B\n",
				L"BaseDevice::GetSamplerNextIndex()"
			);
		}
		return m_samplerSendIndex++;
	}

	ComPtr<ID3D12GraphicsCommandList> BaseDevice::GetComandList()const {
		switch (m_process) {
		case process::init:
			return m_initCommandList;
			break;
		case process::update:
			return GetCurrentBaseFrame()->m_updateCommandList;
			break;
		case process::begin:
			return GetCurrentBaseFrame()->m_beginCommandList;
			break;
		case process::shadowmap:
			return GetCurrentBaseFrame()->m_shadowCommandList;
			break;
		case process::render:
			return GetCurrentBaseFrame()->m_sceneCommandList;
			break;
		case process::end:
			return GetCurrentBaseFrame()->m_endCommandList;
			break;
		}
		throw BaseException(
			L"�R�}���h���X�g�̃v���Z�X������ł��܂���B",
			L"BaseDevice::GetComandList()"
		);
		return nullptr;
	}

	UINT BaseDevice::GetSamplerIndex(const wstring& key) {
		auto it = m_samplerMap.find(key);
		if (it != m_samplerMap.end()) {
			return it->second;
		}
		return UINT_MAX;
	}



	UINT BaseDevice::GetSrvNextIndex() {
		if (m_srvSendIndex >= m_srvMax) {
			throw BaseException(
				L"����ȏ�V�F�[�_���\�[�X�͑��₹�܂���B\n",
				L"BaseDevice::GetSrvNextIndex()"
			);
		}
		return m_srvSendIndex++;
	}

	UINT BaseDevice::GetCbvUavNextIndex() {
		if (m_cbvUavSendIndex >= m_cbvUavMax) {
			throw BaseException(
				L"����ȏ�R���X�^���g�o�b�t�@��UAV�͑��₹�܂���B\n",
				L"BaseDevice::GetCbvUavNextIndex()"
			);
		}
		return m_cbvUavSendIndex++;
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

		if(adapter.Get() == nullptr)
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

	void BaseDevice::OnInit() {
		LoadPipeline();
		LoadAssets();
	}
	void BaseDevice::LoadPipeline() {
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
		//�f�o�C�X�̍쐬
		if (m_useWarpDevice)
		{
			//���b�v���[�h
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

		//�R�}���h�L���[�̍쐬
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
		NAME_D3D12_OBJECT(m_commandQueue);

		//�X���b�v�`�F�[���̍쐬
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
			m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
			App::GetHwnd(),
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain
		));

		// ALT + ENTER�ɂ��t���X�N���[���̓T�|�[�g���Ȃ�
		ThrowIfFailed(factory->MakeWindowAssociation(App::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));
		//�X���b�v�`�F�[�����o�[�W�����A�b�v���ĕۑ�
		ThrowIfFailed(swapChain.As(&m_swapChain));
		//���݂̃o���N�o�b�t�@�ԍ����擾
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		//�f�X�N�v���^�q�[�v�̍쐬
		{
			//Rtv�A�t���[���������쐬����
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = m_frameCount;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

			// dsv�̃f�B�X�N�v���^�q�[�v�̓f�v�X�X�e���V���ƃV���h�E�}�b�v�p�Ƀt���[�����Ɠ�����
			// �����ăV�[�����������g�̕`��p��1�쐬����
			D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
			dsvHeapDesc.NumDescriptors = 1 + m_frameCount * 1;
			// 0: �`��pdsv
			// 1����3: �V���h�E�}�b�v�pdsv
			// �S����4��
			dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			ThrowIfFailed(m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));

			// CBV��SRV��UAV�p�̃f�X�N�v���^�q�[�v
			D3D12_DESCRIPTOR_HEAP_DESC cbvSrvUavHeapDesc = {};
			cbvSrvUavHeapDesc.NumDescriptors = m_cbvSrvUavMax; //�S����4096��
			cbvSrvUavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			cbvSrvUavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			ThrowIfFailed(m_device->CreateDescriptorHeap(&cbvSrvUavHeapDesc, IID_PPV_ARGS(&m_cbvSrvUavHeap)));
			NAME_D3D12_OBJECT(m_cbvSrvUavHeap);

			//�T���v���[�p.
			D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
			samplerHeapDesc.NumDescriptors = m_samplerMax;        // �ő�512
			samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			ThrowIfFailed(m_device->CreateDescriptorHeap(&samplerHeapDesc, IID_PPV_ARGS(&m_samplerHeap)));
			NAME_D3D12_OBJECT(m_samplerHeap);

			//RTV�p�̃C���N�������g�T�C�Y
			m_rtvDescriptorIncrementSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			//DSV�p�̃C���N�������g�T�C�Y
			m_dsvDescriptorIncrementSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			//�T���v���[�p�̃C���N�������g�T�C�Y
			m_samplerDescriptorIncrementSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
			//CBV_SRV_UAV�p�̃C���N�������g�T�C�Y
			m_cbvSrvUavDescriptorIncrementSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		}
		//�R�}���h�A���P�[�^�[
		ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));

	}

	void BaseDevice::LoadAssets() {
		CreateRootSignature();
		CreateInitPipelineState();
		//�������p�R�}���h���X�g���쐬
		ThrowIfFailed(m_device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_commandAllocator.Get(),
			m_initPipelineState.Get(),
			IID_PPV_ARGS(&m_initCommandList))
		);
		NAME_D3D12_OBJECT(m_initCommandList);
		CreateRenderTargetViews();
		CreateDepthStencilView();
		CreateSamplers();
		auto pScene = App::GetBaseScene();
		pScene->OnInitScene();
		pScene->OnInit();
		//�������p�R�}���h���X�g�̃N���[�Y
		ThrowIfFailed(m_initCommandList->Close());
		ID3D12CommandList* ppCommandLists[] = { m_initCommandList.Get() };
		//�������p�R�}���h���X�g�̎��s
		m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		//�����������̏I��
		m_isInited = true;
//		m_process = process::update;
		//�t���[���̍쐬
		CreateBaseFrame();
		//�����I�u�W�F�N�g�̍쐬
		CreateSynchronizationObjects();
	}

	void BaseDevice::CreateRootSignature() {
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

		// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

		if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		CD3DX12_DESCRIPTOR_RANGE1 ranges[4]; // Perfomance TIP: Order from most frequent to least frequent.
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);    // 2 frequently changed diffuse + normal textures - using registers t1 and t2.
		ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);    // 1 frequently changed constant buffer.
		ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);                                                // 1 infrequently changed shadow texture - starting in register t0.
		ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 2, 0);                                            // 2 static samplers.

		CD3DX12_ROOT_PARAMETER1 rootParameters[4];
		rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[2].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[3].InitAsDescriptorTable(1, &ranges[3], D3D12_SHADER_VISIBILITY_PIXEL);

		SetGpuSlot(L"t0", 2);
		SetGpuSlot(L"t1", 0);
		SetGpuSlot(L"t2", 0);
		SetGpuSlot(L"s0", 3);
		SetGpuSlot(L"s1", 3);
		SetGpuSlot(L"b0", 1);


		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
		ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
		NAME_D3D12_OBJECT(m_rootSignature);
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

	void BaseDevice::CreateDepthStencilView() {
		CD3DX12_RESOURCE_DESC dsvTextureDesc(
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
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);

		D3D12_CLEAR_VALUE clearValue;    // Performance tip: Tell the runtime at resource creation the desired clear value.
		clearValue.Format = DXGI_FORMAT_D32_FLOAT;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;

		ThrowIfFailed(m_device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&dsvTextureDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(&m_depthStencil)));

		NAME_D3D12_OBJECT(m_depthStencil);

		// Create the depth stencil view.
		m_device->CreateDepthStencilView(m_depthStencil.Get(), nullptr, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

	}

	void BaseDevice::CreateSamplers() {
		//PointClamp
		// �V���h�E�}�b�v�p�|�C���g�T���v���[
		int index = (int)GetSamplerNextIndex();
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

		//LinearWrap
		// �ʏ�`�惊�j�A���b�v�T���v���[
		index = (int)GetSamplerNextIndex();
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

		//LinearClamp
		// �ʏ�`�惊�j�A�N�����v�T���v���[
		index = (int)GetSamplerNextIndex();
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
		//�e�`��p�R���p�[�W�������j�A

		index = (int)GetSamplerNextIndex();
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

	void BaseDevice::CreateInitPipelineState() {

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		ZeroMemory(&psoDesc, sizeof(psoDesc));
		psoDesc.InputLayout = { VertexPositionNormalTexture::GetVertexElement(), VertexPositionNormalTexture::GetNumElements() };
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
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_initPipelineState)));
		NAME_D3D12_OBJECT(m_initPipelineState);
		//�V���h�E�}�b�v�p
//		psoDesc.InputLayout = { VertexPosition::GetVertexElement(), VertexPosition::GetNumElements() };

		psoDesc.PS = CD3DX12_SHADER_BYTECODE(0, 0);
		psoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
		psoDesc.NumRenderTargets = 0;

		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineStateShadowMap)));
		NAME_D3D12_OBJECT(m_pipelineStateShadowMap);


	}

	void BaseDevice::CreateBaseFrame() {
		for (UINT i = 0; i < m_frameCount; i++)
		{
			m_baseFrame[i] = new BaseFrame(m_initPipelineState.Get(),&m_viewport,i);
			App::GetBaseScene()->OnInitFrame(m_baseFrame[i]);
			App::GetBaseScene()->WriteConstantBuffers(m_baseFrame[i]);
		}
		m_currentBaseFrameIndex = 0;
	}

	void BaseDevice::CreateSynchronizationObjects() {
		ThrowIfFailed(m_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
		m_fenceValue++;

		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_fenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}

		const UINT64 fenceToWaitFor = m_fenceValue;
		ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fenceToWaitFor));
		m_fenceValue++;
		if (m_fenceEvent != nullptr) {
			ThrowIfFailed(m_fence->SetEventOnCompletion(fenceToWaitFor, m_fenceEvent));
			WaitForSingleObject(m_fenceEvent, INFINITE);
		}
	}

	void BaseDevice::OnUpdateRender() {
		int retCode = 0;
		try {
			App::GetInputDevice().ResetControlerState();
			OnUpdate();
			OnRender();
		}
		catch (BaseException& e) {
			//�f�o�b�O�o�͂�����B
			string str = e.what_m() + "\n";
			OutputDebugStringA(str.c_str());
			retCode = 1;
		}
		catch (runtime_error& e) {
			//�f�o�b�O�o�͂�����B
			string str(e.what());
			str += "\n";
			OutputDebugStringA(str.c_str());
			retCode = 1;
		}
		catch (exception& e) {
			//STL�G���[
			string str(e.what());
			str += "\n";
			OutputDebugStringA(str.c_str());
			retCode = 1;
		}
		catch (...) {
			OutputDebugStringA("�����s���̃G���[\n");
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

		//�R�}���h���X�g�J�n�B
		ThrowIfFailed(GetCurrentBaseFrame()->m_updateCommandAllocator->Reset());
		ThrowIfFailed(GetCurrentBaseFrame()->m_updateCommandList->Reset(GetCurrentBaseFrame()->m_updateCommandAllocator.Get(), GetCurrentBaseFrame()->m_pipelineState.Get()));
		m_process = process::update;

		//�V�[����OnUpdate
		App::GetBaseScene()->OnUpdate();
		App::GetBaseScene()->WriteConstantBuffers(GetCurrentBaseFrame());
		ThrowIfFailed(GetCurrentBaseFrame()->m_updateCommandList->Close());

	}

	void BaseDevice::PopulateCommandList(BaseFrame* pBaseFrame) {
		m_process = process::begin;
		GetCurrentBaseFrame()->m_beginCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorIncrementSize);
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
		Col4 color = m_clearColor;
		const float clearColor[] = { color.x, color.y, color.z, color.w };
		GetCurrentBaseFrame()->m_beginCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		GetCurrentBaseFrame()->m_beginCommandList->ClearDepthStencilView(m_dsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		// Clear the depth stencil buffer in preparation for rendering the shadow map.
		GetCurrentBaseFrame()->m_beginCommandList->ClearDepthStencilView(GetCurrentBaseFrame()->m_shadowDepthView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		ThrowIfFailed(GetCurrentBaseFrame()->m_beginCommandList->Close());

		m_process = process::shadowmap;
		GetCurrentBaseFrame()->m_shadowCommandList->OMSetRenderTargets(0, nullptr, FALSE, 
			&GetCurrentBaseFrame()->m_shadowDepthView);    // No render target needed for the shadow pa
		GetCurrentBaseFrame()->m_shadowCommandList->SetGraphicsRootSignature(m_rootSignature.Get());
		ID3D12DescriptorHeap* ppShadowHeaps[] = { m_cbvSrvUavHeap.Get()};
		GetCurrentBaseFrame()->m_shadowCommandList->SetDescriptorHeaps(_countof(ppShadowHeaps), ppShadowHeaps);

		GetCurrentBaseFrame()->m_shadowCommandList->RSSetViewports(1, &m_viewport);
		GetCurrentBaseFrame()->m_shadowCommandList->RSSetScissorRects(1, &m_scissorRect);

		App::GetBaseScene()->PopulateShadowmapCommandList(pBaseFrame);

		ThrowIfFailed(GetCurrentBaseFrame()->m_shadowCommandList->Close());

		m_process = process::render;
		GetCurrentBaseFrame()->m_sceneCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
		GetCurrentBaseFrame()->m_sceneCommandList->SetGraphicsRootSignature(m_rootSignature.Get());
		ID3D12DescriptorHeap* ppHeaps[] = { m_cbvSrvUavHeap.Get(), m_samplerHeap.Get() };
		GetCurrentBaseFrame()->m_sceneCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		GetCurrentBaseFrame()->m_sceneCommandList->SetGraphicsRootDescriptorTable(GetGpuSlotID(L"t0"), GetCurrentBaseFrame()->m_shadowDepthHandle);        // Set the shadow texture as an SRV.


		GetCurrentBaseFrame()->m_sceneCommandList->RSSetViewports(1, &m_viewport);
		GetCurrentBaseFrame()->m_sceneCommandList->RSSetScissorRects(1, &m_scissorRect);

		App::GetBaseScene()->PopulateCommandList(pBaseFrame);

		ThrowIfFailed(GetCurrentBaseFrame()->m_sceneCommandList->Close());

	}

	void BaseDevice::OnRender() {

		ThrowIfFailed(GetCurrentBaseFrame()->m_beginCommandAllocator->Reset());
		ThrowIfFailed(GetCurrentBaseFrame()->m_beginCommandList->Reset(GetCurrentBaseFrame()->m_beginCommandAllocator.Get(), GetCurrentBaseFrame()->m_pipelineState.Get()));

		ThrowIfFailed(GetCurrentBaseFrame()->m_shadowCommandAllocator->Reset());
		ThrowIfFailed(GetCurrentBaseFrame()->m_shadowCommandList->Reset(GetCurrentBaseFrame()->m_shadowCommandAllocator.Get(), GetCurrentBaseFrame()->m_pipelineState.Get()));


		ThrowIfFailed(GetCurrentBaseFrame()->m_sceneCommandAllocator->Reset());
		ThrowIfFailed(GetCurrentBaseFrame()->m_sceneCommandList->Reset(GetCurrentBaseFrame()->m_sceneCommandAllocator.Get(), GetCurrentBaseFrame()->m_pipelineState.Get()));

		ThrowIfFailed(GetCurrentBaseFrame()->m_endCommandAllocator->Reset());
		ThrowIfFailed(GetCurrentBaseFrame()->m_endCommandList->Reset(GetCurrentBaseFrame()->m_endCommandAllocator.Get(), GetCurrentBaseFrame()->m_pipelineState.Get()));

		PopulateCommandList(GetCurrentBaseFrame());

		m_process = process::end;

		GetCurrentBaseFrame()->m_endCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
		ThrowIfFailed(GetCurrentBaseFrame()->m_endCommandList->Close());

		ID3D12CommandList* ppCommandLists[] = { 
			GetCurrentBaseFrame()->m_updateCommandList.Get(),
			GetCurrentBaseFrame()->m_beginCommandList.Get(),
			GetCurrentBaseFrame()->m_shadowCommandList.Get(),
			GetCurrentBaseFrame()->m_sceneCommandList.Get(),
			GetCurrentBaseFrame()->m_endCommandList.Get(),
		};
		m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		ThrowIfFailed(m_swapChain->Present(1, 0));
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		GetCurrentBaseFrame()->m_fenceValue = m_fenceValue;
		ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValue));
		m_fenceValue++;
	}
	void BaseDevice::OnDestroy() {
		//�V�[���̊J��
		App::GetBaseScene()->OnDestroy();
		// GPU���\�[�X�̊J��
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
			delete m_baseFrame[i];
		}

	}
	void BaseDevice::OnKeyDown(UINT8 key) {
		App::GetBaseScene()->OnKeyDown(key);

	}
	void BaseDevice::OnKeyUp(UINT8 key) {
		App::GetBaseScene()->OnKeyUp(key);

	}





}
// end namespace basecross