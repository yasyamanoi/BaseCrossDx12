/*!
@file BaseMesh.h
@brief ���b�V���N���X
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	�A�j���[�V����
	//--------------------------------------------------------------------------------------
	class Animation {
		const aiScene* m_pScene;
		const aiAnimation* m_pAnimation;
		const std::string m_nodeName;
		const aiNodeAnim* m_rootNode;
		bool m_hasAnimation;
		//--------------------------------------------------------------------------------------
		/*!
		@brief �A�j���[�V�������ċA�I�ɍ쐬����
		@param[in]	AnimationTime	�A�j���^�C��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const bsm::Mat4x4& ParentTransform);
	public:
		Animation(const aiScene* pScene,const aiAnimation* pAnimation, const std::string& nodeName);
		bool CreateAnimation();
		bool HasAnimation() const {
			return m_hasAnimation;
		}
	};


	//--------------------------------------------------------------------------------------
	///	���b�V��
	//--------------------------------------------------------------------------------------
	class BaseMesh {
		ComPtr<ID3D12Resource> m_vertexBuffer; //���_�o�b�t�@
		ComPtr<ID3D12Resource> m_indexBuffer; //�C���f�b�N�X�o�b�t�@
		ComPtr<ID3D12Resource> m_vertexBufferUploadHeap; //���_�o�b�t�@�̃A�b�v���[�h�q�[�v
		ComPtr<ID3D12Resource> m_indexBufferUploadHeap; //�C���f�b�N�X�o�b�t�@�̃A�b�v���[�h�q�[�v

		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView; //���_�o�b�t�@�r���[
		D3D12_INDEX_BUFFER_VIEW m_indexBufferView; //�C���f�b�N�X�o�b�t�@�r���[
		UINT m_numVertices; //���_��
		UINT m_numIndices; //�C���f�b�N�X��

		static Assimp::Importer m_importer;
		static const aiScene* m_pScene;
		std::vector<std::shared_ptr<Animation>> m_AnimationVec;
	protected:
		BaseMesh() {}
	public:
		~BaseMesh() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���_�o�b�t�@�̎擾
		@return	���\�[�X��ComPt
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12Resource> GetVertexBuffer() const {
			return m_vertexBuffer;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���_�o�b�t�@�A�b�v���[�h�q�[�v�̎擾
		@return	���\�[�X��ComPt
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12Resource> GetVertexBufferUploadHeap() const {
			return m_vertexBufferUploadHeap;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�C���f�b�N�X�o�b�t�@�̎擾
		@return	���\�[�X��ComPt
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12Resource> GetIndexBuffer() const {
			return m_vertexBuffer;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�C���f�b�N�X�o�b�t�@�A�b�v���[�h�q�[�v�̎擾
		@return	���\�[�X��ComPt
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12Resource> GetIndexBufferUploadHeap() const {
			return m_indexBufferUploadHeap;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���_�o�b�t�@�̒��_���̎擾
		@return	���_�o�b�t�@�̒��_��
		*/
		//--------------------------------------------------------------------------------------
		UINT GetNumVertices()const {
			return m_numVertices;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�C���f�b�N�X�o�b�t�@�̐��̎擾
		@return	�C���f�b�N�X�o�b�t�@�̐�
		*/
		//--------------------------------------------------------------------------------------
		UINT GetNumIndices()const {
			return m_numIndices;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���_�o�b�t�@�r���[�̎擾
		@return	���_�o�b�t�@�r���[�̎Q��
		*/
		//--------------------------------------------------------------------------------------
		const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const {
			return m_vertexBufferView;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�C���f�b�N�X�o�b�t�@�r���[�̎擾
		@return	�C���f�b�N�X�o�b�t�@�r���[�̎Q��
		*/
		//--------------------------------------------------------------------------------------
		const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const {
			return m_indexBufferView;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief ���_�݂̂ō\������郁�b�V���̍쐬
		@tparam T	�쐬���钸�_�̌^
		@param[in]	pCommandList	�R�}���h���X�g
		@param[in]	vertices	���_�̔z��
		@return	BaseMesh��shared_ptr
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		static std::shared_ptr<BaseMesh> CreateBaseMesh(ID3D12GraphicsCommandList* pCommandList,const std::vector<T>& vertices) {
			//�f�o�C�X�̎擾
			auto device = App::GetD3D12Device();
			std::shared_ptr<BaseMesh> ptrMesh = std::shared_ptr<BaseMesh>(new BaseMesh());
			UINT vertexBufferSize = (UINT)(sizeof(T) * vertices.size());
			//���_�o�b�t�@�̍쐬
			{
				ThrowIfFailedEx(device->CreateCommittedResource(
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
				ThrowIfFailedEx(device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
					D3D12_RESOURCE_STATE_COMMON,
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
					pCommandList,
					ptrMesh->m_vertexBuffer.Get(),
					ptrMesh->m_vertexBufferUploadHeap.Get(),
					0, 
					0, 
					1, 
					&vertexData
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
		@param[in]	pCommandList	�R�}���h���X�g
		@param[in]	vertices	���_�̔z��
		@param[in]	indices	�C���f�b�N�X�̔z��
		@return	BaseMesh��shared_ptr
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		static std::shared_ptr<BaseMesh> CreateBaseMesh(ID3D12GraphicsCommandList* pCommandList,const std::vector<T>& vertices, const std::vector<uint32_t>& indices) {
			//�f�o�C�X�̎擾
			auto device = App::GetD3D12Device();
			std::shared_ptr<BaseMesh> ptrMesh = std::shared_ptr<BaseMesh>(new BaseMesh());
			UINT vertexBufferSize = (UINT)(sizeof(T) * vertices.size());
			//���_�o�b�t�@�̍쐬
			{
				ThrowIfFailedEx(device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
					D3D12_RESOURCE_STATE_COMMON,
					nullptr,
					IID_PPV_ARGS(&ptrMesh->m_vertexBuffer)),
					L"���_�o�b�t�@�쐬�Ɏ��s���܂����B",
					L"if(FAILED(device->CreateCommittedResource())",
					L"BaseMesh::CreateBaseMesh()"
				);
				ThrowIfFailedEx(device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
					D3D12_RESOURCE_STATE_COMMON,
					nullptr,
					IID_PPV_ARGS(&ptrMesh->m_vertexBufferUploadHeap)),
					L"���_�o�b�t�@�A�b�v���[�h�q�[�v�쐬�Ɏ��s���܂����B",
					L"if(FAILED(device->CreateCommittedResource())",
					L"BaseMesh::CreateBaseMesh()"
				);

				//���_�o�b�t�@�̍X�V
				D3D12_SUBRESOURCE_DATA vertexData = {};
				vertexData.pData = &vertices[0];
				vertexData.RowPitch = vertexBufferSize;
				vertexData.SlicePitch = vertexData.RowPitch;

				UpdateSubresources<1>(
					pCommandList,
					ptrMesh->m_vertexBuffer.Get(),
					ptrMesh->m_vertexBufferUploadHeap.Get(),
					0,
					0,
					1,
					&vertexData
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
				ThrowIfFailedEx(device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
					D3D12_RESOURCE_STATE_COMMON,
					nullptr,
					IID_PPV_ARGS(&ptrMesh->m_indexBuffer)),
					L"�C���f�b�N�X�o�b�t�@�쐬�Ɏ��s���܂����B",
					L"if(FAILED(device->CreateCommittedResource())",
					L"BaseMesh::CreateBaseMesh()"
				);
				ThrowIfFailedEx(device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
					D3D12_RESOURCE_STATE_COMMON,
					nullptr,
					IID_PPV_ARGS(&ptrMesh->m_indexBufferUploadHeap)),
					L"�C���f�b�N�X�o�b�t�@�̃A�b�v���[�h�q�[�v�쐬�Ɏ��s���܂����B",
					L"if(FAILED(device->CreateCommittedResource())",
					L"BaseMesh::CreateBaseMesh()"
				);
				// �C���f�N�X�o�b�t�@�̍X�V
				D3D12_SUBRESOURCE_DATA indexData = {};
				indexData.pData = (void*)&indices[0];
				indexData.RowPitch = indexBufferSize;
				indexData.SlicePitch = indexData.RowPitch;
				UpdateSubresources<1>(
					pCommandList,
					ptrMesh->m_indexBuffer.Get(), 
					ptrMesh->m_indexBufferUploadHeap.Get(), 
					0, 0, 1, &indexData
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
		@brief ���_�ƃC���f�b�N�X�ō\�������{�[�����f�����b�V���̍쐬
		@param[in]	pCommandList	�R�}���h���X�g
		@param[in]	dataDir	�f�[�^�f�B���N�g��
		@param[in]	dataFile�@�f�[�^�t�@�C����
		@return	BaseMesh��shared_ptr
		*/
		//--------------------------------------------------------------------------------------
		static std::shared_ptr<BaseMesh> CreateBoneModelMesh(
			ID3D12GraphicsCommandList* pCommandList,
			const std::wstring& dataDir, const std::wstring& dataFile);
		//--------------------------------------------------------------------------------------
		/*!
		@brief �A�j���[�V�����̍쐬
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void CreateAnimations();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���_�̕ύX.<br />
		AccessWrite��true�ō쐬���ꂽ���\�[�X�́A���_�̔z��ɂ���Ē��_��ύX����B
		@tparam	T	���_�̌^
		@param[in]	pCommandList	�R�}���h���X�g
		@param[in]	vertices	���_�̔z��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		void UpdateVirtexBuffer(ID3D12GraphicsCommandList* pCommandList, const std::vector<T>& vertices) {
			if (m_numVertices != vertices.size()) {
				throw BaseException(
					L"���_�����Ⴂ�܂��B",
					L"BaseMesh::UpdateVirtexBuffer()"
				);
			}
			//�f�o�C�X�̎擾
			auto device = App::GetD3D12Device();
			UINT vertexBufferSize = (UINT)(sizeof(T) * vertices.size());
			//���_�o�b�t�@�̍X�V
			D3D12_SUBRESOURCE_DATA vertexData = {};
			vertexData.pData = &vertices[0];
			vertexData.RowPitch = vertexBufferSize;
			vertexData.SlicePitch = vertexData.RowPitch;

			auto s = UpdateSubresources<1>(
				pCommandList,
				m_vertexBuffer.Get(),
				m_vertexBufferUploadHeap.Get(),
				0,
				0,
				1,
				&vertexData
				);
			//�ȉ��K�v���ǂ������ؕK�v
			//pCommandList->ResourceBarrier(
			//	1,
			//	&CD3DX12_RESOURCE_BARRIER::Transition(
			//		m_vertexBuffer.Get(),
			//		D3D12_RESOURCE_STATE_COPY_DEST,
			//		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
			//	)
			//);

			//���_�o�b�t�@�r���[�̍쐬
			m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
			m_vertexBufferView.StrideInBytes = static_cast<UINT>(sizeof(T));
			m_vertexBufferView.SizeInBytes = vertexBufferSize;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	4�p�`���ʂ̍쐬
		@param[in]	pCommandList	�R�}���h���X�g
		@param[in]	size		1�ӂ̃T�C�Y
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static std::shared_ptr<BaseMesh> CreateSquare(ID3D12GraphicsCommandList* pCommandList,float size);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�L���[�u�i�����́j�̍쐬
		@param[in]	pCommandList	�R�}���h���X�g
		@param[in]	size	1�ӂ̃T�C�Y
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static std::shared_ptr<BaseMesh> CreateCube(ID3D12GraphicsCommandList* pCommandList, float size);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���̂̍쐬
		@param[in]	pCommandList	�R�}���h���X�g
		@param[in]	diameter	���a
		@param[in]	tessellation	������
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static std::shared_ptr<BaseMesh> CreateSphere(ID3D12GraphicsCommandList* pCommandList, float diameter, size_t tessellation);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�J�v�Z���̍쐬
		@param[in]	pCommandList	�R�}���h���X�g
		@param[in]	diameter	���a
		@param[in]	height	����
		@param[in]	tessellation	������
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static std::shared_ptr<BaseMesh> CreateCapsule(ID3D12GraphicsCommandList* pCommandList, float diameter, float height, size_t tessellation);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�V�����_�[�̍쐬
		@param[in]	pCommandList	�R�}���h���X�g
		@param[in]	height	����
		@param[in]	diameter	���a
		@param[in]	tessellation	������
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static std::shared_ptr<BaseMesh> CreateCylinder(ID3D12GraphicsCommandList* pCommandList, float height, float diameter, size_t tessellation);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R�[���̍쐬
		@param[in]	pCommandList	�R�}���h���X�g
		@param[in]	diameter	���a
		@param[in]	height	����
		@param[in]	tessellation	������
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static std::shared_ptr<BaseMesh> CreateCone(ID3D12GraphicsCommandList* pCommandList, float diameter, float height, size_t tessellation);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�g�[���X�̍쐬
		@param[in]	pCommandList	�R�}���h���X�g
		@param[in]	diameter	���a
		@param[in]	thickness	�h�[�i�b�c�̑���
		@param[in]	tessellation	������
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static std::shared_ptr<BaseMesh> CreateTorus(ID3D12GraphicsCommandList* pCommandList, float diameter, float thickness, size_t tessellation);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	��4�ʑ̂̍쐬
		@param[in]	pCommandList	�R�}���h���X�g
		@param[in]	size	1�ӂ̃T�C�Y
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static std::shared_ptr<BaseMesh> CreateTetrahedron(ID3D12GraphicsCommandList* pCommandList, float size);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	��8�ʑ̂̍쐬
		@param[in]	pCommandList	�R�}���h���X�g
		@param[in]	size	1�ӂ̃T�C�Y
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static std::shared_ptr<BaseMesh> CreateOctahedron(ID3D12GraphicsCommandList* pCommandList, float size);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	��12�ʑ̂̍쐬
		@param[in]	pCommandList	�R�}���h���X�g
		@param[in]	size	1�ӂ̃T�C�Y
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static std::shared_ptr<BaseMesh> CreateDodecahedron(ID3D12GraphicsCommandList* pCommandList, float size);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	��20�ʑ̂̍쐬
		@param[in]	pCommandList	�R�}���h���X�g
		@param[in]	size	1�ӂ̃T�C�Y
		@return	���b�V���̃X�}�[�g�|�C���^
		*/
		//--------------------------------------------------------------------------------------
		static std::shared_ptr<BaseMesh> CreateIcosahedron(ID3D12GraphicsCommandList* pCommandList, float size);


	};
}
//end basecross
