/*!
@file BaseScene.cpp
@brief シーン親クラス
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	IMPLEMENT_DX12SHADER(BcVSPNTStaticPL, App::GetShadersPath() + L"BcVSPNTStaticPL.cso")
	IMPLEMENT_DX12SHADER(BcPSPNTPL, App::GetShadersPath() + L"BcPSPNTPL.cso")


	void BaseScene::CreateDefaultPipelineStates() {
		ComPtr<ID3D12PipelineState> pipelineState;
		auto pDevice = App::GetDefaultDevice();
		CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
		//カリング
		rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		ZeroMemory(&psoDesc, sizeof(psoDesc));
		psoDesc.InputLayout = { VertexPositionNormalTexture::GetVertexElement(), VertexPositionNormalTexture::GetNumElements() };
		psoDesc.pRootSignature = pDevice->GetRootSignature().Get();
		psoDesc.VS =
		{
			reinterpret_cast<UINT8*>(BcVSPNTStaticPL::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			BcVSPNTStaticPL::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<UINT8*>(BcPSPNTPL::GetPtr()->GetShaderComPtr()->GetBufferPointer()),
			BcPSPNTPL::GetPtr()->GetShaderComPtr()->GetBufferSize()
		};
		psoDesc.RasterizerState = rasterizerStateDesc;
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
		NAME_D3D12_OBJECT(pipelineState);
		//作成したパイプライを登録
		AddPipelineState(L"BcPNTStaticPL", pipelineState);
	}

	void BaseScene::CreateDefaultMeshes() {
		//CUBEメッシュ
		auto pDevice = App::GetDefaultDevice();
		auto commandList = pDevice->GetComandList();
		vector<VertexPositionNormalTexture> vertices;
		vector<uint32_t> indices;
		MeshUtill::CreateCube(1.0f, vertices, indices);
		auto mesh = BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(commandList, vertices, indices);
		//作成したメッシュを登録
		AddMesh(L"DEFAULT_CUBE", mesh);
    }

	shared_ptr<BaseTexture> BaseScene::CreateTextureFlomFile(const wstring& falsename) {
		auto pDevice = App::GetDefaultDevice();
		auto commandList = pDevice->GetComandList();
		//テクスチャの作成
		//シェーダリソースハンドルを作成
		UINT srvIndex = pDevice->GetCbvSrvUavNextIndex();
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
			srvIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		//画像ファイルをもとにテクスチャを作成(SRV込み)
		auto texture = BaseTexture::CreateBaseTextureFromFile(commandList, falsename, srvHandle);
		texture->SetSrvIndex(srvIndex);
		return texture;
	}


	void BaseScene::OnInit() {
		//デフォルトのパイプラインの作成
		CreateDefaultPipelineStates();
		//デフォルトのメッシュの作成
		CreateDefaultMeshes();
	}

	void BaseScene::OnInitFrame(FrameResource* pFrameResource) {
		auto pDefaultDev = App::GetDefaultDevice();
		auto pDevice = App::GetID3D12Device();
		ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pFrameResource->m_commandAllocator)));
		auto stagePtr = GetActiveStage();
		if (stagePtr) {
			stagePtr->OnInitFrame(pFrameResource);
		}
	}
	void BaseScene::PopulateCommandList(FrameResource* pFrameResource) {
		auto stagePtr = GetActiveStage();
		if (stagePtr) {
			stagePtr->OnRender();
		}
	}

	void BaseScene::WriteConstantBuffers(FrameResource* pFrameResource) {
		auto stagePtr = GetActiveStage();
		if (stagePtr) {
			stagePtr->WriteConstantBuffers(pFrameResource);
		}
	}
	void BaseScene::OnUpdate() {
		auto stagePtr = GetActiveStage();
		if (stagePtr) {
			stagePtr->UpdateGameObjects();
			stagePtr->OnUpdate();
		}
	}

	void BaseScene::OnDestroy() {
		auto stagePtr = GetActiveStage();
		if (stagePtr) {
			stagePtr->OnDestroy();
		}
	}

	void BaseScene::OnKeyDown(UINT8 key) {
		auto stagePtr = GetActiveStage();
		if (stagePtr) {
			stagePtr->OnKeyDown(key);
		}
	}
	void BaseScene::OnKeyUp(UINT8 key) {
		auto stagePtr = GetActiveStage();
		if (stagePtr) {
			stagePtr->OnKeyUp(key);
		}
	}


}
//end basecross
