/*!
@file BaseMesh.cpp
@brief メッシュクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	アニメーション
	//--------------------------------------------------------------------------------------
	Animation::Animation(const aiScene* pScene, const aiAnimation* pAnimation, const std::string& nodeName) :
		m_pScene(pScene),
		m_pAnimation(pAnimation),
		m_rootNode(nullptr),
		m_nodeName(nodeName),
		m_hasAnimation(false)
	{
		if (pAnimation->mName.data == nodeName) {
			m_rootNode = pAnimation->mChannels[0];
			m_hasAnimation = true;
		}
	}


	void Animation::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const bsm::Mat4x4& ParentTransform) {
		std::string NodeName(pNode->mName.data);
		
		const aiAnimation* pAnimation = m_pAnimation;
		bsm::Mat4x4 NodeTransformation(pNode->mTransformation);
	}


	bool Animation::CreateAnimation() {

		bsm::Mat4x4 Identity;
		Identity.identity();

		//float TicksPerSecond = m_pScene->mAnimations[0]->mTicksPerSecond != 0 ?
		//	m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f;
		float TicksPerSecond = (float)m_pAnimation->mTicksPerSecond != 0.0f ?
			(float)m_pAnimation->mTicksPerSecond : 25.0f;


		float TimeInTicks = TicksPerSecond;
		float AnimationTime = (float)fmod(TimeInTicks, m_pAnimation->mDuration);
//		float AnimationTime = fmod(TimeInTicks, m_pScene->mAnimations[0]->mDuration);

//		ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);


		return false;
	}





	//--------------------------------------------------------------------------------------
	///	メッシュ
	//--------------------------------------------------------------------------------------
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
		XMFLOAT3 pointA(0, -height / 2.0f, 0);
		XMFLOAT3 pointB(0, height / 2.0f, 0);
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

	struct bonesstr {
		uint32_t vIndex;
		uint32_t boneIndex;
		float weight;
		bool operator<(const bonesstr& another) const {
			return vIndex < another.vIndex;
		}

	};


	struct bonedata {
		uint32_t vIndex;
		struct sub {
			uint32_t boneIndex;
			float weight;
		}subdata[4];
	};

	Assimp::Importer BaseMesh::m_importer;
	const aiScene* BaseMesh::m_pScene = nullptr;

	std::shared_ptr<BaseMesh> BaseMesh::CreateBoneModelMesh(
		ID3D12GraphicsCommandList* pCommandList,
		const std::wstring& dataDir, const std::wstring& dataFile)
	{
		try {
			std::wstring modelFile = dataDir + dataFile;
			if (modelFile.size() > 0) {
				std::string mbModelFile;
				Util::WStoMB(modelFile, mbModelFile);

//				Assimp::Importer importer;
				m_pScene = m_importer.ReadFile(mbModelFile,
					aiProcess_CalcTangentSpace |
					aiProcess_Triangulate |
					aiProcess_JoinIdenticalVertices |
					aiProcess_SortByPType);

				if (nullptr == m_pScene) {
					throw BaseException(
						L"データの読み込みに失敗しました。",
						L"BaseMesh::CreateStaticModelMesh()"
					);
				}


				aiVector3D zero3D(0.0f, 0.0f, 0.0f);
				aiColor4D zeroColor(0.0f, 0.0f, 0.0f, 0.0f);
				std::vector<VertexPositionNormalTextureSkinning> vertices;

				std::vector<uint32_t> indices;
				std::vector<bonedata> fixVec;

				for (uint32_t i = 0; i < m_pScene->mNumMeshes; i++) {
					auto src = m_pScene->mMeshes[i];
					if (src->HasPositions()) {
						for (uint32_t j = 0; j < src->mNumVertices; j++) {
							auto position = &(src->mVertices[j]);
							auto normal = &(src->mNormals[j]);
							auto uv = (src->HasTextureCoords(0)) ? &(src->mTextureCoords[0][j]) : &zero3D;
							auto tangent = (src->HasTangentsAndBitangents()) ? &(src->mTangents[j]) : &zero3D;
							auto color = (src->HasVertexColors(0)) ? &(src->mColors[0][j]) : &zeroColor;

							VertexPositionNormalTextureSkinning v = {};
							v.position.x = position->x;
							v.position.y = position->y;
							v.position.z = position->z;
							v.normal.x = normal->x;
							v.normal.y = normal->y;
							v.normal.z = normal->z;
							v.textureCoordinate.x = uv->x;
							v.textureCoordinate.y = uv->y;



							vertices.push_back(v);
						}
					}

					if (src->HasFaces()) {

						for (auto j = 0u; j < src->mNumFaces; ++j)
						{
							const auto& face = src->mFaces[j];

							indices.push_back(face.mIndices[0]);
							indices.push_back(face.mIndices[1]);
							indices.push_back(face.mIndices[2]);
						}

					}
					if (src->HasBones()) {
						std::vector<bonesstr> dataVec;;
						for (auto j = 0u; j < src->mNumBones; ++j) {
							for (auto k = 0u; k < src->mBones[j]->mNumWeights; k++) {
								bonesstr bone;
								bone.boneIndex = j;
								bone.vIndex = src->mBones[j]->mWeights[k].mVertexId;
								bone.weight = src->mBones[j]->mWeights[k].mWeight;
								dataVec.push_back(bone);
							}
						}
						std::sort(dataVec.begin(), dataVec.end());

						//std::ofstream ofs("debug.txt");

						bonedata fixData = {};
						for (uint32_t index = 0; index < dataVec.size();) {
							fixData = {};
							fixData.vIndex = dataVec[index].vIndex;
							uint32_t count = 0;
							while (count < 4) {
								fixData.subdata[count].boneIndex = dataVec[index].boneIndex;
								fixData.subdata[count].weight = dataVec[index].weight;
								count++;
								index++;
								if (index >= dataVec.size()) {
									break;
								}
								if (fixData.vIndex != dataVec[index].vIndex) {
									break;
								}
							}
							//ofs << fixData.vIndex << ','
							//	<< '{' << fixData.subdata[0].boneIndex << ','
							//	<< fixData.subdata[0].weight << "},"
							//	<< '{' << fixData.subdata[1].boneIndex << ','
							//	<< fixData.subdata[1].weight << "},"
							//	<< '{' << fixData.subdata[2].boneIndex << ','
							//	<< fixData.subdata[2].weight << "},"
							//	<< '{' << fixData.subdata[3].boneIndex << ','
							//	<< fixData.subdata[3].weight << "},"
							//	<< "\n";
							fixVec.push_back(fixData);
						}
					}
				}
				for (auto i = 0u; i < vertices.size(); i++) {
					vertices[i].indices[0] = fixVec[i].subdata[0].boneIndex;
					vertices[i].indices[1] = fixVec[i].subdata[1].boneIndex;
					vertices[i].indices[2] = fixVec[i].subdata[2].boneIndex;
					vertices[i].indices[3] = fixVec[i].subdata[3].boneIndex;

					vertices[i].weights[0] = fixVec[i].subdata[0].weight;
					vertices[i].weights[1] = fixVec[i].subdata[1].weight;
					vertices[i].weights[2] = fixVec[i].subdata[2].weight;
					vertices[i].weights[3] = fixVec[i].subdata[3].weight;
				}
				if (vertices.size() > 0 && indices.size() > 0) {
					std::shared_ptr<BaseMesh> mesh = BaseMesh::CreateBaseMesh<VertexPositionNormalTextureSkinning>(pCommandList, vertices, indices);
					mesh->CreateAnimations();
					return mesh;
				}
				else {
					return nullptr;
				}
			}
		}
		catch (...) {
			throw;
		}
		return nullptr;
	}


	void BaseMesh::CreateAnimations() {
		for (auto i = 0u; i < m_pScene->mNumAnimations; i++) {
			const aiAnimation* pAnimation = m_pScene->mAnimations[i];
			std::string nodeName = m_pScene->mAnimations[i]->mName.data;
			std::shared_ptr<Animation> ptr 
				= std::shared_ptr<Animation>(new Animation(m_pScene,pAnimation, nodeName));
			if (ptr->HasAnimation()) {
				m_AnimationVec.push_back(ptr);
			}
		}
		for (auto& anim : m_AnimationVec) {
			anim->CreateAnimation();
		}
	}



}
// end namespace basecross
