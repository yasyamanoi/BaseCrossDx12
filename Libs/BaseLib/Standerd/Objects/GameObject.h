/*!
@file GameObject.h
@brief ゲームオブジェクトクラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// 配置されるオブジェクト
	//--------------------------------------------------------------------------------------
	class GameObject : public ObjectInterface {
	protected:
		GameObject();
		virtual ~GameObject();
		bool m_updateActive = true; //updateするかどうか
		bool m_drawActive = true; //drawするかどうか
		bool m_alphaActive = false; //透明かどうか
		int m_drawLayer = 0;	//描画レイヤー
		std::vector<ConstBuffParam> m_constBuffParamVec; //コンスタントバッファパラメータ用
	public:
		virtual void OnCreate() {}
		virtual void OnUpdate() {}
		virtual void OnUpdate(double elapsedTime) {}
		virtual void OnDraw() {}
		virtual void OnDestroy() {}

		virtual void OnShadowDraw(ID3D12GraphicsCommandList* pCommandList) {}
		virtual void OnSceneDraw(ID3D12GraphicsCommandList* pCommandList) {}
		virtual void OnPostprocessDraw(ID3D12GraphicsCommandList* pCommandList) {}

	};
}
//end namespace basecross
