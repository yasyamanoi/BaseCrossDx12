/*!
@file GameObject.h
@brief ゲームオブジェクトクラス
*/


#pragma once
#include "stdafx.h"


namespace basecross {

	using namespace std;

	//--------------------------------------------------------------------------------------
	// 配置されるオブジェクトの親
	//--------------------------------------------------------------------------------------
	class GameObject : public ObjectInterface {
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
		GameObject(const TransParam& param):m_param(param){
			m_fogColor = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
			m_fogVector = XMFLOAT3(0.0, 0.0, 1.0f);
		}
		virtual ~GameObject() {}

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
	class WallBox : public  GameObject {
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
	class SkyStage : public GameObject {
	public:
		SkyStage(const TransParam& param);
		virtual ~SkyStage();
		virtual void OnCreate(ID3D12GraphicsCommandList* pCommandList);
		virtual void OnUpdate(double elapsedTime);

	};



}
// end namespace basecross
