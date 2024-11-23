/*!
@file BaseMesh.cpp
@brief メッシュクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#include "stdafx.h"

namespace basecross {

	shared_ptr<BaseMesh> BaseMesh::CreateSquare(float size) {
		auto pDevice = App::GetBaseDevice();
		auto commandList = pDevice->GetComandList();
		vector<VertexPositionNormalTexture> vertices;
		vector<uint32_t> indices;
		MeshUtill::CreateSquare(size, vertices, indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(commandList, vertices, indices);
	}

	shared_ptr<BaseMesh> BaseMesh::CreateCube(float size) {
		auto pDevice = App::GetBaseDevice();
		auto commandList = pDevice->GetComandList();
		vector<VertexPositionNormalTexture> vertices;
		vector<uint32_t> indices;
		MeshUtill::CreateCube(size, vertices, indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(commandList, vertices, indices);
	}

	shared_ptr<BaseMesh> BaseMesh::CreateSphere(float diameter, size_t tessellation) {
		auto pDevice = App::GetBaseDevice();
		auto commandList = pDevice->GetComandList();
		vector<VertexPositionNormalTexture> vertices;
		vector<uint32_t> indices;
		MeshUtill::CreateSphere(diameter, tessellation, vertices, indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(commandList, vertices, indices);
	}

	shared_ptr<BaseMesh> BaseMesh::CreateCapsule(float diameter, float height, size_t tessellation) {
		auto pDevice = App::GetBaseDevice();
		auto commandList = pDevice->GetComandList();
		vector<VertexPositionNormalTexture> vertices;
		vector<uint32_t> indices;
		Vec3 pointA(0, -height / 2.0f, 0);
		Vec3 pointB(0, height / 2.0f, 0);
		//Capsuleの作成(ヘルパー関数を利用)
		MeshUtill::CreateCapsule(diameter, pointA, pointB, tessellation, vertices, indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(commandList, vertices, indices);
	}

	shared_ptr<BaseMesh> BaseMesh::CreateCylinder(float height, float diameter, size_t tessellation) {
		auto pDevice = App::GetBaseDevice();
		auto commandList = pDevice->GetComandList();
		vector<VertexPositionNormalTexture> vertices;
		vector<uint32_t> indices;
		MeshUtill::CreateCylinder(height, diameter, tessellation, vertices, indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(commandList, vertices, indices);
	}

	shared_ptr<BaseMesh> BaseMesh::CreateCone(float diameter, float height, size_t tessellation) {
		auto pDevice = App::GetBaseDevice();
		auto commandList = pDevice->GetComandList();
		vector<VertexPositionNormalTexture> vertices;
		vector<uint32_t> indices;
		MeshUtill::CreateCone(diameter,height,tessellation,vertices, indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(commandList, vertices, indices);
	}

	shared_ptr<BaseMesh> BaseMesh::CreateTorus(float diameter, float thickness, size_t tessellation) {
		auto pDevice = App::GetBaseDevice();
		auto commandList = pDevice->GetComandList();
		vector<VertexPositionNormalTexture> vertices;
		vector<uint32_t> indices;
		MeshUtill::CreateTorus( diameter, thickness, tessellation,vertices,indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(commandList, vertices, indices);
	}

	shared_ptr<BaseMesh> BaseMesh::CreateTetrahedron(float size) {
		auto pDevice = App::GetBaseDevice();
		auto commandList = pDevice->GetComandList();
		vector<VertexPositionNormalTexture> vertices;
		vector<uint32_t> indices;
		MeshUtill::CreateTetrahedron(size, vertices, indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(commandList, vertices, indices);
	}

	shared_ptr<BaseMesh> BaseMesh::CreateOctahedron(float size) {
		auto pDevice = App::GetBaseDevice();
		auto commandList = pDevice->GetComandList();
		vector<VertexPositionNormalTexture> vertices;
		vector<uint32_t> indices;
		MeshUtill::CreateOctahedron(size, vertices, indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(commandList, vertices, indices);
	}

	shared_ptr<BaseMesh> BaseMesh::CreateDodecahedron(float size) {
		auto pDevice = App::GetBaseDevice();
		auto commandList = pDevice->GetComandList();
		vector<VertexPositionNormalTexture> vertices;
		vector<uint32_t> indices;
		MeshUtill::CreateDodecahedron(size, vertices, indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(commandList, vertices, indices);
	}

	shared_ptr<BaseMesh> BaseMesh::CreateIcosahedron(float size) {
		auto pDevice = App::GetBaseDevice();
		auto commandList = pDevice->GetComandList();
		vector<VertexPositionNormalTexture> vertices;
		vector<uint32_t> indices;
		MeshUtill::CreateIcosahedron(size, vertices, indices);
		return BaseMesh::CreateBaseMesh<VertexPositionNormalTexture>(commandList, vertices, indices);
	}

}
// end namespace basecross
