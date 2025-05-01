#pragma once
/*!
@file MyObject.h
@brief Myオブジェクトクラス
*/


#pragma once
#include "stdafx.h"


namespace basecross {

	using namespace std;
	using namespace bsm;

	//--------------------------------------------------------------------------------------
	// 配置されるオブジェクトの親
	//--------------------------------------------------------------------------------------
	class MyObject : public GameObject {
	protected:
		BasicConstantBuffer m_constantBuffer;
		size_t m_constantBufferIndex;

		ShadowConstantBuffer m_shadowConstantBuffer;
		size_t m_shadowConstantBufferIndex;

		//フォグが有効かどうか
		bool m_fogEnabled = true;
		//フォグの開始位置
		float m_fogStart = -25.0f;
		//フォグの終了位置
		float m_fogEnd = -40.0f;
		//フォグ色
		XMFLOAT4 m_fogColor;
		//フォグベクトル
		XMFLOAT3 m_fogVector;
		MyObject(const TransParam& param) :
			GameObject(param){
			m_fogColor = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
			m_fogVector = XMFLOAT3(0.0, 0.0, 1.0f);
		}
		virtual ~MyObject() {}

		shared_ptr<BaseMesh> m_mesh;
		shared_ptr < BaseTexture> m_texture;

	public:
		virtual void UpdateConstantBuffers(Scene* scene);
		virtual void CommitConstantBuffers(Scene* scene);

		virtual void OnCreate(ID3D12GraphicsCommandList* pCommandList)override;
		virtual void OnShadowDraw(ID3D12GraphicsCommandList* pCommandList)override;
		virtual void OnSceneDraw(ID3D12GraphicsCommandList* pCommandList)override;
		virtual void OnPostprocessDraw(ID3D12GraphicsCommandList* pCommandList)override;
		virtual void OnDestroy()override;

	};


}
// end namespace basecross
