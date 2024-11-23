/*!
@file BaseFrame.cpp
@brief フレームクラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/

#include "stdafx.h"

namespace basecross {

	using namespace SceneEnums;

	BaseFrame::BaseFrame(ID3D12Device* pDevice, ID3D12CommandQueue* pCommandQueue)
	{
		ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
		NAME_D3D12_OBJECT(m_commandAllocator);

		ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_contextCommandAllocator)));
		NAME_D3D12_OBJECT(m_contextCommandAllocator);

		// Create constant buffers.
		{
			auto scene = std::dynamic_pointer_cast<Scene>(BaseDevice::GetScene());
			if (!scene) {
				throw BaseException(
					L"シーンが取得できません",
					L"BaseFrame::BaseFrame()"
				);
			}

			auto index = scene->GetCbvUavNextIndex();
			CD3DX12_CPU_DESCRIPTOR_HANDLE handle1(
				scene->GetCbvSrvUavDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
				index,
				scene->GetCbvSrvUavDescriptorHandleIncrementSize()
			);
			ThrowIfFailed(CreateConstantBuffer(pDevice, sizeof(BaseConstant), &m_constantBuffers[RenderPass::Shadow], &handle1, D3D12_RESOURCE_STATE_GENERIC_READ));
			m_constantHandleIndeces[RenderPass::Shadow] = index;
			NAME_D3D12_OBJECT(m_constantBuffers[RenderPass::Shadow]);

			// Scene render constant buffer.
			index = scene->GetCbvUavNextIndex();
			CD3DX12_CPU_DESCRIPTOR_HANDLE handle2(
				scene->GetCbvSrvUavDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
				index,
				scene->GetCbvSrvUavDescriptorHandleIncrementSize()
			);
			ThrowIfFailed(CreateConstantBuffer(pDevice, sizeof(BaseConstant), &m_constantBuffers[RenderPass::Scene],&handle2, D3D12_RESOURCE_STATE_GENERIC_READ));
			m_constantHandleIndeces[RenderPass::Scene] = index;
			NAME_D3D12_OBJECT(m_constantBuffers[RenderPass::Scene]);

			// Postprocess pass constant buffer.
			index = scene->GetCbvUavNextIndex();
			CD3DX12_CPU_DESCRIPTOR_HANDLE handle3(
				scene->GetCbvSrvUavDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
				index,
				scene->GetCbvSrvUavDescriptorHandleIncrementSize()
			);
			ThrowIfFailed(CreateConstantBuffer(pDevice, sizeof(PostprocessConstantBuffer), &m_constantBuffers[RenderPass::Postprocess], &handle3, D3D12_RESOURCE_STATE_GENERIC_READ));
			m_constantHandleIndeces[RenderPass::Postprocess] = index;
			NAME_D3D12_OBJECT(m_constantBuffers[RenderPass::Postprocess]);

			// Map the constant buffers and cache their heap pointers.
			// We don't unmap this until the app closes. Keeping buffer mapped for the lifetime of the resource is okay.
			const CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
			ThrowIfFailed(m_constantBuffers[RenderPass::Shadow]->Map(0, &readRange, reinterpret_cast<void**>(&m_pConstantBuffersWO[RenderPass::Shadow])));
			ThrowIfFailed(m_constantBuffers[RenderPass::Scene]->Map(0, &readRange, reinterpret_cast<void**>(&m_pConstantBuffersWO[RenderPass::Scene])));
			ThrowIfFailed(m_constantBuffers[RenderPass::Postprocess]->Map(0, &readRange, reinterpret_cast<void**>(&m_pConstantBuffersWO[RenderPass::Postprocess])));
		}
	}



	BaseFrame::~BaseFrame()
	{
	}

	void BaseFrame::InitFrame()
	{
		// Reset the main thread command allocator.
		m_commandAllocator->Reset();
		ThrowIfFailed(m_contextCommandAllocator->Reset());
	}

	void BaseFrame::BeginFrame(ID3D12GraphicsCommandList* pCommandList)
	{
	}

	void BaseFrame::EndFrame(ID3D12GraphicsCommandList* pCommandList)
	{
	}
}
//end namespace basecross 
