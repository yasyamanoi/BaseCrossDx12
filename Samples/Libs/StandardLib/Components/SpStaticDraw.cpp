/*!
@file BcStaticDraw.cpp
@brief �x�C�V�b�N�X�^�e�B�b�N�`��R���|�[�l���g�@����
*/

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	SpStatic�`��R���|�[�l���g(�e)
	//--------------------------------------------------------------------------------------
	void SpStaticDraw::OnCreate() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		//�V���v���R���X�^���g�o�b�t�@
		for (size_t i = 0; i < BaseDevice::FrameCount; i++) {
			m_constantIndex =
				frameResources[i]->AddBaseConstantBufferSet<SimpleConstant>(pBaseDevice->GetD3D12Device());
		}
		// �V�[���p�C�v���C���X�e�[�g
		{
			ComPtr<ID3D12PipelineState> defaultPipelineState
				= PipelineStatePool::GetPipelineState(L"SpPNTStatic");
			ComPtr<ID3D12PipelineState> defaultShadowPipelineState
				= PipelineStatePool::GetPipelineState(L"SpPNTStaticShadow");
			ComPtr<ID3D12PipelineState> alphaPipelineState
				= PipelineStatePool::GetPipelineState(L"SpPNTStaticAlpha");
			ComPtr<ID3D12PipelineState> alphaShadowPipelineState
				= PipelineStatePool::GetPipelineState(L"SpPNTStaticAlphaShadow");

			CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
			//�J�����O
			rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			ZeroMemory(&psoDesc, sizeof(psoDesc));
			psoDesc.InputLayout = { VertexPositionNormalTexture::GetVertexElement(), VertexPositionNormalTexture::GetNumElements() };
			psoDesc.pRootSignature = RootSignaturePool::GetRootSignature(L"BaseCrossDefault").Get();
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
			psoDesc.BlendState = BlendState::GetOpaqueBlend();
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
				PipelineStatePool::AddPipelineState(L"SpPNTStatic", defaultPipelineState);

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
				PipelineStatePool::AddPipelineState(L"SpPNTStaticShadow", defaultShadowPipelineState);

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
			psoDesc.BlendState = BlendState::GetAlphaBlend();
			if (!alphaPipelineState) {
				ThrowIfFailed(App::GetID3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&alphaPipelineState)));
				NAME_D3D12_OBJECT(alphaPipelineState);
				PipelineStatePool::AddPipelineState(L"SpPNTStaticAlpha", alphaPipelineState);

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
				PipelineStatePool::AddPipelineState(L"SpPNTStaticAlphaShadow", alphaPipelineState);
			}
		}
	}


	//--------------------------------------------------------------------------------------
	///	SpPNTStatic�`��R���|�[�l���g
	//--------------------------------------------------------------------------------------

	IMPLEMENT_DX12SHADER(SpVSPNTStatic, App::GetShadersDir() + L"SpVSPNTStatic.cso")
	IMPLEMENT_DX12SHADER(SpVSPNTStaticShadow, App::GetShadersDir() + L"SpVSPNTStaticShadow.cso")

	IMPLEMENT_DX12SHADER(SpPSPNTStatic, App::GetShadersDir() + L"SpPSPNTStatic.cso")
	IMPLEMENT_DX12SHADER(SpPSPNTStaticShadow, App::GetShadersDir() + L"SpPSPNTStaticShadow.cso")

	SpPNTStaticDraw::SpPNTStaticDraw(const std::shared_ptr<GameObject>& gameObjectPtr) :
		SpStaticDraw(gameObjectPtr)
	{

	}

	SpPNTStaticDraw::SpPNTStaticDraw(const std::shared_ptr<GameObject>& gameObjectPtr,
		std::vector<VertexPositionNormalTexture>& vertices, std::vector<uint32_t>& indices) :
		SpStaticDraw(gameObjectPtr)
	{
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		//�I���W�i���ȃ��b�V�����g��
		m_mesh = BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(pCommandList, vertices, indices);
	}



	void SpPNTStaticDraw::OnUpdateConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto stage = std::dynamic_pointer_cast<Stage>(scene->GetActiveStage(true));
		auto& frameResources = scene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		auto& viewport = scene->GetViewport();
		std::shared_ptr<PerspecCamera> myCamera;
		std::shared_ptr<LightSet> myLightSet;
		if (!stage) {
			return;
		}

		auto gameObject = m_gameObject.lock();
		if (gameObject) {

			myCamera = std::dynamic_pointer_cast<PerspecCamera>(gameObject->GetCamera());
			myLightSet = gameObject->GetLightSet();

			//Transform�R���|�[�l���g�����o��
			auto ptrTrans = gameObject->GetComponent<Transform>();
			auto& param = ptrTrans->GetTransParam();
			//�V�[���̃R���X�^���g�o�b�t�@
			{
				//������
				m_simpleConstant = {};

				Mat4x4 world = ptrTrans->GetWorldMatrix();
				//�]�u����
				world.transpose();
				//�r���[�Ǝˉe�s��𓾂�
				Mat4x4 viewMat = myCamera->GetViewMatrix();
				//�]�u����
				viewMat.transpose();
				Mat4x4 projMat = myCamera->GetProjMatrix();
				//�]�u����
				projMat.transpose();
				m_simpleConstant.world = world;
				m_simpleConstant.view = viewMat;
				m_simpleConstant.projection = projMat;
				//�G�~�b�V�u
				m_simpleConstant.emissive = GetEmissive();
				//�f�t�B�[�Y
				m_simpleConstant.diffuse = GetDiffuse();
				//�X�y�L�����[
				m_simpleConstant.specular = GetSpecular();
				//���C�e�B���O
				auto index = myLightSet->GetMainIndex();
				auto light = myLightSet->GetLight(index);
				m_simpleConstant.lightDir = light.m_directional;
				m_simpleConstant.lightDir.w = 1.0f;
				m_simpleConstant.eyePos = myCamera->GetEye();
				m_simpleConstant.eyePos.w = 1.0f;
				//�e�N�X�`�����������ꍇ
				if (m_textureVec.size() > 0) {
					m_simpleConstant.activeFlg.x = 1;
				}
				//�e�p
				if (IsOwnShadowActive()) {
					Vec3 CalcLightDir = light.m_directional * -1.0;
					Vec3 LightAt = myCamera->GetAt();
					Vec3 LightEye = CalcLightDir;
					LightEye *= Shadowmap::GetLightHeight();
					LightEye = LightAt + LightEye;
					m_simpleConstant.lightPos = LightEye;
					m_simpleConstant.lightPos.w = 1.0f;
					Mat4x4 lightView, lightProj;
					//���C�g�̃r���[�Ǝˉe���v�Z
					lightView = XMMatrixLookAtLH(LightEye, LightAt, Vec3(0, 1.0f, 0));
					lightProj = XMMatrixOrthographicLH(Shadowmap::GetViewWidth(), Shadowmap::GetViewHeight(),
						Shadowmap::GetLightNear(), Shadowmap::GetLightFar());
					m_simpleConstant.lightView = bsmUtil::transpose(lightView);
					m_simpleConstant.lightProjection = bsmUtil::transpose(lightProj);
				}
			}
		}
	}

	void SpPNTStaticDraw::OnCommitConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto pCurrentFrameResource = scene->GetCurrentFrameResource();
		//�V�[��
		memcpy(pCurrentFrameResource->m_baseConstantBufferSetVec[m_constantIndex].m_pBaseConstantBufferWO,
			&m_simpleConstant, sizeof(m_simpleConstant));
	}

	void SpPNTStaticDraw::OnSceneDraw() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pCurrentFrameResource = pBaseScene->GetCurrentFrameResource();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		auto& viewport = pBaseScene->GetViewport();
		auto& scissorRect = pBaseScene->GetScissorRect();
		auto depthDsvs = pBaseScene->GetDepthDsvs();
		auto depthGPUDsvs = pBaseScene->GetDepthSrvGpuHandles();

		auto CbvSrvUavDescriptorHeap = pBaseScene->GetCbvSrvUavDescriptorHeap();
		auto mesh = GetBaseMesh(0);
		auto texture = GetBaseTexture(0);
		if (mesh && texture) {

			ComPtr<ID3D12PipelineState> defaultPipelineState
				= PipelineStatePool::GetPipelineState(L"SpPNTStatic", true);
			ComPtr<ID3D12PipelineState> defaultShadowPipelineState
				= PipelineStatePool::GetPipelineState(L"SpPNTStaticShadow", true);
			ComPtr<ID3D12PipelineState> alphaPipelineState
				= PipelineStatePool::GetPipelineState(L"SpPNTStaticAlpha", true);
			ComPtr<ID3D12PipelineState> alphaShadowPipelineState
				= PipelineStatePool::GetPipelineState(L"SpPNTStaticAlphaShadow", true);
			//null rv
			CD3DX12_GPU_DESCRIPTOR_HANDLE cbvSrvGpuNullHandle(CbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

			// set PipelineState and GetGpuSlotID(L"t0")
			if (GetGameObject()->IsAlphaActive()) {
				if (IsOwnShadowActive()) {
					pCommandList->SetPipelineState(alphaShadowPipelineState.Get());
					pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t0"), depthGPUDsvs[SceneEnums::DepthGenPass::Shadow]);        // Set the shadow texture as an SRV.
				}
				else {
					pCommandList->SetPipelineState(alphaPipelineState.Get());
					pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t0"), cbvSrvGpuNullHandle);        // Set the shadow texture as an SRV.
				}
			}
			else {
				if (IsOwnShadowActive()) {
					pCommandList->SetPipelineState(defaultShadowPipelineState.Get());
					pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t0"), depthGPUDsvs[SceneEnums::DepthGenPass::Shadow]);        // Set the shadow texture as an SRV.
				}
				else {
					pCommandList->SetPipelineState(defaultPipelineState.Get());
					pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t0"), cbvSrvGpuNullHandle);        // Set the shadow texture as an SRV.
				}

			}
			//Sampler
			UINT index = pBaseScene->GetSamplerIndex(L"LinearClamp");
			if (index == UINT_MAX) {
				throw BaseException(
					L"LinearClamp�T���v���[������ł��܂���B",
					L"Scene::ScenePass()"
				);
			}
			CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle(
				pBaseScene->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				index,
				pBaseScene->GetSamplerDescriptorHandleIncrementSize()
			);
			pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"s0"), samplerHandle);

			index = pBaseScene->GetSamplerIndex(L"ComparisonLinear");
			if (index == UINT_MAX) {
				throw BaseException(
					L"ComparisonLinear�T���v���[������ł��܂���B",
					L"Scene::ScenePass()"
				);
			}
			CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle2(
				pBaseScene->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				index,
				pBaseScene->GetSamplerDescriptorHandleIncrementSize()
			);
			pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"s1"), samplerHandle2);
			//�V�F�[�_���\�[�X�i�e�N�X�`���j�̃n���h���̐ݒ�
			CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(
				pBaseScene->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
				texture->GetSrvIndex(),
				pBaseScene->GetCbvSrvUavDescriptorHandleIncrementSize()
			);
			pCommandList->SetGraphicsRootDescriptorTable(pBaseScene->GetGpuSlotID(L"t1"), srvHandle);
			//Cbv
			// Set scene constant buffer.
			pCommandList->SetGraphicsRootConstantBufferView(pBaseScene->GetGpuSlotID(L"b0"),
				pCurrentFrameResource->m_baseConstantBufferSetVec[m_constantIndex].m_baseConstantBuffer->GetGPUVirtualAddress());
			//Draw
			pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			pCommandList->IASetVertexBuffers(0, 1, &mesh->GetVertexBufferView());
			pCommandList->IASetIndexBuffer(&mesh->GetIndexBufferView());
			pCommandList->DrawIndexedInstanced(mesh->GetNumIndices(), 1, 0, 0, 0);
		}

	}






}
// end namespace basecross
