/*!
@file Scene.cpp
@brief �V�[���N���X�B�K�v�ł���΁ABaseScene���Ē�`
*/

#include "stdafx.h"

using namespace bsm;
using namespace basecross;
namespace basecross {
	//--------------------------------------------------------------------------------------
	///	�V�F�[�_�[
	//--------------------------------------------------------------------------------------
	IMPLEMENT_DX12SHADER(VSPNTStatic, App::GetShadersDir() + L"VSPNTStatic.cso")
	IMPLEMENT_DX12SHADER(PSPNTStatic, App::GetShadersDir() + L"PSPNTStatic.cso")
	//--------------------------------------------------------------------------------------
	///	�V�[��
	//--------------------------------------------------------------------------------------
	Scene::Scene(UINT frameCount, PrimDevice* pPrimDevice) :
	BaseScene(frameCount, pPrimDevice)
	{
	}

	Scene::~Scene(){}
	//--------------------------------------------------------------------------------------
	// �p�C�v���C���X�e�[�g�̍쐬
	//--------------------------------------------------------------------------------------
	void Scene::CreatePipelineStates(ID3D12Device* pDevice) {
		// 3D�V�[���̃p�C�v���C���X�e�[�g�̍쐬
		{
			//�f�v�X�X�e���V���X�e�[�g
			CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);
			depthStencilDesc.DepthEnable = true;
			depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
			depthStencilDesc.StencilEnable = FALSE;

			// 3D�V�[���̃p�C�v���C���X�e�[�g
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = { VertexPositionNormalTexture::GetVertexElement(), VertexPositionNormalTexture::GetNumElements() };
			psoDesc.pRootSignature = m_rootSignatures[RootSignature::ScenePass].Get();
			psoDesc.VS =
			{
				reinterpret_cast<UINT8*>(VSPNTStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				VSPNTStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
			};
			psoDesc.PS =
			{
				reinterpret_cast<UINT8*>(PSPNTStatic::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				PSPNTStatic::GetPtr()->GetShaderComPtr()->GetBufferSize()
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

			ComPtr<ID3D12PipelineState> pipelineStateScene;
			ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineStateScene)));
			PipelineStatePool::AddPipelineState(L"PNTStatic", pipelineStateScene);
		}
	}

	//--------------------------------------------------------------------------------------
	// �T���v���[�̍쐬
	//--------------------------------------------------------------------------------------
	void Scene::CreateSamplers(ID3D12Device* pDevice) {
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
	}

	void Scene::CreateSceneResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) {
		//�J�����̍\�z
		m_perspecCamera = ObjectFactory::Create<PerspecCamera>();
		m_perspecCamera->SetEye(Vec3(0.0f, 5.0f, -5.0f));
		m_perspecCamera->SetAt(Vec3(0.0f, 0.0f, 0.0f));
		m_perspecCamera->SetUp(Vec3(0.0f, 1.0f, 0.0f));
		//���C�g�̍\�z
		m_lightSet = ObjectFactory::Create<LightSet>();
		//�z�u����I�u�W�F�N�g�i2�j�̍쐬
		auto scale = Vec3(1.0f);
		auto quat = Quat();
		auto pos = Vec3(-2.0f,0,0);
		m_fixedBox1 = ObjectFactory::CreateInitParam<FixedBox>(pCommandList, scale, quat, pos);
		pos.x += 4.0f;
		m_fixedBox2 = ObjectFactory::CreateInitParam<FixedBox>(pCommandList, scale, quat, pos);
	}

	void Scene::Update(double elapsedTime) {
		m_fixedBox1->OnUpdate(elapsedTime);
		m_fixedBox2->OnUpdate(elapsedTime);
	}

	void Scene::ScenePass(ID3D12GraphicsCommandList* pCommandList) {
		m_fixedBox1->OnSceneDraw(pCommandList);
		m_fixedBox2->OnSceneDraw(pCommandList);
	}




}
// end namespace basecross