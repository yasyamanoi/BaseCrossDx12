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
	IMPLEMENT_DX12SHADER(VSPCSprite, App::GetShadersDir() + L"VSPCSprite.cso")
	IMPLEMENT_DX12SHADER(PSPCSprite, App::GetShadersDir() + L"PSPCSprite.cso")
	//--------------------------------------------------------------------------------------
	///	シーン
	//--------------------------------------------------------------------------------------
	Scene::Scene(UINT frameCount, PrimDevice* pPrimDevice) :
		BaseScene(frameCount, pPrimDevice)
	{
	}

	Scene::~Scene(){}

	//--------------------------------------------------------------------------------------
	// D3Dリソースの作成
	//--------------------------------------------------------------------------------------
	void Scene::CreatePipelineStates(ID3D12Device* pDevice) {
		// スプライトのパイプラインステートの作成
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = { VertexPositionColor::GetVertexElement(), VertexPositionColor::GetNumElements() };
			psoDesc.pRootSignature = m_rootSignatures[RootSignature::SpritePass].Get();
			psoDesc.VS =
			{
				reinterpret_cast<UINT8*>(VSPCSprite::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				VSPCSprite::GetPtr()->GetShaderComPtr()->GetBufferSize()
			};
			psoDesc.PS =
			{
				reinterpret_cast<UINT8*>(PSPCSprite::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
				PSPCSprite::GetPtr()->GetShaderComPtr()->GetBufferSize()
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

			ComPtr<ID3D12PipelineState> pipelineStateSprite;
			ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineStateSprite)));
			PipelineStatePool::AddPipelineState(L"PCSprite", pipelineStateSprite);
		}
	}

	//--------------------------------------------------------------------------------------
	// オブジェクト作成
	//--------------------------------------------------------------------------------------
	void Scene::CreateSceneResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) {
		auto scale = Vec3(480, 480, 1);
		auto pos = Vec3(0, 0, 0);
		m_pcSprite = ObjectFactory::CreateInitParam<PcSprite>(pCommandList, scale, pos);
	}

	//--------------------------------------------------------------------------------------
	//// オブジェクト更新
	//--------------------------------------------------------------------------------------
	void Scene::Update(double elapsedTime) {
		m_pcSprite->OnUpdate(elapsedTime);
	}
	//--------------------------------------------------------------------------------------
	// 描画処理（シーン）
	//--------------------------------------------------------------------------------------
	void Scene::ScenePass(ID3D12GraphicsCommandList* pCommandList) {
		m_pcSprite->OnSceneDraw(pCommandList);
	}

}
// end namespace basecross
