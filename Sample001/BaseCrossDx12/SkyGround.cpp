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


	void SkyGround::OnCreate(ID3D12GraphicsCommandList* pCommandList) {
		MyObject::OnCreate(pCommandList);
		m_mesh = BaseMesh::CreateCube(pCommandList, 1.0f);
		auto texFile = App::GetRelativeAssetsDir() + L"sky.jpg";
		m_texture = BaseTexture::CreateTextureFlomFile(pCommandList, texFile);
	}

	void SkyGround::OnUpdate(double elapsedTime) {}

}
// end namespace basecross
