/*!
@file BaseMesh.cpp
@brief メッシュクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#include "stdafx.h"

namespace basecross {

	std::shared_ptr<BaseMesh> BaseMesh::CreateSquare(ID3D12GraphicsCommandList* pCommandList, float size) {
		std::vector<VertexPositionNormalTexture> vertices;
		std::vector<uint32_t> indices;
		MeshUtill::CreateSquare(size, vertices, indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(pCommandList, vertices, indices);
	}

	std::shared_ptr<BaseMesh> BaseMesh::CreateCube(ID3D12GraphicsCommandList* pCommandList, float size) {
		std::vector<VertexPositionNormalTexture> vertices;
		std::vector<uint32_t> indices;
		MeshUtill::CreateCube(size, vertices, indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(pCommandList, vertices, indices);
	}

	std::shared_ptr<BaseMesh> BaseMesh::CreateSphere(ID3D12GraphicsCommandList* pCommandList, float diameter, size_t tessellation) {
		std::vector<VertexPositionNormalTexture> vertices;
		std::vector<uint32_t> indices;
		MeshUtill::CreateSphere(diameter, tessellation, vertices, indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(pCommandList, vertices, indices);
	}

	std::shared_ptr<BaseMesh> BaseMesh::CreateCapsule(ID3D12GraphicsCommandList* pCommandList, float diameter, float height, size_t tessellation) {
		std::vector<VertexPositionNormalTexture> vertices;
		std::vector<uint32_t> indices;
		bsm::Vec3 pointA(0, -height / 2.0f, 0);
		bsm::Vec3 pointB(0, height / 2.0f, 0);
		//Capsuleの作成(ヘルパー関数を利用)
		MeshUtill::CreateCapsule(diameter, pointA, pointB, tessellation, vertices, indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(pCommandList, vertices, indices);
	}

	std::shared_ptr<BaseMesh> BaseMesh::CreateCylinder(ID3D12GraphicsCommandList* pCommandList, float height, float diameter, size_t tessellation) {
		std::vector<VertexPositionNormalTexture> vertices;
		std::vector<uint32_t> indices;
		MeshUtill::CreateCylinder(height, diameter, tessellation, vertices, indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(pCommandList, vertices, indices);
	}

	std::shared_ptr<BaseMesh> BaseMesh::CreateCone(ID3D12GraphicsCommandList* pCommandList, float diameter, float height, size_t tessellation) {
		std::vector<VertexPositionNormalTexture> vertices;
		std::vector<uint32_t> indices;
		MeshUtill::CreateCone(diameter,height,tessellation,vertices, indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(pCommandList, vertices, indices);
	}

	std::shared_ptr<BaseMesh> BaseMesh::CreateTorus(ID3D12GraphicsCommandList* pCommandList, float diameter, float thickness, size_t tessellation) {
		std::vector<VertexPositionNormalTexture> vertices;
		std::vector<uint32_t> indices;
		MeshUtill::CreateTorus( diameter, thickness, tessellation,vertices,indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(pCommandList, vertices, indices);
	}

	std::shared_ptr<BaseMesh> BaseMesh::CreateTetrahedron(ID3D12GraphicsCommandList* pCommandList, float size) {
		std::vector<VertexPositionNormalTexture> vertices;
		std::vector<uint32_t> indices;
		MeshUtill::CreateTetrahedron(size, vertices, indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(pCommandList, vertices, indices);
	}

	std::shared_ptr<BaseMesh> BaseMesh::CreateOctahedron(ID3D12GraphicsCommandList* pCommandList, float size) {
		std::vector<VertexPositionNormalTexture> vertices;
		std::vector<uint32_t> indices;
		MeshUtill::CreateOctahedron(size, vertices, indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(pCommandList, vertices, indices);
	}

	std::shared_ptr<BaseMesh> BaseMesh::CreateDodecahedron(ID3D12GraphicsCommandList* pCommandList, float size) {
		std::vector<VertexPositionNormalTexture> vertices;
		std::vector<uint32_t> indices;
		MeshUtill::CreateDodecahedron(size, vertices, indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(pCommandList, vertices, indices);
	}

	std::shared_ptr<BaseMesh> BaseMesh::CreateIcosahedron(ID3D12GraphicsCommandList* pCommandList, float size) {
		std::vector<VertexPositionNormalTexture> vertices;
		std::vector<uint32_t> indices;
		MeshUtill::CreateIcosahedron(size, vertices, indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(pCommandList, vertices, indices);
	}

}
// end namespace basecross
