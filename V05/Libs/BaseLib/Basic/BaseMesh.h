/*!
@file BaseMesh.h
@brief ���b�V���N���X
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	���b�V���N���X
	//--------------------------------------------------------------------------------------
	class BaseMesh {
		ComPtr<ID3D12Resource> m_vertexBuffer;
		ComPtr<ID3D12Resource> m_indexBuffer;
		ComPtr<ID3D12Resource> m_vertexBufferUploadHeap;
		ComPtr<ID3D12Resource> m_indexBufferUploadHeap;

		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
		UINT m_numVertices;
		UINT m_numIndices;
	protected:
		BaseMesh() {}
	public:
		~BaseMesh() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief ���_�o�b�t�@�̎擾
		@return	���_�o�b�t�@��ComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12Resource> GetVertexBuffer() const {
			return m_vertexBuffer;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief ���_�o�b�t�@�A�b�v���[�h�q�[�v�̎擾
		@return	���_�o�b�t�@�A�b�v���[�h�q�[�v��ComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12Resource> GetVertexBufferUploadHeap() const {
			return m_vertexBufferUploadHeap;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief �C���f�b�N�X�o�b�t�@�̎擾
		@return	�C���f�b�N�X�o�b�t�@��ComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12Resource> GetIndexBuffer() const {
			return m_vertexBuffer;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief �C���f�b�N�X�o�b�t�@�A�b�v���[�h�q�[�v�̎擾
		@return	�C���f�b�N�X�o�b�t�@�A�b�v���[�h�q�[�v��ComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12Resource> GetIndexBufferUploadHeap() const {
			return m_indexBufferUploadHeap;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief ���_�o�b�t�@�̒��_���̎擾
		@return	���_�o�b�t�@�̒��_��
		*/
		//--------------------------------------------------------------------------------------
		UINT GetNumVertices()const {
			return m_numVertices;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief �C���f�b�N�X�o�b�t�@�̐��̎擾
		@return	�C���f�b�N�X�o�b�t�@�̐�
		*/
		//--------------------------------------------------------------------------------------
		UINT GetNumIndices()const {
			return m_numIndices;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief ���_�o�b�t�@�r���[�̎擾
		@return	���_�o�b�t�@�r���[
		*/
		//--------------------------------------------------------------------------------------
		const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const {
			return m_vertexBufferView;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief �C���f�b�N�X�o�b�t�@�r���[�̎擾
		@return	�C���f�b�N�X�o�b�t�@�r���[
		*/
		//--------------------------------------------------------------------------------------
		const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const {
			return m_indexBufferView;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief ���_�݂̂ō\������郁�b�V���̍쐬
		@tparam T	�쐬���钸�_�̌^
		@param[in]	commandList	�R�}���h���X�g
		@param[in]	vertices	���_�̔z��
		@return	BaseMesh��ComPtr
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		static shared_ptr<BaseMesh> CreateBaseMesh(ComPtr<ID3D12GraphicsCommandList>& commandList,const vector<T>& vertices) {
			//�f�o�C�X�̎擾
			auto device = App::GetID3D12Device();
			auto baseDevice = App::GetBaseDevice();
			shared_ptr<BaseMesh> ptrMesh = shared_ptr<BaseMesh>(new BaseMesh());
			UINT vertexBufferSize = (UINT)(sizeof(T) * vertices.size());
			//���_�o�b�t�@�̍쐬
			{
				ThrowIfFailed(device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
					D3D12_RESOURCE_STATE_COPY_DEST,
					nullptr,
					IID_PPV_ARGS(&ptrMesh->m_vertexBuffer)),
					L"���_�o�b�t�@�쐬�Ɏ��s���܂����B",
					L"if(FAILED(device->CreateCommittedResource())",
					L"BaseMesh::CreateBaseMesh()"
				);

				ThrowIfFailed(device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&ptrMesh->m_vertexBufferUploadHeap)),
					L"���_�o�b�t�@�쐬�Ɏ��s���܂����B",
					L"if(FAILED(device->CreateCommittedResource())",
					L"BaseMesh::CreateBaseMesh()"
				);

				//���_�o�b�t�@�̍X�V
				D3D12_SUBRESOURCE_DATA vertexData = {};
				vertexData.pData = &vertices[0];
				vertexData.RowPitch = vertexBufferSize;
				vertexData.SlicePitch = vertexData.RowPitch;

				UpdateSubresources<1>(
					commandList.Get(),
					ptrMesh->m_vertexBuffer.Get(),
					ptrMesh->m_vertexBufferUploadHeap.Get(),
					0, 
					0, 
					1, 
					&vertexData
					);
				commandList->ResourceBarrier(
					1, 
					&CD3DX12_RESOURCE_BARRIER::Transition(
						ptrMesh->m_vertexBuffer.Get(),
						D3D12_RESOURCE_STATE_COPY_DEST, 
						D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
					)
				);

				ptrMesh->m_vertexBufferView.BufferLocation = ptrMesh->m_vertexBuffer->GetGPUVirtualAddress();
				ptrMesh->m_vertexBufferView.StrideInBytes = static_cast<UINT>(sizeof(T));
				ptrMesh->m_vertexBufferView.SizeInBytes = vertexBufferSize;
			}
			//���_���̐ݒ�
			ptrMesh->m_numVertices = static_cast<UINT>(vertices.size());
			return ptrMesh;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief ���_�ƃC���f�b�N�X�ō\������郁�b�V���̍쐬
		@tparam T	�쐬���钸�_�̌^
		@param[in]	commandList	�R�}���h���X�g
		@param[in]	vertices	���_�̔z��
		@param[in]	indices	�C���f�b�N�X�̔z��
		@return	BaseMesh��ComPtr
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		static shared_ptr<BaseMesh> CreateBaseMesh(ComPtr<ID3D12GraphicsCommandList>& commandList,const vector<T>& vertices, const vector<uint32_t>& indices) {
			//�f�o�C�X�̎擾
			auto device = App::GetID3D12Device();
			shared_ptr<BaseMesh> ptrMesh = shared_ptr<BaseMesh>(new BaseMesh());
			UINT vertexBufferSize = (UINT)(sizeof(T) * vertices.size());
			//���_�o�b�t�@�̍쐬
			{
				ThrowIfFailed(device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
					D3D12_RESOURCE_STATE_COPY_DEST,
					nullptr,
					IID_PPV_ARGS(&ptrMesh->m_vertexBuffer)),
					L"���_�o�b�t�@�쐬�Ɏ��s���܂����B",
					L"if(FAILED(device->CreateCommittedResource())",
					L"BaseMesh::CreateBaseMesh()"
				);

				ThrowIfFailed(device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&ptrMesh->m_vertexBufferUploadHeap)),
					L"���_�o�b�t�@�쐬�Ɏ��s���܂����B",
					L"if(FAILED(device->CreateCommittedResource())",
					L"BaseMesh::CreateBaseMesh()"
				);

				//���_�o�b�t�@�̍X�V
				D3D12_SUBRESOURCE_DATA vertexData = {};
				vertexData.pData = &vertices[0];
				vertexData.RowPitch = vertexBufferSize;
				vertexData.SlicePitch = vertexData.RowPitch;

				UpdateSubresources<1>(
					commandList.Get(),
					ptrMesh->m_vertexBuffer.Get(),
					ptrMesh->m_vertexBufferUploadHeap.Get(),
					0,
					0,
					1,
					&vertexData
					);
				commandList->ResourceBarrier(
					1,
					&CD3DX12_RESOURCE_BARRIER::Transition(
						ptrMesh->m_vertexBuffer.Get(),
						D3D12_RESOURCE_STATE_COPY_DEST,
						D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
					)
				);

				//���_�o�b�t�@�r���[�̍쐬
				ptrMesh->m_vertexBufferView.BufferLocation = ptrMesh->m_vertexBuffer->GetGPUVirtualAddress();
				ptrMesh->m_vertexBufferView.StrideInBytes = static_cast<UINT>(sizeof(T));
				ptrMesh->m_vertexBufferView.SizeInBytes = vertexBufferSize;
			}
			//���_���̐ݒ�
			ptrMesh->m_numVertices = static_cast<UINT>(vertices.size());
			//�C���f�b�N�X�o�b�t�@�̍쐬
			UINT indexBufferSize = static_cast<UINT>(sizeof(uint32_t) * indices.size());
			{
				ThrowIfFailed(device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
					D3D12_RESOURCE_STATE_COPY_DEST,
					nullptr,
					IID_PPV_ARGS(&ptrMesh->m_indexBuffer)),
					L"�C���f�b�N�X�o�b�t�@�쐬�Ɏ��s���܂����B",
					L"if(FAILED(device->CreateCommittedResource())",
					L"BaseMesh::CreateBaseMesh()"
				);

				ThrowIfFailed(device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&ptrMesh->m_indexBufferUploadHeap)),
					L"�C���f�b�N�X�A�b�v���[�h�o�b�t�@�쐬�Ɏ��s���܂����B",
					L"if(FAILED(device->CreateCommittedResource())",
					L"BaseMesh::CreateBaseMesh()"
				);

				// �C���f�N�X�o�b�t�@�̍X�V
				D3D12_SUBRESOURCE_DATA indexData = {};
				indexData.pData = (void*)&indices[0];
				indexData.RowPitch = indexBufferSize;
				indexData.SlicePitch = indexData.RowPitch;

				UpdateSubresources<1>(
					commandList.Get(), 
					ptrMesh->m_indexBuffer.Get(), 
					ptrMesh->m_indexBufferUploadHeap.Get(), 
					0, 0, 1, &indexData
				);
				commandList->ResourceBarrier(
					1, 
					&CD3DX12_RESOURCE_BARRIER::Transition(
						ptrMesh->m_indexBuffer.Get(),
						D3D12_RESOURCE_STATE_COPY_DEST, 
						D3D12_RESOURCE_STATE_INDEX_BUFFER
						)
					);


				//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
				ptrMesh->m_indexBufferView.BufferLocation = ptrMesh->m_indexBuffer->GetGPUVirtualAddress();
				ptrMesh->m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
				ptrMesh->m_indexBufferView.SizeInBytes = indexBufferSize;

			}
			//�C���f�b�N�X���̐ݒ�
			ptrMesh->m_numIndices = static_cast<UINT>(indices.size());
			return ptrMesh;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���_�̕ύX.<br />
		AccessWrite��true�ō쐬���ꂽ���\�[�X�́A���_�̔z��ɂ���Ē��_��ύX����B
		@tparam	T	���_�̌^
		@param[in]	vertices	���_�̔z��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		void UpdateVirtexBuffer(ComPtr<ID3D12GraphicsCommandList>& commandList, const vector<T>& vertices) {
			if (m_numVertices != vertices.size()) {
				throw BaseException(
					L"���_�����Ⴂ�܂��B",
					L"BaseMesh::UpdateVirtexBuffer()"
				);
			}
			//�f�o�C�X�̎擾
			auto device = App::GetID3D12Device();
			UINT vertexBufferSize = (UINT)(sizeof(T) * vertices.size());
			//���_�o�b�t�@�̍X�V
			D3D12_SUBRESOURCE_DATA vertexData = {};
			vertexData.pData = &vertices[0];
			vertexData.RowPitch = vertexBufferSize;
			vertexData.SlicePitch = vertexData.RowPitch;

			auto s = UpdateSubresources<1>(
				commandList.Get(),
				m_vertexBuffer.Get(),
				m_vertexBufferUploadHeap.Get(),
				0,
				0,
				1,
				&vertexData
				);
			commandList->ResourceBarrier(
				1,
				&CD3DX12_RESOURCE_BARRIER::Transition(
					m_vertexBuffer.Get(),
					D3D12_RESOURCE_STATE_COPY_DEST,
					D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
				)
			);

			//���_�o�b�t�@�r���[�̍쐬
			m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
			m_vertexBufferView.StrideInBytes = static_cast<UINT>(sizeof(T));
			m_vertexBufferView.SizeInBytes = vertexBufferSize;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	4�p�`���ʂ̍쐬
		@param[in]	size		1�ӂ̃T�C�Y
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateSquare(float size);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�L���[�u�i�����́j�̍쐬
		@param[in]	size	1�ӂ̃T�C�Y
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateCube(float size);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���̂̍쐬
		@param[in]	diameter	���a
		@param[in]	tessellation	������
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateSphere(float diameter, size_t tessellation);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�J�v�Z���̍쐬
		@param[in]	diameter	���a
		@param[in]	height	����
		@param[in]	tessellation	������
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateCapsule(float diameter, float height, size_t tessellation);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�V�����_�[�̍쐬
		@param[in]	height	����
		@param[in]	diameter	���a
		@param[in]	tessellation	������
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateCylinder(float height, float diameter, size_t tessellation);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R�[���̍쐬
		@param[in]	diameter	���a
		@param[in]	height	����
		@param[in]	tessellation	������
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateCone(float diameter, float height, size_t tessellation);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�g�[���X�̍쐬
		@param[in]	diameter	���a
		@param[in]	thickness	�h�[�i�b�c�̑���
		@param[in]	tessellation	������
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateTorus(float diameter, float thickness, size_t tessellation);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	��4�ʑ̂̍쐬
		@param[in]	size	1�ӂ̃T�C�Y
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateTetrahedron(float size);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	��8�ʑ̂̍쐬
		@param[in]	size	1�ӂ̃T�C�Y
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateOctahedron(float size);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	��12�ʑ̂̍쐬
		@param[in]	size	1�ӂ̃T�C�Y
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateDodecahedron(float size);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	��20�ʑ̂̍쐬
		@param[in]	size	1�ӂ̃T�C�Y
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateIcosahedron(float size);


	};


}
//end basecross