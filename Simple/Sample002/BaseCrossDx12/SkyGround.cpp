/*!
@file SkyGround.h
@brief 土台クラス　実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// 土台のオブジェクト
	//--------------------------------------------------------------------------------------
	SkyGround::SkyGround(const shared_ptr<Stage>& stage, const TransParam& param) :
		GameObject(stage,param)
	{
	}
	SkyGround::~SkyGround() {}

	void SkyGround::OnCreate() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		m_mesh = BaseMesh::CreateCube(pCommandList, 1.0f);
		auto ptrShadow = AddComponent<ShadowmapComp>();
		ptrShadow->SetBaseMesh(m_mesh);
		auto texFile = App::GetRelativeAssetsDir() + L"sky.jpg";
		m_texture = BaseTexture::CreateTextureFlomFile(pCommandList, texFile);
		auto ptrScene = AddComponent<BcSceneComp>();
		ptrScene->SetBaseMesh(m_mesh);
		ptrScene->SetBaseTexture(m_texture);
	}

	void SkyGround::OnUpdateConstantBuffers() {
		auto ptrShadow = GetComponent<ShadowmapComp>();
		ptrShadow->OnUpdateConstantBuffers();
		auto ptrScene = GetComponent<BcSceneComp>();
		ptrScene->OnUpdateConstantBuffers();
	}

	void SkyGround::OnCommitConstantBuffers() {
		auto ptrShadow = GetComponent<ShadowmapComp>();
		ptrShadow->OnCommitConstantBuffers();
		auto ptrScene = GetComponent<BcSceneComp>();
		ptrScene->OnCommitConstantBuffers();
	}

	void SkyGround::OnShadowDraw() {
		auto ptrShadow = GetComponent<ShadowmapComp>();
		ptrShadow->OnShadowDraw();
	}
	void SkyGround::OnSceneDraw() {
		auto ptrScene = GetComponent<BcSceneComp>();
		ptrScene->OnSceneDraw();
	}

}
// end namespace basecross
