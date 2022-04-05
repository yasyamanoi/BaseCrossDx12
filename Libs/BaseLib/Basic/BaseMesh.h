/*!
@file BaseMesh.h
@brief メッシュクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	メッシュクラス
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
		BaseMesh() {}
	public:
		~BaseMesh() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief 頂点バッファの取得
		@return	頂点バッファのComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12Resource> GetVertexBuffer() const {
			return m_vertexBuffer;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief 頂点バッファアップロードヒープの取得
		@return	頂点バッファアップロードヒープのComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12Resource> GetVertexBufferUploadHeap() const {
			return m_vertexBufferUploadHeap;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief インデックスバッファの取得
		@return	インデックスバッファのComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12Resource> GetIndexBuffer() const {
			return m_vertexBuffer;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief インデックスバッファアップロードヒープの取得
		@return	インデックスバッファアップロードヒープのComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12Resource> GetIndexBufferUploadHeap() const {
			return m_indexBufferUploadHeap;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief 頂点バッファの頂点数の取得
		@return	頂点バッファの頂点数
		*/
		//--------------------------------------------------------------------------------------
		UINT GetNumVertices()const {
			return m_numVertices;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief インデックスバッファの数の取得
		@return	インデックスバッファの数
		*/
		//--------------------------------------------------------------------------------------
		UINT GetNumIndices()const {
			return m_numIndices;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief 頂点バッファビューの取得
		@return	頂点バッファビュー
		*/
		//--------------------------------------------------------------------------------------
		const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const {
			return m_vertexBufferView;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief インデックスバッファビューの取得
		@return	インデックスバッファビュー
		*/
		//--------------------------------------------------------------------------------------
		const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const {
			return m_indexBufferView;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief 頂点のみで構成されるメッシュの作成
		@tparam T	作成する頂点の型
		@param[in]	commandList	コマンドリスト
		@param[in]	vertices	頂点の配列
		@return	BaseMeshのComPtr
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		static shared_ptr<BaseMesh> CreateBaseMesh(ComPtr<ID3D12GraphicsCommandList>& commandList,const vector<T>& vertices) {
			//デバイスの取得
			auto device = App::GetID3D12Device();
			auto baseDevice = App::GetBaseDevice();
			shared_ptr<BaseMesh> ptrMesh = shared_ptr<BaseMesh>(new BaseMesh());
			UINT vertexBufferSize = (UINT)(sizeof(T) * vertices.size());
			//頂点バッファの作成
			{
				ThrowIfFailed(device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
					D3D12_RESOURCE_STATE_COPY_DEST,
					nullptr,
					IID_PPV_ARGS(&ptrMesh->m_vertexBuffer)),
					L"頂点バッファ作成に失敗しました。",
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
					L"頂点バッファ作成に失敗しました。",
					L"if(FAILED(device->CreateCommittedResource())",
					L"BaseMesh::CreateBaseMesh()"
				);

				//頂点バッファの更新
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
			//頂点数の設定
			ptrMesh->m_numVertices = static_cast<UINT>(vertices.size());
			return ptrMesh;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief 頂点とインデックスで構成されるメッシュの作成
		@tparam T	作成する頂点の型
		@param[in]	commandList	コマンドリスト
		@param[in]	vertices	頂点の配列
		@param[in]	indices	インデックスの配列
		@return	BaseMeshのComPtr
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		static shared_ptr<BaseMesh> CreateBaseMesh(ComPtr<ID3D12GraphicsCommandList>& commandList,const vector<T>& vertices, const vector<uint32_t>& indices) {
			//デバイスの取得
			auto device = App::GetID3D12Device();
			shared_ptr<BaseMesh> ptrMesh = shared_ptr<BaseMesh>(new BaseMesh());
			UINT vertexBufferSize = (UINT)(sizeof(T) * vertices.size());
			//頂点バッファの作成
			{
				ThrowIfFailed(device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
					D3D12_RESOURCE_STATE_COPY_DEST,
					nullptr,
					IID_PPV_ARGS(&ptrMesh->m_vertexBuffer)),
					L"頂点バッファ作成に失敗しました。",
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
					L"頂点バッファ作成に失敗しました。",
					L"if(FAILED(device->CreateCommittedResource())",
					L"BaseMesh::CreateBaseMesh()"
				);

				//頂点バッファの更新
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

				//頂点バッファビューの作成
				ptrMesh->m_vertexBufferView.BufferLocation = ptrMesh->m_vertexBuffer->GetGPUVirtualAddress();
				ptrMesh->m_vertexBufferView.StrideInBytes = static_cast<UINT>(sizeof(T));
				ptrMesh->m_vertexBufferView.SizeInBytes = vertexBufferSize;
			}
			//頂点数の設定
			ptrMesh->m_numVertices = static_cast<UINT>(vertices.size());
			//インデックスバッファの作成
			UINT indexBufferSize = static_cast<UINT>(sizeof(uint32_t) * indices.size());
			{
				ThrowIfFailed(device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
					D3D12_RESOURCE_STATE_COPY_DEST,
					nullptr,
					IID_PPV_ARGS(&ptrMesh->m_indexBuffer)),
					L"インデックスバッファ作成に失敗しました。",
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
					L"インデックスアップロードバッファ作成に失敗しました。",
					L"if(FAILED(device->CreateCommittedResource())",
					L"BaseMesh::CreateBaseMesh()"
				);

				// インデクスバッファの更新
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


				//インデックスバッファビューの作成
				ptrMesh->m_indexBufferView.BufferLocation = ptrMesh->m_indexBuffer->GetGPUVirtualAddress();
				ptrMesh->m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
				ptrMesh->m_indexBufferView.SizeInBytes = indexBufferSize;

			}
			//インデックス数の設定
			ptrMesh->m_numIndices = static_cast<UINT>(indices.size());
			return ptrMesh;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	頂点の変更.<br />
		AccessWriteがtrueで作成されたリソースは、頂点の配列によって頂点を変更する。
		@tparam	T	頂点の型
		@param[in]	vertices	頂点の配列
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		void UpdateVirtexBuffer(ComPtr<ID3D12GraphicsCommandList>& commandList, const vector<T>& vertices) {
			if (m_numVertices != vertices.size()) {
				throw BaseException(
					L"頂点数が違います。",
					L"BaseMesh::UpdateVirtexBuffer()"
				);
			}
			//デバイスの取得
			auto device = App::GetID3D12Device();
			UINT vertexBufferSize = (UINT)(sizeof(T) * vertices.size());
			//頂点バッファの更新
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

			//頂点バッファビューの作成
			m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
			m_vertexBufferView.StrideInBytes = static_cast<UINT>(sizeof(T));
			m_vertexBufferView.SizeInBytes = vertexBufferSize;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	4角形平面の作成
		@param[in]	size		1辺のサイズ
		@return	メッシュのスマートポインタ
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateSquare(float size);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	キューブ（立方体）の作成
		@param[in]	size	1辺のサイズ
		@return	メッシュのスマートポインタ
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateCube(float size);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	球体の作成
		@param[in]	diameter	直径
		@param[in]	tessellation	分割数
		@return	メッシュのスマートポインタ
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateSphere(float diameter, size_t tessellation);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	カプセルの作成
		@param[in]	diameter	直径
		@param[in]	height	高さ
		@param[in]	tessellation	分割数
		@return	メッシュのスマートポインタ
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateCapsule(float diameter, float height, size_t tessellation);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	シリンダーの作成
		@param[in]	height	高さ
		@param[in]	diameter	直径
		@param[in]	tessellation	分割数
		@return	メッシュのスマートポインタ
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateCylinder(float height, float diameter, size_t tessellation);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コーンの作成
		@param[in]	diameter	直径
		@param[in]	height	高さ
		@param[in]	tessellation	分割数
		@return	メッシュのスマートポインタ
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateCone(float diameter, float height, size_t tessellation);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	トーラスの作成
		@param[in]	diameter	直径
		@param[in]	thickness	ドーナッツの太さ
		@param[in]	tessellation	分割数
		@return	メッシュのスマートポインタ
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateTorus(float diameter, float thickness, size_t tessellation);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	正4面体の作成
		@param[in]	size	1辺のサイズ
		@return	メッシュのスマートポインタ
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateTetrahedron(float size);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	正8面体の作成
		@param[in]	size	1辺のサイズ
		@return	メッシュのスマートポインタ
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateOctahedron(float size);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	正12面体の作成
		@param[in]	size	1辺のサイズ
		@return	メッシュのスマートポインタ
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateDodecahedron(float size);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	正20面体の作成
		@param[in]	size	1辺のサイズ
		@return	メッシュのスマートポインタ
		*/
		//--------------------------------------------------------------------------------------
		static shared_ptr<BaseMesh> CreateIcosahedron(float size);


	};


}
//end basecross
