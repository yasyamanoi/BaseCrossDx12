/*!
@file SkyGround.h
@brief 土台クラス　実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	using namespace std;
	using namespace bsm;

	//--------------------------------------------------------------------------------------
	// 土台のオブジェクト
	//--------------------------------------------------------------------------------------
	SkyGround::SkyGround(const TransParam& param) :
		MyObject(param)
	{
	}
	SkyGround::~SkyGround() {}


	void SkyGround::OnCreate() {
		MyObject::OnCreate();
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		m_mesh = BaseMesh::CreateCube(pCommandList, 1.0f);
		auto texFile = App::GetRelativeAssetsDir() + L"sky.jpg";
		m_texture = BaseTexture::CreateTextureFlomFile(pCommandList, texFile);
	}

}
// end namespace basecross
