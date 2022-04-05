/*!
@file BaseScene.cpp
@brief シーンの親クラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/
#include "stdafx.h"

namespace basecross {


	void BaseScene::ConvertVertex(const vector<VertexPositionNormalTexture>& vertices,
		vector<VertexPositionColor>& new_pc_vertices,
		vector<VertexPositionNormal>& new_pn_vertices,
		vector<VertexPositionTexture>& new_pt_vertices,
		vector<VertexPositionNormalTangentTexture>& new_pntnt_vertices
	) {
		new_pc_vertices.clear();
		new_pn_vertices.clear();
		new_pt_vertices.clear();
		new_pntnt_vertices.clear();
		for (size_t i = 0; i < vertices.size(); i++) {
			VertexPositionColor new_pc_v;
			VertexPositionNormal new_pn_v;
			VertexPositionTexture new_pt_v;
			VertexPositionNormalTangentTexture new_pntnt_v;

			new_pc_v.position = vertices[i].position;
			new_pc_v.color = Col4(1.0f, 1.0f, 1.0f, 1.0f);

			new_pn_v.position = vertices[i].position;
			new_pn_v.normal = vertices[i].normal;

			new_pt_v.position = vertices[i].position;
			new_pt_v.textureCoordinate = vertices[i].textureCoordinate;

			new_pntnt_v.position = vertices[i].position;
			new_pntnt_v.normal = vertices[i].normal;
			new_pntnt_v.textureCoordinate = vertices[i].textureCoordinate;
			Vec3 n = bsm::cross((Vec3)new_pntnt_v.normal, Vec3(0, 1, 0));
			new_pntnt_v.tangent = Vec4(n.x, n.y, n.z, 0.0f);
			new_pntnt_v.tangent.w = 0.0f;

			new_pc_vertices.push_back(new_pc_v);
			new_pn_vertices.push_back(new_pn_v);
			new_pt_vertices.push_back(new_pt_v);
			new_pntnt_vertices.push_back(new_pntnt_v);

		}


	}


	void BaseScene::CreateDefaultMeshes() {
		//デフォルトのメッシュの登録
		AddMesh(L"DEFAULT_SQUARE", BaseMesh::CreateSquare(1.0f));
		AddMesh(L"DEFAULT_CUBE", BaseMesh::CreateCube(1.0f));
		AddMesh(L"DEFAULT_SPHERE", BaseMesh::CreateSphere(1.0f, 18));
		AddMesh(L"DEFAULT_CAPSULE", BaseMesh::CreateCapsule(1.0f, 1.0f, 18));
		AddMesh(L"DEFAULT_CYLINDER", BaseMesh::CreateCylinder(1.0f, 1.0f, 18));
		AddMesh(L"DEFAULT_CONE", BaseMesh::CreateCone(1.0f, 1.0f, 18));
		AddMesh(L"DEFAULT_TORUS", BaseMesh::CreateTorus(1.0f, 0.3f, 18));
		AddMesh(L"DEFAULT_TETRAHEDRON", BaseMesh::CreateTetrahedron(1.0f));
		AddMesh(L"DEFAULT_OCTAHEDRON", BaseMesh::CreateOctahedron(1.0f));
		AddMesh(L"DEFAULT_DODECAHEDRON", BaseMesh::CreateDodecahedron(1.0f));
		AddMesh(L"DEFAULT_ICOSAHEDRON", BaseMesh::CreateIcosahedron(1.0f));

		vector<VertexPositionNormalTexture> vertices;
		vector<VertexPositionColor> new_pc_vertices;
		vector<VertexPositionNormal> new_pn_vertices;
		vector<VertexPositionTexture> new_pt_vertices;
		vector<VertexPositionNormalTangentTexture> new_pntnt_vertices;

		vector<uint32_t> indices;

		auto pDevice = App::GetBaseDevice();
		auto commandList = pDevice->GetComandList();

		MeshUtill::CreateSquare(1.0f, vertices, indices);
		ConvertVertex(vertices, new_pc_vertices, new_pn_vertices, new_pt_vertices, new_pntnt_vertices);
		MeshUtill::SetNormalTangent(new_pntnt_vertices);
		AddMesh(L"DEFAULT_PC_SQUARE", BaseMesh::CreateBaseMesh<VertexPositionColor>(commandList, new_pc_vertices, indices));
		AddMesh(L"DEFAULT_PN_SQUARE", BaseMesh::CreateBaseMesh<VertexPositionNormal>(commandList, new_pn_vertices, indices));
		AddMesh(L"DEFAULT_PT_SQUARE", BaseMesh::CreateBaseMesh<VertexPositionTexture>(commandList, new_pt_vertices, indices));
		AddMesh(L"DEFAULT_PNTnT_SQUARE", BaseMesh::CreateBaseMesh<VertexPositionNormalTangentTexture>(commandList, new_pntnt_vertices, indices));

		vertices.clear();
		indices.clear();
		MeshUtill::CreateCube(1.0f, vertices, indices);
		ConvertVertex(vertices, new_pc_vertices, new_pn_vertices, new_pt_vertices, new_pntnt_vertices);
		MeshUtill::SetNormalTangent(new_pntnt_vertices);
		AddMesh(L"DEFAULT_PC_CUBE", BaseMesh::CreateBaseMesh<VertexPositionColor>(commandList, new_pc_vertices, indices));
		AddMesh(L"DEFAULT_PN_CUBE", BaseMesh::CreateBaseMesh<VertexPositionNormal>(commandList, new_pn_vertices, indices));
		AddMesh(L"DEFAULT_PT_CUBE", BaseMesh::CreateBaseMesh<VertexPositionTexture>(commandList, new_pt_vertices, indices));
		AddMesh(L"DEFAULT_PNTnT_CUBE", BaseMesh::CreateBaseMesh<VertexPositionNormalTangentTexture>(commandList, new_pntnt_vertices, indices));

		vertices.clear();
		indices.clear();
		MeshUtill::CreateSphere(1.0f, 18, vertices, indices);
		ConvertVertex(vertices, new_pc_vertices, new_pn_vertices, new_pt_vertices, new_pntnt_vertices);
		MeshUtill::SetNormalTangent(new_pntnt_vertices);
		AddMesh(L"DEFAULT_PC_SPHERE", BaseMesh::CreateBaseMesh<VertexPositionColor>(commandList, new_pc_vertices, indices));
		AddMesh(L"DEFAULT_PN_SPHERE", BaseMesh::CreateBaseMesh<VertexPositionNormal>(commandList, new_pn_vertices, indices));
		AddMesh(L"DEFAULT_PT_SPHERE", BaseMesh::CreateBaseMesh<VertexPositionTexture>(commandList, new_pt_vertices, indices));
		AddMesh(L"DEFAULT_PNTnT_SPHERE", BaseMesh::CreateBaseMesh<VertexPositionNormalTangentTexture>(commandList, new_pntnt_vertices, indices));

		vertices.clear();
		indices.clear();
		Vec3 PointA(0, -1.0f / 2.0f, 0);
		Vec3 PointB(0, 1.0f / 2.0f, 0);
		//Capsuleの作成(ヘルパー関数を利用)
		MeshUtill::CreateCapsule(1.0f, PointA, PointB, 18, vertices, indices);
		ConvertVertex(vertices, new_pc_vertices, new_pn_vertices, new_pt_vertices, new_pntnt_vertices);
		MeshUtill::SetNormalTangent(new_pntnt_vertices);
		AddMesh(L"DEFAULT_PC_CAPSULE", BaseMesh::CreateBaseMesh<VertexPositionColor>(commandList, new_pc_vertices, indices));
		AddMesh(L"DEFAULT_PN_CAPSULE", BaseMesh::CreateBaseMesh<VertexPositionNormal>(commandList, new_pn_vertices, indices));
		AddMesh(L"DEFAULT_PT_CAPSULE", BaseMesh::CreateBaseMesh<VertexPositionTexture>(commandList, new_pt_vertices, indices));
		AddMesh(L"DEFAULT_PNTnT_CAPSULE", BaseMesh::CreateBaseMesh<VertexPositionNormalTangentTexture>(commandList, new_pntnt_vertices, indices));

		vertices.clear();
		indices.clear();
		MeshUtill::CreateCylinder(1.0f, 1.0f, 18, vertices, indices);
		ConvertVertex(vertices, new_pc_vertices, new_pn_vertices, new_pt_vertices, new_pntnt_vertices);
		MeshUtill::SetNormalTangent(new_pntnt_vertices);
		AddMesh(L"DEFAULT_PC_CYLINDER", BaseMesh::CreateBaseMesh<VertexPositionColor>(commandList, new_pc_vertices, indices));
		AddMesh(L"DEFAULT_PN_CYLINDER", BaseMesh::CreateBaseMesh<VertexPositionNormal>(commandList, new_pn_vertices, indices));
		AddMesh(L"DEFAULT_PT_CYLINDER", BaseMesh::CreateBaseMesh<VertexPositionTexture>(commandList, new_pt_vertices, indices));
		AddMesh(L"DEFAULT_PNTnT_CYLINDER", BaseMesh::CreateBaseMesh<VertexPositionNormalTangentTexture>(commandList, new_pntnt_vertices, indices));

		vertices.clear();
		indices.clear();
		MeshUtill::CreateCone(1.0f, 1.0f, 18, vertices, indices);
		ConvertVertex(vertices, new_pc_vertices, new_pn_vertices, new_pt_vertices, new_pntnt_vertices);
		MeshUtill::SetNormalTangent(new_pntnt_vertices);
		AddMesh(L"DEFAULT_PC_CONE", BaseMesh::CreateBaseMesh<VertexPositionColor>(commandList, new_pc_vertices, indices));
		AddMesh(L"DEFAULT_PN_CONE", BaseMesh::CreateBaseMesh<VertexPositionNormal>(commandList, new_pn_vertices, indices));
		AddMesh(L"DEFAULT_PT_CONE", BaseMesh::CreateBaseMesh<VertexPositionTexture>(commandList, new_pt_vertices, indices));
		AddMesh(L"DEFAULT_PNTnT_CONE", BaseMesh::CreateBaseMesh<VertexPositionNormalTangentTexture>(commandList, new_pntnt_vertices, indices));

		vertices.clear();
		indices.clear();
		MeshUtill::CreateTorus(1.0f, 0.3f, 18, vertices, indices);
		ConvertVertex(vertices, new_pc_vertices, new_pn_vertices, new_pt_vertices, new_pntnt_vertices);
		MeshUtill::SetNormalTangent(new_pntnt_vertices);
		AddMesh(L"DEFAULT_PC_TORUS", BaseMesh::CreateBaseMesh<VertexPositionColor>(commandList, new_pc_vertices, indices));
		AddMesh(L"DEFAULT_PN_TORUS", BaseMesh::CreateBaseMesh<VertexPositionNormal>(commandList, new_pn_vertices, indices));
		AddMesh(L"DEFAULT_PT_TORUS", BaseMesh::CreateBaseMesh<VertexPositionTexture>(commandList, new_pt_vertices, indices));
		AddMesh(L"DEFAULT_PNTnT_TORUS", BaseMesh::CreateBaseMesh<VertexPositionNormalTangentTexture>(commandList, new_pntnt_vertices, indices));

		vertices.clear();
		indices.clear();
		MeshUtill::CreateTetrahedron(1.0f, vertices, indices);
		ConvertVertex(vertices, new_pc_vertices, new_pn_vertices, new_pt_vertices, new_pntnt_vertices);
		MeshUtill::SetNormalTangent(new_pntnt_vertices);
		AddMesh(L"DEFAULT_PC_TETRAHEDRON", BaseMesh::CreateBaseMesh<VertexPositionColor>(commandList, new_pc_vertices, indices));
		AddMesh(L"DEFAULT_PN_TETRAHEDRON", BaseMesh::CreateBaseMesh<VertexPositionNormal>(commandList, new_pn_vertices, indices));
		AddMesh(L"DEFAULT_PT_TETRAHEDRON", BaseMesh::CreateBaseMesh<VertexPositionTexture>(commandList, new_pt_vertices, indices));
		AddMesh(L"DEFAULT_PNTnT_TETRAHEDRON", BaseMesh::CreateBaseMesh<VertexPositionNormalTangentTexture>(commandList, new_pntnt_vertices, indices));

		vertices.clear();
		indices.clear();
		MeshUtill::CreateOctahedron(1.0f, vertices, indices);
		ConvertVertex(vertices, new_pc_vertices, new_pn_vertices, new_pt_vertices, new_pntnt_vertices);
		MeshUtill::SetNormalTangent(new_pntnt_vertices);
		AddMesh(L"DEFAULT_PC_OCTAHEDRON", BaseMesh::CreateBaseMesh<VertexPositionColor>(commandList, new_pc_vertices, indices));
		AddMesh(L"DEFAULT_PN_OCTAHEDRON", BaseMesh::CreateBaseMesh<VertexPositionNormal>(commandList, new_pn_vertices, indices));
		AddMesh(L"DEFAULT_PT_OCTAHEDRON", BaseMesh::CreateBaseMesh<VertexPositionTexture>(commandList, new_pt_vertices, indices));
		AddMesh(L"DEFAULT_PNTnT_OCTAHEDRON", BaseMesh::CreateBaseMesh<VertexPositionNormalTangentTexture>(commandList, new_pntnt_vertices, indices));

		vertices.clear();
		indices.clear();
		MeshUtill::CreateDodecahedron(1.0f, vertices, indices);
		ConvertVertex(vertices, new_pc_vertices, new_pn_vertices, new_pt_vertices, new_pntnt_vertices);
		MeshUtill::SetNormalTangent(new_pntnt_vertices);
		AddMesh(L"DEFAULT_PC_DODECAHEDRON", BaseMesh::CreateBaseMesh<VertexPositionColor>(commandList, new_pc_vertices, indices));
		AddMesh(L"DEFAULT_PN_DODECAHEDRON", BaseMesh::CreateBaseMesh<VertexPositionNormal>(commandList, new_pn_vertices, indices));
		AddMesh(L"DEFAULT_PT_DODECAHEDRON", BaseMesh::CreateBaseMesh<VertexPositionTexture>(commandList, new_pt_vertices, indices));
		AddMesh(L"DEFAULT_PNTnT_DODECAHEDRON", BaseMesh::CreateBaseMesh<VertexPositionNormalTangentTexture>(commandList, new_pntnt_vertices, indices));

		vertices.clear();
		indices.clear();
		MeshUtill::CreateIcosahedron(1.0f, vertices, indices);
		ConvertVertex(vertices, new_pc_vertices, new_pn_vertices, new_pt_vertices, new_pntnt_vertices);
		MeshUtill::SetNormalTangent(new_pntnt_vertices);
		AddMesh(L"DEFAULT_PC_ICOSAHEDRON", BaseMesh::CreateBaseMesh<VertexPositionColor>(commandList, new_pc_vertices, indices));
		AddMesh(L"DEFAULT_PN_ICOSAHEDRON", BaseMesh::CreateBaseMesh<VertexPositionNormal>(commandList, new_pn_vertices, indices));
		AddMesh(L"DEFAULT_PT_ICOSAHEDRON", BaseMesh::CreateBaseMesh<VertexPositionTexture>(commandList, new_pt_vertices, indices));
		AddMesh(L"DEFAULT_PNTnT_ICOSAHEDRON", BaseMesh::CreateBaseMesh<VertexPositionNormalTangentTexture>(commandList, new_pntnt_vertices, indices));

	}

	shared_ptr<BaseTexture> BaseScene::CreateTextureFlomFile(const wstring& falsename) {
		auto pDevice = App::GetBaseDevice();
		auto commandList = pDevice->GetComandList();
		//テクスチャの作成
		//シェーダリソースハンドルを作成
		UINT srvIndex = pDevice->GetSrvNextIndex();
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


	void BaseScene::OnInitScene() {
		//デフォルトのメッシュ群の作成
		CreateDefaultMeshes();
	}


	void BaseScene::OnInitFrame(BaseFrame* pBaseFrame) {
		auto pDefaultDev = App::GetBaseDevice();
		auto pDevice = App::GetID3D12Device();
		ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pBaseFrame->m_commandAllocator)));
		auto stagePtr = GetActiveStage();
		if (stagePtr) {
			stagePtr->OnInitFrame(pBaseFrame);
		}
	}

	void BaseScene::PopulateCommandList(BaseFrame* pBaseFrame) {
		auto stagePtr = GetActiveStage();
		if (stagePtr) {
			stagePtr->OnRender();
		}
	}

	void BaseScene::WriteConstantBuffers(BaseFrame* pBaseFrame) {
		auto stagePtr = GetActiveStage();
		if (stagePtr) {
			stagePtr->WriteConstantBuffers(pBaseFrame);
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
// end namespace basecross
