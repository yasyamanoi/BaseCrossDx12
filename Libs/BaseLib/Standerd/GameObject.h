/*!
@file GameObject.h
@brief ゲームオブジェクトクラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"


namespace basecross {

	using namespace std;
	using namespace bsm;

	//--------------------------------------------------------------------------------------
	///	オブジェクト変換用
	//--------------------------------------------------------------------------------------
	struct TransParam {
		Vec3 scale;
		Vec3 rotOrigin;
		Quat quaternion;
		Vec3 position;
	};

	//--------------------------------------------------------------------------------------
	// 配置されるオブジェクトの親
	//--------------------------------------------------------------------------------------
	class GameObject : public ObjectInterface {
	protected:
		TransParam m_param;
		GameObject(const TransParam& param):m_param(param){
		}
		virtual ~GameObject() {}
	public:
		virtual void UpdateConstantBuffers(Scene* scene) {}
		virtual void CommitConstantBuffers(Scene* scene) {}
		virtual void OnCreate(ID3D12GraphicsCommandList* pCommandList)override {}
		virtual void OnShadowDraw(ID3D12GraphicsCommandList* pCommandList)override {}
		virtual void OnSceneDraw(ID3D12GraphicsCommandList* pCommandList)override {}
		virtual void OnPostprocessDraw(ID3D12GraphicsCommandList* pCommandList)override {}
		virtual void OnDestroy()override {}
	};


}
// end namespace basecross
