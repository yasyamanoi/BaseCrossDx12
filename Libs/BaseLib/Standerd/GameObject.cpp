/*!
@file GameObject.cpp
@brief ゲームオブジェクトクラス　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"


namespace basecross {

	GameObject::GameObject() {}
	GameObject::~GameObject() {}
	void GameObject::OnCreate(ID3D12GraphicsCommandList* pCommandList){}
	void GameObject::OnUpdate(double elapsedTime){}

}
// end namespace basecross
