/*!
@file SpStaticDraw.cpp
@brief Simple�X�^�e�B�b�N�`��R���|�[�l���g�Q
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	SpStatic�`��R���|�[�l���g(�e)
	//--------------------------------------------------------------------------------------

	void SpStaticDraw::SetConstant(SimpleConstant& constant, const shared_ptr<Transform>& transform) {
		//���ׂ�0�ɏ�����
		constant = {};
		Mat4x4 world = transform->GetWorldMatrix();
		//�]�u����
		world.transpose();
		auto cameraPtr = GetGameObject()->GetActiveCamera();
		//�r���[�Ǝˉe�s��𓾂�
		Mat4x4 viewMat = cameraPtr->GetViewMatrix();
		//�]�u����
		viewMat.transpose();
		Mat4x4 projMat = cameraPtr->GetProjMatrix();
		//�]�u����
		projMat.transpose();
		constant.world = world;
		constant.view = viewMat;
		constant.projection = projMat;
		//�G�~�b�V�u
		constant.emissive = GetEmissive();
		//�f�t�B�[�Y
		constant.diffuse = GetDiffuse();
		//�X�y�L�����[
		constant.specular = GetSpecular();
		//���C�e�B���O
		auto activeLights = GetGameObject()->GetActiveLightSet();
		auto index = activeLights->GetMainIndex();
		auto light = activeLights->GetLight(index);
		constant.lightDir = light.m_directional;
		constant.lightDir.w = 1.0f;
		constant.eyePos = cameraPtr->GetEye();
		constant.eyePos.w = 1.0f;
		//�e�N�X�`�����������ꍇ
		if (GetTexture()) {
			constant.activeFlg.x = 1;
		}
		//�e�p
		if (IsOwnShadowActive()) {
			bsm::Vec3 CalcLightDir = -1.0 * light.m_directional;
			bsm::Vec3 LightAt = cameraPtr->GetAt();
			bsm::Vec3 LightEye = CalcLightDir;
			LightEye *= Shadowmap::GetLightHeight();
			LightEye = LightAt + LightEye;
			constant.lightPos = LightEye;
			constant.lightPos.w = 1.0f;
			bsm::Mat4x4 lightView, lightProj;
			//���C�g�̃r���[�Ǝˉe���v�Z
			lightView = XMMatrixLookAtLH(LightEye, LightAt, bsm::Vec3(0, 1.0f, 0));
			lightProj = XMMatrixOrthographicLH(Shadowmap::GetViewWidth(), Shadowmap::GetViewHeight(),
				Shadowmap::GetLightNear(), Shadowmap::GetLightFar());
			constant.lightView = bsm::transpose(lightView);
			constant.lightProjection = bsm::transpose(lightProj);
		}
	}

	void SpStaticDraw::OnInitFrame(BaseFrame* pBaseFrame) {
		auto pDefaultDev = App::GetBaseDevice();
		auto pDevice = App::GetID3D12Device();

		// �R���X�^���g�o�b�t�@�̃T�C�Y��256�o�C�g���E���Ƃɍ쐬����
		UINT constsize = (sizeof(SimpleConstant) + 255) & ~255;

		ConstBuffParam param;
		// �A�b�v���[�h�q�[�v
		ThrowIfFailed(pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(constsize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&param.m_cbvUploadHeap)));
		//�}�b�v
		CD3DX12_RANGE readRange(0, 0);
		ThrowIfFailed(param.m_cbvUploadHeap->Map(0, &readRange, reinterpret_cast<void**>(&param.m_pConstantBuffer)));
		//�n���h���̍쐬
		param.m_constBuffIndex = pDefaultDev->GetCbvUavNextIndex();
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle(
			pDefaultDev->GetCbvSrvUavDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
			param.m_constBuffIndex,
			pDefaultDev->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		//�R���X�^���g�o�b�t�@�r���[�̍쐬
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = param.m_cbvUploadHeap->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = constsize;
		pDevice->CreateConstantBufferView(&cbvDesc, handle);
		m_constBuffParamIndex = pBaseFrame->m_constBuffParamVec.size();
		pBaseFrame->m_constBuffParamVec.push_back(param);

	}

	void SpStaticDraw::WriteConstantBuffers(BaseFrame* pBaseFrame) {
		SimpleConstant constants;
		SetConstant(constants, GetGameObject()->GetComponent<Transform>());
		memcpy(pBaseFrame->m_constBuffParamVec[m_constBuffParamIndex].m_pConstantBuffer, &constants, sizeof(SimpleConstant));
	}


	//--------------------------------------------------------------------------------------
	///	SpPCStatic�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------

	IMPLEMENT_DX12SHADER(SpVSPCStatic, App::GetShadersDir() + L"SpVSPCStatic.cso")
	IMPLEMENT_DX12SHADER(SpPSPCStatic, App::GetShadersDir() + L"SpPSPCStatic.cso")

	SpPCStaticDraw::SpPCStaticDraw(const shared_ptr<GameObject>& gameObjectPtr) :
		SpStaticDraw(gameObjectPtr)
	{
		SetDrawActive(true);
	}

	SpPCStaticDraw::SpPCStaticDraw(const shared_ptr<GameObject>& gameObjectPtr,
		vector<VertexPositionColor>& vertices, vector<uint32_t>& indices) :
		SpStaticDraw(gameObjectPtr)
	{
		try {
			SetDrawActive(true);
			auto pDevice = App::GetBaseDevice();
			auto commandList = pDevice->GetComandList();
			m_baseMesh = BaseMesh::CreateBaseMesh<VertexPositionColor>(commandList, vertices, indices);
		}
		catch (...) {
			throw;
		}
	}



	void SpPCStaticDraw::CreatePipelineStates() {
		auto pDevice = App::GetBaseDevice();

		ComPtr<ID3D12PipelineState> defaultPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPCStaticDefault");
		ComPtr<ID3D12PipelineState> alphaPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPCStaticAlpha");


		CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
		//�J�����O
		rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		ZeroMemory(&psoDesc, sizeof(psoDesc));
		psoDesc.InputLayout = { VertexPositionColor::GetVertexElement(), VertexPositionColor::GetNumElements() };
		psoDesc.pRootSignature = pDevice->GetRootSignature().Get();
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(SpVSPCStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpVSPCStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<UINT8*>(SpPSPCStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpPSPCStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.RasterizerState = rasterizerStateDesc;
		psoDesc.BlendState = BaseBlendState::GetOpaqueBlend();
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		if (!defaultPipelineState) {
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&defaultPipelineState)));
			NAME_D3D12_OBJECT(defaultPipelineState);
			BasicPipelineStatePool::AddPipelineState(L"SpPCStaticDefault", defaultPipelineState);
		}
		psoDesc.BlendState = BaseBlendState::GetAlphaBlend();
		if (!alphaPipelineState) {
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&alphaPipelineState)));
			NAME_D3D12_OBJECT(alphaPipelineState);
			BasicPipelineStatePool::AddPipelineState(L"SpPCStaticAlpha", alphaPipelineState);

		}

	}


	void SpPCStaticDraw::OnCreate() {
		CreatePipelineStates();
	}


	void SpPCStaticDraw::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();

		ComPtr<ID3D12PipelineState> defaultPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPCStaticDefault",true);
		ComPtr<ID3D12PipelineState> alphaPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPCStaticAlpha",true);

		auto pCommandList = pDevice->GetComandList().Get();

		if (GetGameObject()->IsAlphaActive()) {
			pCommandList->SetPipelineState(alphaPipelineState.Get());
		}
		else {
			pCommandList->SetPipelineState(defaultPipelineState.Get());
		}

		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		auto paramIndex = GetConstBuffParamIndex();
		//Cbv
		CD3DX12_GPU_DESCRIPTOR_HANDLE cbvGpuHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_constBuffParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), cbvGpuHandle);

		if (!m_baseMesh) {
			throw BaseException(
				L"���b�V��������ł��܂���B",
				L"SpPCStaticDraw::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);
	}


	//--------------------------------------------------------------------------------------
	///	SpWireframeDraw�`��R���|�[�l���g(���C���t���[��)
	//--------------------------------------------------------------------------------------
	SpWireframeDraw::SpWireframeDraw(const shared_ptr<GameObject>& gameObjectPtr) :
		SpStaticDraw(gameObjectPtr)
	{
		SetDrawActive(true);
	}

	SpWireframeDraw::SpWireframeDraw(const shared_ptr<GameObject>& gameObjectPtr,
		vector<VertexPositionColor>& vertices, vector<uint32_t>& indices) :
		SpStaticDraw(gameObjectPtr)
	{
		try {
			SetDrawActive(true);
			auto pDevice = App::GetBaseDevice();
			auto commandList = pDevice->GetComandList();
			m_baseMesh = BaseMesh::CreateBaseMesh<VertexPositionColor>(commandList, vertices, indices);
		}
		catch (...) {
			throw;
		}
	}



	void SpWireframeDraw::CreatePipelineStates() {
		auto pDevice = App::GetBaseDevice();

		ComPtr<ID3D12PipelineState> defaultPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpWireframe");


		const D3D12_RASTERIZER_DESC rasterizerStateDesc =
		{
			D3D12_FILL_MODE_WIREFRAME,
			D3D12_CULL_MODE_NONE,
			FALSE, // FrontCounterClockwise
			D3D12_DEFAULT_DEPTH_BIAS,
			D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
			D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
			TRUE, // DepthClipEnable
			TRUE, // MultisampleEnable
			FALSE, // AntialiasedLineEnable
			0, // ForcedSampleCount
			D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		ZeroMemory(&psoDesc, sizeof(psoDesc));
		psoDesc.InputLayout = { VertexPositionColor::GetVertexElement(), VertexPositionColor::GetNumElements() };
		psoDesc.pRootSignature = pDevice->GetRootSignature().Get();
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(SpVSPCStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpVSPCStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<UINT8*>(SpPSPCStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpPSPCStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.RasterizerState = rasterizerStateDesc;
		psoDesc.BlendState = BaseBlendState::GetOpaqueBlend();
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		if (!defaultPipelineState) {
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&defaultPipelineState)));
			NAME_D3D12_OBJECT(defaultPipelineState);
			BasicPipelineStatePool::AddPipelineState(L"SpWireframe", defaultPipelineState);

		}
	}


	void SpWireframeDraw::OnCreate() {
		CreatePipelineStates();
	}


	void SpWireframeDraw::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();

		ComPtr<ID3D12PipelineState> defaultPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpWireframe",true);

		auto pCommandList = pDevice->GetComandList().Get();

		pCommandList->SetPipelineState(defaultPipelineState.Get());

		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		auto paramIndex = GetConstBuffParamIndex();
		//Cbv
		CD3DX12_GPU_DESCRIPTOR_HANDLE cbvGpuHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_constBuffParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), cbvGpuHandle);

		if (!m_baseMesh) {
			throw BaseException(
				L"���b�V��������ł��܂���B",
				L"SpWireframeDraw::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);
	}


	//--------------------------------------------------------------------------------------
	///	SpPNTStatic�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------

	IMPLEMENT_DX12SHADER(SpVSPNTStatic, App::GetShadersDir() + L"SpVSPNTStatic.cso")
	IMPLEMENT_DX12SHADER(SpVSPNTStaticShadow, App::GetShadersDir() + L"SpVSPNTStaticShadow.cso")

	IMPLEMENT_DX12SHADER(SpPSPNTStatic, App::GetShadersDir() + L"SpPSPNTStatic.cso")
	IMPLEMENT_DX12SHADER(SpPSPNTStaticShadow, App::GetShadersDir() + L"SpPSPNTStaticShadow.cso")

	SpPNTStaticDraw::SpPNTStaticDraw(const shared_ptr<GameObject>& gameObjectPtr) :
		SpStaticDraw(gameObjectPtr)
	{
		SetDrawActive(true);
	}

	SpPNTStaticDraw::SpPNTStaticDraw(const shared_ptr<GameObject>& gameObjectPtr,
		vector<VertexPositionNormalTexture>& vertices, vector<uint32_t>& indices) :
		SpStaticDraw(gameObjectPtr)
	{
		try {
			SetDrawActive(true);
			auto pDevice = App::GetBaseDevice();
			auto commandList = pDevice->GetComandList();
			m_baseMesh = BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(commandList, vertices, indices);
		}
		catch (...) {
			throw;
		}
	}



	void SpPNTStaticDraw::CreatePipelineStates() {
		auto pDevice = App::GetBaseDevice();

		ComPtr<ID3D12PipelineState> defaultPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPNTStaticDefault");
		ComPtr<ID3D12PipelineState> defaultShadowPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPNTStaticDefaultShadow");
		ComPtr<ID3D12PipelineState> alphaPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPNTStaticAlpha");
		ComPtr<ID3D12PipelineState> alphaShadowPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPNTStaticAlphaShadow");


		CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
		//�J�����O
		rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		ZeroMemory(&psoDesc, sizeof(psoDesc));
		psoDesc.InputLayout = { VertexPositionNormalTexture::GetVertexElement(), VertexPositionNormalTexture::GetNumElements() };
		psoDesc.pRootSignature = pDevice->GetRootSignature().Get();
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(SpVSPNTStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpVSPNTStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<UINT8*>(SpPSPNTStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpPSPNTStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.RasterizerState = rasterizerStateDesc;
		psoDesc.BlendState = BaseBlendState::GetOpaqueBlend();
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		//�f�t�H���g�e����
		if (!defaultPipelineState) {
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&defaultPipelineState)));
			NAME_D3D12_OBJECT(defaultPipelineState);
			BasicPipelineStatePool::AddPipelineState(L"SpPNTStaticDefault", defaultPipelineState);

		}
		//�f�t�H���g�e����
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(SpVSPNTStaticShadow::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpVSPNTStaticShadow::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<UINT8*>(SpPSPNTStaticShadow::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpPSPNTStaticShadow::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		if (!defaultShadowPipelineState) {
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&defaultShadowPipelineState)));
			NAME_D3D12_OBJECT(defaultShadowPipelineState);
			BasicPipelineStatePool::AddPipelineState(L"SpPNTStaticDefaultShadow", defaultShadowPipelineState);

		}
		//�A���t�@�e����
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(SpVSPNTStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpVSPNTStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<UINT8*>(SpPSPNTStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpPSPNTStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.BlendState = BaseBlendState::GetAlphaBlend();
		if (!alphaPipelineState) {
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&alphaPipelineState)));
			NAME_D3D12_OBJECT(alphaPipelineState);
			BasicPipelineStatePool::AddPipelineState(L"SpPNTStaticAlpha", alphaPipelineState);

		}
		//�A���t�@�e����
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(SpVSPNTStaticShadow::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpVSPNTStaticShadow::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<UINT8*>(SpPSPNTStaticShadow::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpPSPNTStaticShadow::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		if (!alphaShadowPipelineState) {
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&alphaShadowPipelineState)));
			NAME_D3D12_OBJECT(alphaShadowPipelineState);
			BasicPipelineStatePool::AddPipelineState(L"SpPNTStaticAlphaShadow", alphaPipelineState);
		}
	}


	void SpPNTStaticDraw::OnCreate() {
		CreatePipelineStates();
	}


	void SpPNTStaticDraw::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();

		ComPtr<ID3D12PipelineState> defaultPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPNTStaticDefault",true);
		ComPtr<ID3D12PipelineState> defaultShadowPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPNTStaticDefaultShadow", true);
		ComPtr<ID3D12PipelineState> alphaPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPNTStaticAlpha", true);
		ComPtr<ID3D12PipelineState> alphaShadowPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"SpPNTStaticAlphaShadow", true);

		auto pCommandList = pDevice->GetComandList().Get();
		if (GetGameObject()->IsAlphaActive()) {
			if (IsOwnShadowActive()) {
				pCommandList->SetPipelineState(alphaShadowPipelineState.Get());
				pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"t0"), pBaseFrame->m_shadowDepthHandle);        // Set the shadow texture as an SRV.
			}
			else {
				pCommandList->SetPipelineState(alphaPipelineState.Get());
				pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"t0"), pBaseFrame->m_nullSrvHandle0);        // Set the shadow texture as an SRV.
			}
		}
		else {
			if (IsOwnShadowActive()) {
				pCommandList->SetPipelineState(defaultShadowPipelineState.Get());
				pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"t0"), pBaseFrame->m_shadowDepthHandle);        // Set the shadow texture as an SRV.
			}
			else {
				pCommandList->SetPipelineState(defaultPipelineState.Get());
				pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"t0"), pBaseFrame->m_nullSrvHandle0);        // Set the shadow texture as an SRV.
			}

		}
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//Srv
		if (m_baseTexture) {
			CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(
				pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				m_baseTexture->GetSrvIndex(),
				pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
			);
			pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"t1"), srvHandle);
			//Sampler
			UINT index = pDevice->GetSamplerIndex(L"LinearClamp");
			if (index == UINT_MAX) {
				throw BaseException(
					L"LinearClamp�T���v���[������ł��܂���B",
					L"SpPNTStaticDraw::PopulateCommandList()"
				);
			}
			CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle(
				pDevice->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				index,
				pDevice->GetSamplerDescriptorHandleIncrementSize()
			);
			pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"s0"), samplerHandle);

			index = pDevice->GetSamplerIndex(L"ComparisonLinear");
			if (index == UINT_MAX) {
				throw BaseException(
					L"ComparisonLinear�T���v���[������ł��܂���B",
					L"SpPNTStaticDraw::PopulateCommandList()"
				);
			}
			CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle2(
				pDevice->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				index,
				pDevice->GetSamplerDescriptorHandleIncrementSize()
			);
			pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"s1"), samplerHandle2);


		}
		//Cbv
		auto paramIndex = GetConstBuffParamIndex();
		CD3DX12_GPU_DESCRIPTOR_HANDLE skyCbvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_constBuffParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), skyCbvHandle);

		if (!m_baseMesh) {
			throw BaseException(
				L"���b�V��������ł��܂���B",
				L"SpPNTStaticDraw::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);
	}





}
// end namespace basecross