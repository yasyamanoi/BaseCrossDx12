/*!
@file SpStaticRender.cpp
@brief Simple�X�^�e�B�b�N�`��R���|�[�l���g�Q
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	Shadowmap�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------

	IMPLEMENT_DX12SHADER(PNTShadowmap, App::GetShadersDir() + L"VSShadowmap.cso")

	void Shadowmap::CreatePipelineStates() {
		auto pDevice = App::GetBaseDevice();
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		ZeroMemory(&psoDesc, sizeof(psoDesc));
		CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);

		psoDesc.InputLayout = { VertexPositionNormalTexture::GetVertexElement(), VertexPositionNormalTexture::GetNumElements() };
		psoDesc.pRootSignature = pDevice->GetRootSignature().Get();;
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(PNTShadowmap::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			PNTShadowmap::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(0, 0);
		psoDesc.RasterizerState = rasterizerStateDesc;
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 0;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
		psoDesc.SampleDesc.Count = 1;
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PNTPipelineState)));
		NAME_D3D12_OBJECT(m_PNTPipelineState);

	}

	//�R���X�^���g�o�b�t�@�̓o�^
	void Shadowmap::SetConstant(ShadowConstant& constant, const shared_ptr<Transform>& transform) {
		//���ׂ�0�ɏ�����
		constant = {};
		bsm::Mat4x4 world = transform->GetWorldMatrix();
		auto lights = GetGameObject()->GetActiveLightSet();
		auto light = lights->GetMainBaseLight();
		Vec3 lightDir = -1.0 * light.m_directional;
		auto camera = GetGameObject()->GetActiveCamera();
		Vec3 lightAt = camera->GetAt();
		Vec3 lightEye = lightAt + (lightDir * m_lightHeight);
		auto width = App::GetBaseDevice()->GetViewport().Width;
		auto height = App::GetBaseDevice()->GetViewport().Height;
		Mat4x4 lightView = XMMatrixLookAtLH(lightEye, lightAt, Vec3(0, 1.0f, 0));
//		Mat4x4 lightProj = XMMatrixOrthographicLH(width, height, m_lightNear, m_lightFar);
		Mat4x4 lightProj = XMMatrixOrthographicLH(m_viewWidth, m_viewHeight, m_lightNear, m_lightFar);
		constant.world = transpose(world);
		constant.view = transpose(lightView);
		constant.projection = transpose(lightProj);
	}

	void Shadowmap::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();
		auto pCommandList = pDevice->GetComandList().Get();
		pCommandList->SetPipelineState(m_PNTPipelineState.Get());
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		auto paramIndex = GetFrameParamIndex();
		//Cbv
		CD3DX12_GPU_DESCRIPTOR_HANDLE cbvGpuHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_frameParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), cbvGpuHandle);
		if (!m_baseMesh) {
			throw BaseException(
				L"���b�V��������ł��܂���B",
				L"ShadowmapRender::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);

	}

	void Shadowmap::OnInitFrame(BaseFrame* pBaseFrame) {
		auto pDefaultDev = App::GetBaseDevice();
		auto pDevice = App::GetID3D12Device();

		// �R���X�^���g�o�b�t�@�̃T�C�Y��256�o�C�g���E���Ƃɍ쐬����
		UINT constsize = (sizeof(ShadowConstant) + 255) & ~255;

		FrameParam param;
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
		m_frameParamIndex = pBaseFrame->m_frameParamVec.size();
		pBaseFrame->m_frameParamVec.push_back(param);


	}

	void Shadowmap::WriteConstantBuffers(BaseFrame* pBaseFrame) {
		ShadowConstant constants;
		SetConstant(constants, GetGameObject()->GetComponent<Transform>());
		memcpy(pBaseFrame->m_frameParamVec[m_frameParamIndex].m_pConstantBuffer, &constants, sizeof(ShadowConstant));

	}

	Shadowmap::Shadowmap(const shared_ptr<GameObject>& gameObjectPtr):
		Component(gameObjectPtr)
	{
		SetRenderActive(true);
	}

	void Shadowmap::OnInit() {
		CreatePipelineStates();
	}


	//--------------------------------------------------------------------------------------
	///	SpStatic�`��R���|�[�l���g(�e)
	//--------------------------------------------------------------------------------------

	void SpStaticRender::SetConstant(SimpleConstant& constant, const shared_ptr<Transform>& transform) {
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
	}

	void SpStaticRender::OnInitFrame(BaseFrame* pBaseFrame) {
		auto pDefaultDev = App::GetBaseDevice();
		auto pDevice = App::GetID3D12Device();

		// �R���X�^���g�o�b�t�@�̃T�C�Y��256�o�C�g���E���Ƃɍ쐬����
		UINT constsize = (sizeof(SimpleConstant) + 255) & ~255;

		FrameParam param;
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
		m_frameParamIndex = pBaseFrame->m_frameParamVec.size();
		pBaseFrame->m_frameParamVec.push_back(param);

	}

	void SpStaticRender::WriteConstantBuffers(BaseFrame* pBaseFrame) {
		SimpleConstant constants;
		SetConstant(constants, GetGameObject()->GetComponent<Transform>());
		memcpy(pBaseFrame->m_frameParamVec[m_frameParamIndex].m_pConstantBuffer, &constants, sizeof(SimpleConstant));
	}


	//--------------------------------------------------------------------------------------
	///	SpPCStatic�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------

	IMPLEMENT_DX12SHADER(SpVSPCStatic, App::GetShadersDir() + L"SpVSPCStatic.cso")
	IMPLEMENT_DX12SHADER(SpPSPCStatic, App::GetShadersDir() + L"SpPSPCStatic.cso")

	SpPCStaticRender::SpPCStaticRender(const shared_ptr<GameObject>& gameObjectPtr) :
		SpStaticRender(gameObjectPtr)
	{
		SetRenderActive(true);
	}

	SpPCStaticRender::SpPCStaticRender(const shared_ptr<GameObject>& gameObjectPtr,
		vector<VertexPositionColor>& vertices, vector<uint32_t>& indices) :
		SpStaticRender(gameObjectPtr)
	{
		try {
			SetRenderActive(true);
			auto pDevice = App::GetBaseDevice();
			auto commandList = pDevice->GetComandList();
			m_baseMesh = BaseMesh::CreateBaseMesh<VertexPositionColor>(commandList, vertices, indices);
		}
		catch (...) {
			throw;
		}
	}



	void SpPCStaticRender::CreatePipelineStates() {
		auto pDevice = App::GetBaseDevice();
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
		psoDesc.BlendState = BaseRenderState::GetOpaqueBlend();
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_defaultPipelineState)));
		NAME_D3D12_OBJECT(m_defaultPipelineState);

		psoDesc.BlendState = BaseRenderState::GetAlphaBlend();
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_alphaPipelineState)));
		NAME_D3D12_OBJECT(m_alphaPipelineState);
	}


	void SpPCStaticRender::OnInit() {
		CreatePipelineStates();
	}


	void SpPCStaticRender::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();
		auto pCommandList = pDevice->GetComandList().Get();

		if (GetGameObject()->IsAlphaActive()) {
			pCommandList->SetPipelineState(m_alphaPipelineState.Get());
		}
		else {
			pCommandList->SetPipelineState(m_defaultPipelineState.Get());
		}

		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		auto paramIndex = GetFrameParamIndex();
		//Cbv
		CD3DX12_GPU_DESCRIPTOR_HANDLE cbvGpuHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_frameParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), cbvGpuHandle);

		if (!m_baseMesh) {
			throw BaseException(
				L"���b�V��������ł��܂���B",
				L"SpPCStaticRender::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);
	}


	//--------------------------------------------------------------------------------------
	///	SpPTStatic�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------

	IMPLEMENT_DX12SHADER(SpVSPTStatic, App::GetShadersDir() + L"SpVSPTStatic.cso")
	IMPLEMENT_DX12SHADER(SpPSPTStatic, App::GetShadersDir() + L"SpPSPTStatic.cso")

	SpPTStaticRender::SpPTStaticRender(const shared_ptr<GameObject>& gameObjectPtr) :
		SpStaticRender(gameObjectPtr)
	{
		SetRenderActive(true);
	}

	SpPTStaticRender::SpPTStaticRender(const shared_ptr<GameObject>& gameObjectPtr,
		vector<VertexPositionTexture>& vertices, vector<uint32_t>& indices) :
		SpStaticRender(gameObjectPtr)
	{
		try {
			SetRenderActive(true);
			auto pDevice = App::GetBaseDevice();
			auto commandList = pDevice->GetComandList();
			m_baseMesh = BaseMesh::CreateBaseMesh<VertexPositionTexture>(commandList, vertices, indices);
		}
		catch (...) {
			throw;
		}
	}



	void SpPTStaticRender::CreatePipelineStates() {
		auto pDevice = App::GetBaseDevice();
		CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
		//�J�����O
		rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		ZeroMemory(&psoDesc, sizeof(psoDesc));
		psoDesc.InputLayout = { VertexPositionTexture::GetVertexElement(), VertexPositionTexture::GetNumElements() };
		psoDesc.pRootSignature = pDevice->GetRootSignature().Get();
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(SpVSPTStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpVSPTStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<UINT8*>(SpPSPTStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpPSPTStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.RasterizerState = rasterizerStateDesc;
		psoDesc.BlendState = BaseRenderState::GetOpaqueBlend();
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_defaultPipelineState)));
		NAME_D3D12_OBJECT(m_defaultPipelineState);

		psoDesc.BlendState = BaseRenderState::GetAlphaBlend();
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_alphaPipelineState)));
		NAME_D3D12_OBJECT(m_alphaPipelineState);
	}


	void SpPTStaticRender::OnInit() {
		CreatePipelineStates();
	}


	void SpPTStaticRender::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();
		auto pCommandList = pDevice->GetComandList().Get();

		if (GetGameObject()->IsAlphaActive()) {
			pCommandList->SetPipelineState(m_alphaPipelineState.Get());
		}
		else {
			pCommandList->SetPipelineState(m_defaultPipelineState.Get());
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
			UINT index = pDevice->GetSamplerIndex(GetSamplerKey());
			if (index == UINT_MAX) {
				throw BaseException(
					L"�T���v���[������ł��܂���B",
					L"SpPTStaticRender::PopulateCommandList()"
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
		CD3DX12_GPU_DESCRIPTOR_HANDLE skyCbvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_frameParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), skyCbvHandle);

		if (!m_baseMesh) {
			throw BaseException(
				L"���b�V��������ł��܂���B",
				L"SpPTStaticRender::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);
	}


	//--------------------------------------------------------------------------------------
	///	SpPNStatic�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------

	IMPLEMENT_DX12SHADER(SpVSPNStatic, App::GetShadersDir() + L"SpVSPNStatic.cso")
	IMPLEMENT_DX12SHADER(SpPSPNStatic, App::GetShadersDir() + L"SpPSPNStatic.cso")

	SpPNStaticRender::SpPNStaticRender(const shared_ptr<GameObject>& gameObjectPtr) :
		SpStaticRender(gameObjectPtr)
	{
		SetRenderActive(true);
	}

	SpPNStaticRender::SpPNStaticRender(const shared_ptr<GameObject>& gameObjectPtr,
		vector<VertexPositionNormal>& vertices, vector<uint32_t>& indices) :
		SpStaticRender(gameObjectPtr)
	{
		try {
			SetRenderActive(true);
			auto pDevice = App::GetBaseDevice();
			auto commandList = pDevice->GetComandList();
			m_baseMesh = BaseMesh::CreateBaseMesh<VertexPositionNormal>(commandList, vertices, indices);
		}
		catch (...) {
			throw;
		}
	}



	void SpPNStaticRender::CreatePipelineStates() {
		auto pDevice = App::GetBaseDevice();
		CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
		//�J�����O
		rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		ZeroMemory(&psoDesc, sizeof(psoDesc));
		psoDesc.InputLayout = { VertexPositionNormal::GetVertexElement(), VertexPositionNormal::GetNumElements() };
		psoDesc.pRootSignature = pDevice->GetRootSignature().Get();
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(SpVSPNStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpVSPNStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<UINT8*>(SpPSPNStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpPSPNStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.RasterizerState = rasterizerStateDesc;
		psoDesc.BlendState = BaseRenderState::GetOpaqueBlend();
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_defaultPipelineState)));
		NAME_D3D12_OBJECT(m_defaultPipelineState);

		psoDesc.BlendState = BaseRenderState::GetAlphaBlend();
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_alphaPipelineState)));
		NAME_D3D12_OBJECT(m_alphaPipelineState);
	}


	void SpPNStaticRender::OnInit() {
		CreatePipelineStates();
	}


	void SpPNStaticRender::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();
		auto pCommandList = pDevice->GetComandList().Get();

		if (GetGameObject()->IsAlphaActive()) {
			pCommandList->SetPipelineState(m_alphaPipelineState.Get());
		}
		else {
			pCommandList->SetPipelineState(m_defaultPipelineState.Get());
		}
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//Cbv
		auto paramIndex = GetFrameParamIndex();
		CD3DX12_GPU_DESCRIPTOR_HANDLE skyCbvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_frameParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), skyCbvHandle);

		if (!m_baseMesh) {
			throw BaseException(
				L"���b�V��������ł��܂���B",
				L"SpPNStaticRender::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);
	}

	//--------------------------------------------------------------------------------------
	///	SpPCTStatic�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------

	IMPLEMENT_DX12SHADER(SpVSPCTStatic, App::GetShadersDir() + L"SpVSPCTStatic.cso")
	IMPLEMENT_DX12SHADER(SpPSPCTStatic, App::GetShadersDir() + L"SpPSPCTStatic.cso")

	SpPCTStaticRender::SpPCTStaticRender(const shared_ptr<GameObject>& gameObjectPtr) :
		SpStaticRender(gameObjectPtr)
	{
		SetRenderActive(true);
	}

	SpPCTStaticRender::SpPCTStaticRender(const shared_ptr<GameObject>& gameObjectPtr,
		vector<VertexPositionColorTexture>& vertices, vector<uint32_t>& indices) :
		SpStaticRender(gameObjectPtr)
	{
		try {
			SetRenderActive(true);
			auto pDevice = App::GetBaseDevice();
			auto commandList = pDevice->GetComandList();
			m_baseMesh = BaseMesh::CreateBaseMesh<VertexPositionColorTexture>(commandList, vertices, indices);
		}
		catch (...) {
			throw;
		}
	}



	void SpPCTStaticRender::CreatePipelineStates() {
		auto pDevice = App::GetBaseDevice();
		CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
		//�J�����O
		rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		ZeroMemory(&psoDesc, sizeof(psoDesc));
		psoDesc.InputLayout = { VertexPositionColorTexture::GetVertexElement(), VertexPositionColorTexture::GetNumElements() };
		psoDesc.pRootSignature = pDevice->GetRootSignature().Get();
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(SpVSPCTStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpVSPCTStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<UINT8*>(SpPSPCTStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			SpPSPCTStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.RasterizerState = rasterizerStateDesc;
		psoDesc.BlendState = BaseRenderState::GetOpaqueBlend();
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_defaultPipelineState)));
		NAME_D3D12_OBJECT(m_defaultPipelineState);

		psoDesc.BlendState = BaseRenderState::GetAlphaBlend();
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_alphaPipelineState)));
		NAME_D3D12_OBJECT(m_alphaPipelineState);
	}


	void SpPCTStaticRender::OnInit() {
		CreatePipelineStates();
	}


	void SpPCTStaticRender::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();
		auto pCommandList = pDevice->GetComandList().Get();

		if (GetGameObject()->IsAlphaActive()) {
			pCommandList->SetPipelineState(m_alphaPipelineState.Get());
		}
		else {
			pCommandList->SetPipelineState(m_defaultPipelineState.Get());
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
			UINT index = pDevice->GetSamplerIndex(GetSamplerKey());
			if (index == UINT_MAX) {
				throw BaseException(
					L"�T���v���[������ł��܂���B",
					L"SpPCTStaticRender::PopulateCommandList()"
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
		CD3DX12_GPU_DESCRIPTOR_HANDLE skyCbvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_frameParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), skyCbvHandle);

		if (!m_baseMesh) {
			throw BaseException(
				L"���b�V��������ł��܂���B",
				L"SpPCTStaticRender::PopulateCommandList()"
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

	SpPNTStaticRender::SpPNTStaticRender(const shared_ptr<GameObject>& gameObjectPtr) :
		SpStaticRender(gameObjectPtr)
	{
		SetRenderActive(true);
	}

	SpPNTStaticRender::SpPNTStaticRender(const shared_ptr<GameObject>& gameObjectPtr,
		vector<VertexPositionNormalTexture>& vertices, vector<uint32_t>& indices) :
		SpStaticRender(gameObjectPtr)
	{
		try {
			SetRenderActive(true);
			auto pDevice = App::GetBaseDevice();
			auto commandList = pDevice->GetComandList();
			m_baseMesh = BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(commandList, vertices, indices);
		}
		catch (...) {
			throw;
		}
	}



	void SpPNTStaticRender::CreatePipelineStates() {
		auto pDevice = App::GetBaseDevice();
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
		psoDesc.BlendState = BaseRenderState::GetOpaqueBlend();
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		//�f�t�H���g�e����
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_defaultPipelineState)));
		NAME_D3D12_OBJECT(m_defaultPipelineState);
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
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_defaultShadowPipelineState)));
		NAME_D3D12_OBJECT(m_defaultShadowPipelineState);
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
		psoDesc.BlendState = BaseRenderState::GetAlphaBlend();
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_alphaPipelineState)));
		NAME_D3D12_OBJECT(m_alphaPipelineState);
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
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_alphaShadowPipelineState)));
		NAME_D3D12_OBJECT(m_alphaShadowPipelineState);
	}


	void SpPNTStaticRender::OnInit() {
		CreatePipelineStates();
	}


	void SpPNTStaticRender::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto pDevice = App::GetBaseDevice();
		auto pCommandList = pDevice->GetComandList().Get();
		if (GetGameObject()->IsAlphaActive()) {
			if (IsOwnShadowActive()) {
				pCommandList->SetPipelineState(m_alphaShadowPipelineState.Get());
			}
			else {
				pCommandList->SetPipelineState(m_alphaPipelineState.Get());
			}
		}
		else {
			if (IsOwnShadowActive()) {
				pCommandList->SetPipelineState(m_defaultShadowPipelineState.Get());
			}
			else {
				pCommandList->SetPipelineState(m_defaultPipelineState.Get());
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
					L"SpPNTStaticRender::PopulateCommandList()"
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
					L"SpPNTStaticRender::PopulateCommandList()"
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
		auto paramIndex = GetFrameParamIndex();
		CD3DX12_GPU_DESCRIPTOR_HANDLE skyCbvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pBaseFrame->m_frameParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), skyCbvHandle);

		if (!m_baseMesh) {
			throw BaseException(
				L"���b�V��������ł��܂���B",
				L"SpPNTStaticRender::PopulateCommandList()"
			);
		}
		pCommandList->IASetVertexBuffers(0, 1, &m_baseMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_baseMesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(m_baseMesh->GetNumIndices(), 1, 0, 0, 0);
	}




}
//end namespace basecross