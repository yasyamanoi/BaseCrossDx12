/*!
@file Character.h
@brief ���܂��܂ȃI�u�W�F�N�g
*/

#include "stdafx.h"

using namespace bsm;
using namespace basecross;
namespace basecross {
	//--------------------------------------------------------------------------------------
	//	FixedBox�N���X
	//--------------------------------------------------------------------------------------
	//�\�z�Ɣj��
	FixedBox::FixedBox(
	) :GameObject()
	{
	}
	FixedBox::~FixedBox() {}

	//�������i�p�����[�^�t���j
	void FixedBox::OnCreate(ID3D12GraphicsCommandList* pCommandList, 
		const Vec3& scale, 
		const Quat& quaternion,
		const Vec3& position) {
		//�����o�ϐ��̏�����
		m_scale = scale;
		m_quaternion = quaternion;
		m_firstPosition = m_position = position;
		m_totalTime = 0.0;
		//��]���𗐐��Ō���
		Vec3 qtVec(
			Util::RandZeroToOne() - 0.5f,
			Util::RandZeroToOne() - 0.5f,
			Util::RandZeroToOne() - 0.5f
			);
		qtVec.normalize();
		m_spanQuat = Quat(qtVec, (Util::RandZeroToOne() - 0.5f) * 0.01f);
		m_spanQuat.normalize();
		//�����̍쐬�̏���
		std::vector<VertexPositionNormalTexture> vertices;
		std::vector<uint32_t> indices;
		//�����̂����o��
		MeshUtill::CreateCube(1.0f, vertices, indices);
		//���b�V���쐬
		m_box = BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(pCommandList, vertices, indices);
		//�e�N�X�`���쐬
		auto texFile = App::GetRelativeAssetsDir() + L"wall.jpg";
		m_texture = BaseTexture::CreateTextureFlomFile(pCommandList, texFile);
		//�V�[���̃|�C���^���擾
		auto pScene = BaseDevice::GetScene();
		UINT frameCount = pScene->GetFrameCount();
		m_constBuffParamVec.resize(frameCount);
		//�萔�o�b�t�@�̍쐬
		for (UINT i = 0; i < frameCount; i++) {
			m_constBuffParamVec[i] = pScene->GetframeResources()[i]->CreateConstBuffParam<BaseConstant>();
		}
	}

	void FixedBox::OnUpdate(double elapsedTime) {
		//�V�[���̃|�C���^���擾
		auto pScene = BaseDevice::GetScene();
		//�g�[�^�����Ԃ̕ύX
		m_totalTime += (float)elapsedTime;
		if (m_totalTime > XM_2PI) {
			m_totalTime = 0.0;
		}
		//�s�����藈����̌v�Z
		m_position.x = (float)sin(m_totalTime * 2) + m_firstPosition.x;
		//���݂̉�]�ɒǉ�
		Quat q = m_spanQuat;
		q.z = (float)elapsedTime;
		q.normalize();
		m_quaternion *= q;
		//�萔�o�b�t�@��Update
		auto perspecCamera = pScene->m_perspecCamera;
		auto lightSet = pScene->m_lightSet;
		//3D�p�̒萔�o�b�t�@
		BaseConstant sceneConstantBuffer = {};
		//���[���h�s��̎擾
		Mat4x4 worldMat;
		worldMat.affineTransformation(
			m_scale,
			Vec3(0.0f),
			m_quaternion,
			m_position
		);
		sceneConstantBuffer.world = worldMat;
		//���[���h�s��̓]�u
		sceneConstantBuffer.world.transpose();
		//�r���[�s��̎擾
		sceneConstantBuffer.view = perspecCamera->GetViewMatrix();
		//�r���[�s��̓]�u
		sceneConstantBuffer.view.transpose();
		//�ˉe�s��̎擾
		sceneConstantBuffer.projection = perspecCamera->GetProjMatrix();
		//�ˉe�s��̓]�u
		sceneConstantBuffer.projection.transpose();
		sceneConstantBuffer.emissive = Col4(0.5f);
		sceneConstantBuffer.diffuse = Col4(1.0f);
		sceneConstantBuffer.specular = Col4(0.0f);
		//�e�N�X�`�����g�p����ݒ�
		sceneConstantBuffer.activeFlg.x = 1;
		//���C�g�̐ݒ�
		auto mainLight = lightSet->GetMainBaseLight();
		sceneConstantBuffer.lightDir = mainLight.m_directional;
		sceneConstantBuffer.lightPos = Vec3(2.0f, 10.0f, 5.0f);
		sceneConstantBuffer.eyePos = perspecCamera->GetEye();
		//�萔�o�b�t�@�̃R�~�b�g
		auto frameIndex = pScene->GetFrameIndex();
		memcpy(
			m_constBuffParamVec[frameIndex].m_pConstantBufferWO,
			&sceneConstantBuffer,
			sizeof(sceneConstantBuffer)
		);
	}

	void FixedBox::OnSceneDraw(ID3D12GraphicsCommandList* pCommandList) {
		//�V�[���̃|�C���^���擾
		auto pScene = BaseDevice::GetScene();
		//���[�g�V�O�l�`�����擾
		auto rootSignatures = pScene->GetRootSignatures();
		auto frameIndex = pScene->GetFrameIndex();
		//������`�揈��
		//���[�g�V�O�l�`���̐ݒ�
		pCommandList->SetGraphicsRootSignature(rootSignatures[RootSignature::ScenePass].Get());
		//�p�C�v���C���X�e�[�g�̐ݒ�
		auto scenePipeline = PipelineStatePool::GetPipelineState(L"PNTStatic");
		pCommandList->SetPipelineState(scenePipeline.Get());
		//�萔�o�b�t�@�̃Z�b�g
		pCommandList->SetGraphicsRootConstantBufferView(
			pScene->GetGpuSlotID(L"3d_b0"),
			m_constBuffParamVec[frameIndex].m_constantBuffer->GetGPUVirtualAddress()
		);
		//�T���v���[�̐ݒ�
		UINT index = pScene->GetSamplerIndex(L"Clamp");
		CD3DX12_GPU_DESCRIPTOR_HANDLE s0Handle(
			pScene->GetSamplerDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			index,
			pScene->GetSamplerDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pScene->GetGpuSlotID(L"3d_s0"), s0Handle);
		//�e�N�X�`���̐ݒ�
		CD3DX12_GPU_DESCRIPTOR_HANDLE t0Handle(
			pScene->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			m_texture->GetSrvIndex(),
			pScene->GetCbvSrvUavDescriptorHandleIncrementSize()
		);
		pCommandList->SetGraphicsRootDescriptorTable(pScene->GetGpuSlotID(L"3d_t0"), t0Handle);
		//���̂ق��̕K�v�Ȑݒ�
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pCommandList->IASetVertexBuffers(0, 1, &m_box->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&m_box->GetIndexBufferView());
		pCommandList->RSSetViewports(1, &pScene->GetViewport());
		pCommandList->RSSetScissorRects(1, &pScene->GetScissorRect());
		pCommandList->OMSetRenderTargets(1, &pScene->GetCurrentBackBufferRtvCpuHandle(),
			FALSE, &pScene->GetDepthDsvs()[DepthGenPass::Scene]);
		//FixedBox�̕`��
		pCommandList->DrawIndexedInstanced(m_box->GetNumIndices(), 1, 0, 0, 0);
	}


}
// end namespace basecross