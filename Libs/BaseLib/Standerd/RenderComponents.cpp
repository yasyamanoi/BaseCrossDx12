/*!
@file RenderComponents.cpp
@brief 描画コンポーネント
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {

	RenderComponent::RenderComponent(const shared_ptr<GameObject>& gameObjectPtr) :
		Component(gameObjectPtr)
	{}




	BcPNTStatic::BcPNTStatic(const shared_ptr<GameObject>& gameObjectPtr):
		RenderComponent(gameObjectPtr)
	{}

	void BcPNTStatic::OnInit() {
		SetPipelineState(App::GetBaseScene()->GetPipelineState(L"BcPNTStaticPL"));
	}


	void BcPNTStatic::OnRender() {
		auto pDefaultDev = App::GetDefaultDevice();
		auto pFrame = pDefaultDev->GetCurrentFrameResource();
		PopulateCommandList(pFrame);

	}

	void BcPNTStatic::PopulateCommandList(FrameResource* pFrameResource) {
		auto pDevice = App::GetDefaultDevice();
		auto pCommandList = pDevice->GetComandList().Get();
		//Srv
		auto texture = m_baseTexture;
		CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			texture->GetSrvIndex(),
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"t0"), srvHandle);

		//Sampler
		CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle(
			pDevice->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			0,
			0
		);
		auto pipeLine = m_pipelineState;
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"s0"), samplerHandle);
		pCommandList->SetPipelineState(pipeLine.Get());
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		auto paramIndex = GetGameObject()->GetParamIndex();
		//Cbv
		CD3DX12_GPU_DESCRIPTOR_HANDLE skyCbvHandle(
			pDevice->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			pFrameResource->m_frameParamVec[paramIndex].m_constBuffIndex,
			pDevice->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pDevice->GetGpuSlotID(L"b0"), skyCbvHandle);

		auto mesh = m_baseMesh;
		pCommandList->IASetVertexBuffers(0, 1, &mesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&mesh->GetIndexBufferView());
		pCommandList->DrawIndexedInstanced(mesh->GetNumIndices(), 1, 0, 0, 0);
	}




}
// end basecross
