/*!
@file ShadowmapDraw.cpp
@brief �V���h�E�}�b�v�`��R���|�[�l���g
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	Shadowmap�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------

	IMPLEMENT_DX12SHADER(PNTShadowmap, App::GetShadersDir() + L"VSShadowmap.cso")

	float Shadowmap::m_lightHeight(200.0f);
	float Shadowmap::m_lightNear(1.0f);
	float Shadowmap::m_lightFar(220.0f);
	float Shadowmap::m_viewWidth(32.0f);
	float Shadowmap::m_viewHeight(32.0f);
	float Shadowmap::m_posAdjustment(0.1f);


	void Shadowmap::CreatePipelineStates() {
		auto pDevice = App::GetBaseDevice();

		ComPtr<ID3D12PipelineState> PNTShadowmapPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"PNTShadowmap");

		// �V���h�E�}�b�v�p
		CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		depthStencilDesc.StencilEnable = FALSE;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { VertexPositionNormalTexture::GetVertexElement(), VertexPositionNormalTexture::GetNumElements() };;
		psoDesc.pRootSignature = pDevice->GetRootSignature().Get();
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(PNTShadowmap::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			PNTShadowmap::GetPtr()->GetShaderComPtr()->GetBufferSize()

		};
		psoDesc.PS =
		{
			CD3DX12_SHADER_BYTECODE(0, 0)
		};
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState = depthStencilDesc;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 0;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleDesc.Count = 1;
		if (!PNTShadowmapPipelineState) {
			ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&PNTShadowmapPipelineState)));
			NAME_D3D12_OBJECT(PNTShadowmapPipelineState);
			BasicPipelineStatePool::AddPipelineState(L"PNTShadowmap", PNTShadowmapPipelineState);

		}

	}

	//�R���X�^���g�o�b�t�@�̓o�^
	void Shadowmap::SetConstant(ShadowConstant& constant, const shared_ptr<Transform>& transform) {
		//���ׂ�0�ɏ�����
		constant = {};
		bsm::Mat4x4 world;
		auto lights = GetGameObject()->GetActiveLightSet();
		auto light = lights->GetMainBaseLight();

		//�ʒu�̎擾
		auto pos = transform->GetWorldMatrix().transInMatrix();
		bsm::Vec3 posSpan = light.m_directional;
		posSpan *= m_posAdjustment;
		pos += posSpan;
		//���[���h�s��̌���
		world.affineTransformation(
			transform->GetScale(),			//�X�P�[�����O
			transform->GetPivot(),		//��]�̒��S�i�d�S�j
			transform->GetQuaternion(),				//��]�p�x
			pos				//�ʒu
		);


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

		ComPtr<ID3D12PipelineState> PNTShadowmapPipelineState
			= BasicPipelineStatePool::GetPipelineState(L"PNTShadowmap",true);

		auto pCommandList = pDevice->GetComandList().Get();
		pCommandList->SetPipelineState(PNTShadowmapPipelineState.Get());
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
				L"Shadowmap::PopulateCommandList()"
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

	void Shadowmap::WriteConstantBuffers(BaseFrame* pBaseFrame) {
		ShadowConstant constants;
		SetConstant(constants, GetGameObject()->GetComponent<Transform>());
		memcpy(pBaseFrame->m_constBuffParamVec[m_constBuffParamIndex].m_pConstantBuffer, &constants, sizeof(ShadowConstant));

	}

	Shadowmap::Shadowmap(const shared_ptr<GameObject>& gameObjectPtr) :
		Component(gameObjectPtr)
	{
		SetDrawActive(true);
	}

	void Shadowmap::OnCreate() {
		CreatePipelineStates();
	}



}
// end namespace basecross