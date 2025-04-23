/*!
@file MyObject.h
@brief ゲームオブジェクトクラス
*/


#pragma once
#include "stdafx.h"


namespace basecross {

	using namespace std;

	//--------------------------------------------------------------------------------------
	// 配置されるオブジェクトの親
	//--------------------------------------------------------------------------------------
	class MyObject : public ObjectInterface {
	protected:
		TransParam m_param;
		BaseConstantBuffer m_constantBuffer;
		size_t m_constantBufferIndex;

		ShadowConstant m_shadowConstantBuffer;
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
		MyObject(const TransParam& param):m_param(param){
			m_fogColor = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
			m_fogVector = XMFLOAT3(0.0, 0.0, 1.0f);
		}
		virtual ~MyObject() {}

		shared_ptr<BaseMesh> m_mesh;
		shared_ptr < BaseTexture> m_texture;

	public:
		virtual void UpdateConstantBuffers(Scene* scene);
		virtual void CommitConstantBuffers(Scene* scene);

		virtual void OnCreate(ID3D12GraphicsCommandList* pCommandList);
		virtual void OnShadowDraw(ID3D12GraphicsCommandList* pCommandList);
		virtual void OnSceneDraw(ID3D12GraphicsCommandList* pCommandList);
		virtual void OnPostprocessDraw(ID3D12GraphicsCommandList* pCommandList);
		virtual void OnDestroy();

	};

	//--------------------------------------------------------------------------------------
	// 四角のオブジェクト
	//--------------------------------------------------------------------------------------
	class WallBox : public  MyObject {
		double m_totalTime;
	protected:
	public:
		WallBox(const TransParam& param);
		virtual ~WallBox();
		virtual void OnCreate(ID3D12GraphicsCommandList* pCommandList);
		virtual void OnUpdate(double elapsedTime);
	};

	//--------------------------------------------------------------------------------------
	// ステージのオブジェクト
	//--------------------------------------------------------------------------------------
	class SkyStage : public MyObject {
	public:
		SkyStage(const TransParam& param);
		virtual ~SkyStage();
		virtual void OnCreate(ID3D12GraphicsCommandList* pCommandList);
		virtual void OnUpdate(double elapsedTime);

	};



}
// end namespace basecross
