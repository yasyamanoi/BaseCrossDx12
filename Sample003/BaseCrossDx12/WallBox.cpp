/*!
@file WallBox.cpp
@brief 四角のオブジェクト　実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	using namespace std;
	using namespace bsm;

	//--------------------------------------------------------------------------------------
	// 四角のオブジェクト
	//--------------------------------------------------------------------------------------
	WallBox::WallBox(const shared_ptr<Stage>& stage, const TransParam& param) :
		GameObject(stage,param),
		m_totalTime(0.0)
	{
	}
	WallBox::~WallBox() {}

	void WallBox::OnCreate() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		//メッシュ
		m_mesh = BaseMesh::CreateCube(pCommandList, 1.0f);
		auto ptrShadow = AddComponent<ShadowmapComp>();
		ptrShadow->SetBaseMesh(m_mesh);
		//テクスチャ
		auto texFile = App::GetRelativeAssetsDir() + L"wall.jpg";
		m_texture = BaseTexture::CreateTextureFlomFile(pCommandList, texFile);
		auto ptrScene = AddComponent<SpSceneComp>();
		ptrScene->SetBaseMesh(m_mesh);
		ptrScene->SetBaseTexture(m_texture);
	}

	void WallBox::OnUpdateConstantBuffers() {
		auto ptrShadow = GetComponent<ShadowmapComp>();
		ptrShadow->OnUpdateConstantBuffers();
		auto ptrScene = GetComponent<SpSceneComp>();
		ptrScene->OnUpdateConstantBuffers();
	}

	void WallBox::OnCommitConstantBuffers() {
		auto ptrShadow = GetComponent<ShadowmapComp>();
		ptrShadow->OnCommitConstantBuffers();
		auto ptrScene = GetComponent<SpSceneComp>();
		ptrScene->OnCommitConstantBuffers();
	}


	void WallBox::OnUpdate(double elapsedTime) {
		//Transformコンポーネントを取り出す
		auto ptrTrans = GetComponent<Transform>();
		auto& param = ptrTrans->GetTransParam();

		m_totalTime += elapsedTime;
		if (m_totalTime >= XM_2PI) {
			m_totalTime = 0.0;
		}
		Quat spanQt(Vec3(1.0f, 1.0f, 0.0f), (float)(elapsedTime * 4));
		Quat quaternion(param.quaternion);
		quaternion *= spanQt;
		param.quaternion = quaternion;
		param.position.x = (float)sin(m_totalTime) * 2.0f;
	}

	void WallBox::OnShadowDraw() {
		auto ptrShadow = GetComponent<ShadowmapComp>();
		ptrShadow->OnShadowDraw();
	}
	void WallBox::OnSceneDraw() {
		auto ptrScene = GetComponent<SpSceneComp>();
//		auto ptrScene = GetComponent<BcSceneComp>();
		ptrScene->OnSceneDraw();
	}


}
// end namespace basecross
