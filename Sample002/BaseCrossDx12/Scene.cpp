/*!
@file Scene.cpp
@brief シーンクラス。必要であれば、BaseSceneを再定義
*/

#include "stdafx.h"

using namespace bsm;
using namespace basecross;
namespace basecross {
	//--------------------------------------------------------------------------------------
	///	シェーダー
	//--------------------------------------------------------------------------------------
	IMPLEMENT_DX12SHADER(VSPNTStatic, App::GetShadersDir() + L"VSPNTStatic.cso")
	IMPLEMENT_DX12SHADER(PSPNTStatic, App::GetShadersDir() + L"PSPNTStatic.cso")
	//--------------------------------------------------------------------------------------
	///	シーン
	//--------------------------------------------------------------------------------------
	Scene::Scene(UINT frameCount, PrimDevice* pPrimDevice) :
	BaseScene(frameCount, pPrimDevice)
	{
	}

	Scene::~Scene(){}
	//--------------------------------------------------------------------------------------
	// パイプラインステートの作成
	//--------------------------------------------------------------------------------------
	void Scene::CreatePipelineStates(ID3D12Device* pDevice) {
		// 3Dシーンのパイプラインステートの作成
		{
			//デプスステンシルステート
			CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);
			depthStencilDesc.DepthEnable = true;
			depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
			depthStencilDesc.StencilEnable = FALSE;

			// 3Dシーンのパイプラインステート
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
	// サンプラーの作成
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
		//カメラの構築
		m_perspecCamera = ObjectFactory::Create<PerspecCamera>();
		m_perspecCamera->SetEye(Vec3(0.0f, 5.0f, -5.0f));
		m_perspecCamera->SetAt(Vec3(0.0f, 0.0f, 0.0f));
		m_perspecCamera->SetUp(Vec3(0.0f, 1.0f, 0.0f));
		//ライトの構築
		m_lightSet = ObjectFactory::Create<LightSet>();
		//配置するオブジェクト（2つ）の作成
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
