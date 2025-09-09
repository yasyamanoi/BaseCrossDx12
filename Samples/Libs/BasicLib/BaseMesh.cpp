/*!
@file BaseMesh.cpp
@brief メッシュクラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	Assimpローダー
	//--------------------------------------------------------------------------------------


/*


Assimp::Importer importer;
 uint32_t flag = 0;
 flag |= aiProcess_ConvertToLeftHanded;
 flag |= aiProcess_Triangulate;
 importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false); // ←ピボットを読み込まない設定
 auto constScene = importer.ReadFile(modelFileName, flag);
 if (!constScene)
 {
  MessageBox(0, L"aiSceneの読み込みに失敗しました", 0, 0);
 }

*/

	BaseAssimp::BaseAssimp(const std::string& modelFile) :
		m_ModelFile(modelFile)
	{
		try{

			uint32_t flag = 0;
			flag |= aiProcess_ConvertToLeftHanded;
			flag |= aiProcess_Triangulate;
			m_importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false); // ←ピボットを読み込まない設定

			m_pScene = m_importer.ReadFile(m_ModelFile,
				ASSIMP_LOAD_FLAGS);
			if (nullptr == m_pScene) {
				throw BaseException(
					L"データの読み込みに失敗しました。",
					L"BaseAssimp::BaseAssimp()"
				);
			}
			//座標変換の逆行列を初期化
			Mat4x4 tmpMat(m_pScene->mRootNode->mTransformation);
			m_GlobalInverseTransform = tmpMat;
			m_GlobalInverseTransform.inverse();

		}
		catch (...) {
			throw;
		}
	}

	bool BaseAssimp::InitScene(std::vector<VertexPositionNormalTextureSkinning>& vertices,
		std::vector<uint32_t>& indices) {

		m_Meshes.resize(m_pScene->mNumMeshes);
		//		m_Materials.resize(pScene->mNumMaterials);

		unsigned int NumVertices = 0;
		unsigned int NumIndices = 0;

		CountVerticesAndIndices(NumVertices, NumIndices);

		ReserveSpace(NumVertices, NumIndices);
		//最初のメッシュを読み込む
		InitFirstMeshe();

		//if (!InitMaterials(pScene, Filename)) {
		//	return false;
		//}

//		PopulateBuffers();

		int a = 0;

		for (auto& v : m_SkinnedVertices) {
			VertexPositionNormalTextureSkinning tempV;
			tempV.position = v.Position;
			tempV.normal = v.Normal;
			tempV.textureCoordinate = v.TexCoords;
			for (int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++) {
				tempV.indices[i] = v.Bones.BoneIDs[i];
				tempV.weights[i] = v.Bones.Weights[i];
			}
			vertices.push_back(tempV);
		}
		for (auto& i : m_Indices) {
			indices.push_back(i);
		}
		

		return true;
		//		return GLCheckError();



	}

	bool BaseAssimp::InitMuliScene(std::vector <SkinningMeshSet>& meshSetVec) {

		m_Meshes.resize(m_pScene->mNumMeshes);
		unsigned int NumVertices = 0;
		unsigned int NumIndices = 0;
		CountVerticesAndIndices(NumVertices, NumIndices);
		ReserveSpace(NumVertices, NumIndices);
		InitAllMeshes();
//		PopulateBuffers();
		for (size_t i = 0; i < m_AllSkinnedVertices.size(); i++) {
			std::vector<VertexPositionNormalTextureSkinning> vertices;
			std::vector<uint32_t> indices;
			for (auto& v : m_AllSkinnedVertices[i]) {
				vertices.clear();
				indices.clear();
				VertexPositionNormalTextureSkinning tempV;
				tempV.position = v.Position;
				tempV.normal = v.Normal;
				tempV.textureCoordinate = v.TexCoords;
				for (int j = 0; j < MAX_NUM_BONES_PER_VERTEX; j++) {
					tempV.indices[j] = v.Bones.BoneIDs[j];
					tempV.weights[j] = v.Bones.Weights[j];
				}
				vertices.push_back(tempV);
			}
			indices = m_AllIndices[i];
			SkinningMeshSet meshSet;
			meshSet.vertices = vertices;
			meshSet.indices = indices;
			meshSetVec.push_back(meshSet);
		}


		//for (auto& v : m_SkinnedVertices) {
		//	VertexPositionNormalTextureSkinning tempV;
		//	tempV.position = v.Position;
		//	tempV.normal = v.Normal;
		//	tempV.textureCoordinate = v.TexCoords;
		//	for (int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++) {
		//		tempV.indices[i] = v.Bones.BoneIDs[i];
		//		tempV.weights[i] = v.Bones.Weights[i];
		//	}
		//	vertices.push_back(tempV);
		//}
		//for (auto& i : m_Indices) {
		//	indices.push_back(i);
		//}


		return true;

	}




	void BaseAssimp::CountVerticesAndIndices(uint32_t& NumVertices, uint32_t& NumIndices) {
		for (unsigned int i = 0; i < m_Meshes.size(); i++) {
			m_Meshes[i].MaterialIndex = m_pScene->mMeshes[i]->mMaterialIndex;
			m_Meshes[i].NumIndices = m_pScene->mMeshes[i]->mNumFaces * 3;
			m_Meshes[i].BaseVertex = NumVertices;
			m_Meshes[i].BaseIndex = NumIndices;

			NumVertices += m_pScene->mMeshes[i]->mNumVertices;
			NumIndices += m_Meshes[i].NumIndices;
		}

	}

	void BaseAssimp::InitFirstMeshe() {
		if (m_Meshes.size() == 0) {
			throw BaseException(
				L"メッシュが見つかりません",
				L"BaseAssimp::InitFirstMeshe()"
			);
		}
		const aiMesh* paiMesh = m_pScene->mMeshes[0];
		m_SkinnedVertices.clear();
		m_Indices.clear();
		InitSingleMesh(0, paiMesh);
	}


	void BaseAssimp::InitAllMeshes() {
		for (unsigned int i = 0; i < m_Meshes.size(); i++) {
			const aiMesh* paiMesh = m_pScene->mMeshes[i];
			m_SkinnedVertices.clear();
			m_Indices.clear();
			InitSingleMesh(i, paiMesh);
			if (m_SkinnedVertices.size() > 0 && m_Indices.size() > 0) {
				m_AllSkinnedVertices.push_back(m_SkinnedVertices);
				m_AllIndices.push_back(m_Indices);
			}
		}
	}

	void BaseAssimp::InitializeRequiredNodeMap(const aiNode* pNode)
	{
		std::string NodeName(pNode->mName.C_Str());

		NodeInfo info(pNode);

		m_requiredNodeMap[NodeName] = info;

		for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
			InitializeRequiredNodeMap(pNode->mChildren[i]);
		}
	}

	void BaseAssimp::ReserveSpace(unsigned int NumVertices, unsigned int NumIndices)
	{
		m_Vertices.reserve(NumVertices);
		m_Indices.reserve(NumIndices);
		InitializeRequiredNodeMap(m_pScene->mRootNode);
	}



	int BaseAssimp::GetBoneId(const aiBone* pBone)
	{
		int BoneIndex = 0;
		std::string BoneName(pBone->mName.C_Str());

		if (m_BoneNameToIndexMap.find(BoneName) == m_BoneNameToIndexMap.end()) {
			// Allocate an index for a new bone
			BoneIndex = (int)m_BoneNameToIndexMap.size();
			m_BoneNameToIndexMap[BoneName] = BoneIndex;
		}
		else {
			BoneIndex = m_BoneNameToIndexMap[BoneName];
		}

		return BoneIndex;
	}

	void BaseAssimp::MarkRequiredNodesForBone(const aiBone* pBone)
	{
		std::string NodeName(pBone->mName.C_Str());

		const aiNode* pParent = NULL;

		do {
			std::map<std::string, NodeInfo>::iterator it = m_requiredNodeMap.find(NodeName);

			if (it == m_requiredNodeMap.end()) {
				printf("Cannot find bone %s in the hierarchy\n", NodeName.c_str());
				assert(0);
			}

			it->second.isRequired = true;

			pParent = it->second.pNode->mParent;

			if (pParent) {
				NodeName = std::string(pParent->mName.C_Str());
			}

		} while (pParent);
	}



	void BaseAssimp::LoadSingleBone(uint32_t MeshIndex, const aiBone* pBone, std::vector<SkinnedVertex>& SkinnedVertices, int BaseVertex)
	{
		int BoneId = GetBoneId(pBone);


		if (BoneId == m_BoneInfo.size()) {
			BoneInfo bi(pBone->mOffsetMatrix);
			// bi.OffsetMatrix.Print();
			m_BoneInfo.push_back(bi);
		}

		for (uint32_t i = 0; i < pBone->mNumWeights; i++) {
			const aiVertexWeight& vw = pBone->mWeights[i];
			uint32_t GlobalVertexID = BaseVertex + pBone->mWeights[i].mVertexId;
			// printf("%d: %d %f\n",i, pBone->mWeights[i].mVertexId, vw.mWeight);
			SkinnedVertices[GlobalVertexID].Bones.AddBoneData(BoneId, vw.mWeight);
		}

		MarkRequiredNodesForBone(pBone);
	}


	void BaseAssimp::LoadMeshBones(uint32_t MeshIndex, const aiMesh* pMesh, std::vector<SkinnedVertex>& SkinnedVertices, int BaseVertex)
	{
		if (pMesh->mNumBones > MAX_BONES) {
			printf("The number of bones in the model (%d) is larger than the maximum supported (%d)\n", pMesh->mNumBones, MAX_BONES);
			printf("Make sure to increase the macro MAX_BONES in the C++ header as well as in the shader to the same value\n");
			assert(0);
		}

		// printf("Loading mesh bones %d\n", MeshIndex);
		for (uint32_t i = 0; i < pMesh->mNumBones; i++) {
			// printf("Bone %d %s\n", i, pMesh->mBones[i]->mName.C_Str());
			LoadSingleBone(MeshIndex, pMesh->mBones[i], SkinnedVertices, BaseVertex);
		}
	}


	void BaseAssimp::InitSingleMesh(uint32_t MeshIndex, const aiMesh* paiMesh)
	{
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

		// printf("Mesh %d\n", MeshIndex);
		// Populate the vertex attribute vectors
		SkinnedVertex v;

		for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {

			const aiVector3D& pPos = paiMesh->mVertices[i];
			// printf("%d: ", i); Vector3f v(pPos.x, pPos.y, pPos.z); v.Print();
			v.Position = Vec3(pPos.x, pPos.y, pPos.z);

			if (paiMesh->mNormals) {
				const aiVector3D& pNormal = paiMesh->mNormals[i];
				v.Normal = Vec3(pNormal.x, pNormal.y, pNormal.z);
			}
			else {
				aiVector3D Normal(0.0f, 1.0f, 0.0f);
				v.Normal = Vec3(Normal.x, Normal.y, Normal.z);
			}

			const aiVector3D& pTexCoord = paiMesh->HasTextureCoords(0) ? paiMesh->mTextureCoords[0][i] : Zero3D;
			v.TexCoords = Vec2(pTexCoord.x, pTexCoord.y);

			m_SkinnedVertices.push_back(v);
		}

		// Populate the index buffer
		for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
			const aiFace& Face = paiMesh->mFaces[i];
			m_Indices.push_back(Face.mIndices[0]);
			m_Indices.push_back(Face.mIndices[1]);
			m_Indices.push_back(Face.mIndices[2]);
		}

		LoadMeshBones(MeshIndex, paiMesh, m_SkinnedVertices, m_Meshes[MeshIndex].BaseVertex);
	}



	void BaseAssimp::GetBoneTransforms(float TimeInSeconds, std::vector<Mat4x4>& Transforms, unsigned int AnimationIndex)
	{
		if (AnimationIndex >= m_pScene->mNumAnimations) {
			printf("Invalid animation index %d, max is %d\n", AnimationIndex, m_pScene->mNumAnimations);
			assert(0);
		}

		Mat4x4 Identity;
		Identity.identity();

		float AnimationTimeTicks = CalcAnimationTimeTicks(TimeInSeconds, AnimationIndex);
		const aiAnimation& Animation = *m_pScene->mAnimations[AnimationIndex];

		ReadNodeHierarchy(AnimationTimeTicks, m_pScene->mRootNode, Identity, Animation);
		Transforms.resize(m_BoneInfo.size());

		for (uint32_t i = 0; i < m_BoneInfo.size(); i++) {
			Transforms[i] = m_BoneInfo[i].FinalTransformation;
		}
	}

	float BaseAssimp::CalcAnimationTimeTicks(float TimeInSeconds, unsigned int AnimationIndex)
	{
		float TicksPerSecond = (float)(m_pScene->mAnimations[AnimationIndex]->mTicksPerSecond != 0 ? m_pScene->mAnimations[AnimationIndex]->mTicksPerSecond : 25.0f);
		float TimeInTicks = TimeInSeconds * TicksPerSecond;
		// we need to use the integral part of mDuration for the total length of the animation
		float Duration = 0.0f;
		float fraction = modf((float)m_pScene->mAnimations[AnimationIndex]->mDuration, &Duration);
		float AnimationTimeTicks = fmod(TimeInTicks, Duration);
		return AnimationTimeTicks;
	}

	void BaseAssimp::ReadNodeHierarchy(float AnimationTimeTicks, const aiNode* pNode, const Mat4x4& ParentTransform, const aiAnimation& Animation)
	{
		std::string NodeName(pNode->mName.data);

		Mat4x4 NodeTransformation(pNode->mTransformation);

		const aiNodeAnim* pNodeAnim = FindNodeAnim(Animation, NodeName);

		if (pNodeAnim) {
			LocalTransform Transform;
			CalcLocalTransform(Transform, AnimationTimeTicks, pNodeAnim);

			Mat4x4 ScalingM;
			ScalingM.scale(Vec3(
				Transform.Scaling.x,
				Transform.Scaling.y,
				Transform.Scaling.z
			));
			//ScalingM.InitScaleTransform(Transform.Scaling.x, Transform.Scaling.y, Transform.Scaling.z);
			//        printf("Scaling %f %f %f\n", Transoform.Scaling.x, Transform.Scaling.y, Transform.Scaling.z);
			Mat4x4 RotationM;
			Quat qt;
			qt.x = Transform.Rotation.x;
			qt.y = Transform.Rotation.y;
			qt.z = Transform.Rotation.z;
			qt.w = Transform.Rotation.w;

			RotationM.rotation(qt);

			//Mat4x4 RotationM = Mat4x4(Transform.Rotation.GetMatrix());

			Mat4x4 TranslationM;
			TranslationM.translation(
				Vec3(Transform.Translation.x,
					Transform.Translation.y,
					Transform.Translation.z
				)
			);
			//TranslationM.InitTranslationTransform(Transform.Translation.x, Transform.Translation.y, Transform.Translation.z);
			//        printf("Translation %f %f %f\n", Transform.Translation.x, Transform.Translation.y, Transform.Translation.z);

			// Combine the above transformations
			NodeTransformation = ScalingM * RotationM * TranslationM;
			//			NodeTransformation = TranslationM * RotationM * ScalingM;
			NodeTransformation.transpose();
		}

//		Mat4x4 GlobalTransformation = NodeTransformation * ParentTransform;
		Mat4x4 GlobalTransformation = ParentTransform * NodeTransformation;

		if (m_BoneNameToIndexMap.find(NodeName) != m_BoneNameToIndexMap.end()) {
			uint32_t BoneIndex = m_BoneNameToIndexMap[NodeName];
			m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].OffsetMatrix;
		}

		for (uint32_t i = 0; i < pNode->mNumChildren; i++) {
			std::string ChildName(pNode->mChildren[i]->mName.data);

			std::map<std::string, NodeInfo>::iterator it = m_requiredNodeMap.find(ChildName);

			if (it == m_requiredNodeMap.end()) {
				printf("Child %s cannot be found in the required node map\n", ChildName.c_str());
				assert(0);
			}

			if (it->second.isRequired) {
				ReadNodeHierarchy(AnimationTimeTicks, pNode->mChildren[i], GlobalTransformation, Animation);
			}
		}
	}

	const aiNodeAnim* BaseAssimp::FindNodeAnim(const aiAnimation&
		Animation, const std::string& NodeName)
	{
		for (uint32_t i = 0; i < Animation.mNumChannels; i++) {
			const aiNodeAnim* pNodeAnim = Animation.mChannels[i];

			if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
				return pNodeAnim;
			}
		}

		return NULL;
	}

	void BaseAssimp::CalcLocalTransform(LocalTransform& Transform, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		CalcInterpolatedScaling(Transform.Scaling, AnimationTimeTicks, pNodeAnim);
		CalcInterpolatedRotation(Transform.Rotation, AnimationTimeTicks, pNodeAnim);
		CalcInterpolatedPosition(Transform.Translation, AnimationTimeTicks, pNodeAnim);
	}


	uint32_t BaseAssimp::FindPosition(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		for (uint32_t i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
			float t = (float)pNodeAnim->mPositionKeys[i + 1].mTime;
			if (AnimationTimeTicks < t) {
				return i;
			}
		}

		return 0;
	}


	void BaseAssimp::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		// we need at least two values to interpolate...
		if (pNodeAnim->mNumPositionKeys == 1) {
			Out = pNodeAnim->mPositionKeys[0].mValue;
			return;
		}

		uint32_t PositionIndex = FindPosition(AnimationTimeTicks, pNodeAnim);
		uint32_t NextPositionIndex = PositionIndex + 1;
		assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
		float t1 = (float)pNodeAnim->mPositionKeys[PositionIndex].mTime;
		if (t1 > AnimationTimeTicks) {
			Out = pNodeAnim->mPositionKeys[PositionIndex].mValue;
		}
		else {
			float t2 = (float)pNodeAnim->mPositionKeys[NextPositionIndex].mTime;
			float DeltaTime = t2 - t1;
			float Factor = (AnimationTimeTicks - t1) / DeltaTime;
			assert(Factor >= 0.0f && Factor <= 1.0f);
			const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
			const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
			aiVector3D Delta = End - Start;
			Out = Start + Factor * Delta;
		}
	}


	uint32_t BaseAssimp::FindRotation(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		assert(pNodeAnim->mNumRotationKeys > 0);

		for (uint32_t i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
			float t = (float)pNodeAnim->mRotationKeys[i + 1].mTime;
			if (AnimationTimeTicks < t) {
				return i;
			}
		}

		return 0;
	}


	void BaseAssimp::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		// we need at least two values to interpolate...
		if (pNodeAnim->mNumRotationKeys == 1) {
			Out = pNodeAnim->mRotationKeys[0].mValue;
			return;
		}

		uint32_t RotationIndex = FindRotation(AnimationTimeTicks, pNodeAnim);
		uint32_t NextRotationIndex = RotationIndex + 1;
		assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
		float t1 = (float)pNodeAnim->mRotationKeys[RotationIndex].mTime;
		if (t1 > AnimationTimeTicks) {
			Out = pNodeAnim->mRotationKeys[RotationIndex].mValue;
		}
		else {
			float t2 = (float)pNodeAnim->mRotationKeys[NextRotationIndex].mTime;
			float DeltaTime = t2 - t1;
			float Factor = (AnimationTimeTicks - t1) / DeltaTime;
			assert(Factor >= 0.0f && Factor <= 1.0f);
			const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
			const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
			aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
		}

		Out.Normalize();
	}


	uint32_t BaseAssimp::FindScaling(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		assert(pNodeAnim->mNumScalingKeys > 0);

		for (uint32_t i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
			float t = (float)pNodeAnim->mScalingKeys[i + 1].mTime;
			if (AnimationTimeTicks < t) {
				return i;
			}
		}

		return 0;
	}


	void BaseAssimp::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		// we need at least two values to interpolate...
		if (pNodeAnim->mNumScalingKeys == 1) {
			Out = pNodeAnim->mScalingKeys[0].mValue;
			return;
		}

		uint32_t ScalingIndex = FindScaling(AnimationTimeTicks, pNodeAnim);
		uint32_t NextScalingIndex = ScalingIndex + 1;
		assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
		float t1 = (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime;
		if (t1 > AnimationTimeTicks) {
			Out = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
		}
		else {
			float t2 = (float)pNodeAnim->mScalingKeys[NextScalingIndex].mTime;
			float DeltaTime = t2 - t1;
			float Factor = (AnimationTimeTicks - (float)t1) / DeltaTime;
			assert(Factor >= 0.0f && Factor <= 1.0f);
			const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
			const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
			aiVector3D Delta = End - Start;
			Out = Start + Factor * Delta;
		}
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


	std::shared_ptr<BaseMesh> BaseMesh::CreateBoneModelMesh(
		ID3D12GraphicsCommandList* pCommandList,
		const std::wstring& dataDir, const std::wstring& dataFile)
	{
		try {
			std::wstring modelFile = dataDir + dataFile;
			if (modelFile.size() > 0) {
				std::string mbModelFile;
				Util::WStoMB(modelFile, mbModelFile);

				std::shared_ptr<BaseAssimp> ptrBaseAssimp = std::shared_ptr<BaseAssimp>(new BaseAssimp(mbModelFile));

				std::vector<VertexPositionNormalTextureSkinning> vertices;
				std::vector<uint32_t> indices;
				ptrBaseAssimp->InitScene(vertices, indices);

				//std::vector < SkinningMeshSet> meshVec;
				//ptrBaseAssimp->InitMuliScene(meshVec);

				if (vertices.size() > 0 && indices.size() > 0) {
					std::shared_ptr<BaseMesh> mesh = BaseMesh::CreateBaseMesh<VertexPositionNormalTextureSkinning>(pCommandList, vertices, indices);
					mesh->m_BaseAssimp = ptrBaseAssimp;

					////アニメーションテスト
					//std::vector<Mat4x4> Transforms;
					//ptrBaseAssimp->GetBoneTransforms(0.0f, Transforms);


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





}
// end namespace basecross
