/*!
@file Stage.cpp
@brief 親ステージクラス　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {

	void Stage::OnUpdateConstantBuffers()
	{
		for (auto& v : m_gameObjectvec) {
			v->OnUpdateConstantBuffers();
		}
	}

	void Stage::OnCommitConstantBuffers()
	{
		for (auto& v : m_gameObjectvec) {
			v->OnCommitConstantBuffers();
		}
	}

	void Stage::OnUpdate(double elapsedTime) {
		for (auto& v : m_gameObjectvec) {
			v->OnUpdate(elapsedTime);
		}
	}

	void Stage::OnUpdate2(double elapsedTime) {
		for (auto& v : m_gameObjectvec) {
			v->OnUpdate2(elapsedTime);
		}
	}


	void Stage::OnShadowDraw() {
		for (auto& v : m_gameObjectvec) {
			v->OnShadowDraw();
		}
	}
	void Stage::OnSceneDraw() {
		for (auto& v : m_gameObjectvec) {
			v->OnSceneDraw();
		}
	}

	void Stage::OnDestroy() {
		for (auto& v : m_gameObjectvec) {
			v->OnDestroy();
		}
	}

	void Stage::SetToBefore() {
		for (auto& v : m_gameObjectvec) {
			v->SetToBefore();
		}
	}



	



}
// end namespace basecross
