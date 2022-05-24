/*!
@file BcStaticRender.h
@brief BasicStatic描画コンポーネント群
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	DECLARE_DX12SHADER(BcVSPNTStaticPL)
	DECLARE_DX12SHADER(BcPSPNTPL)

	//--------------------------------------------------------------------------------------
	///	BcStatic描画コンポーネント
	//--------------------------------------------------------------------------------------
	class BcStaticRender : public Component {
	protected:
		explicit BcStaticRender(const shared_ptr<GameObject>& gameObjectPtr) :
			Component(gameObjectPtr)
		{}
		virtual ~BcStaticRender() {}
		//コンスタントバッファの登録
		void SetConstants(BasicConstants& constants, const shared_ptr<Transform>& transform);
	public:
	};


	//--------------------------------------------------------------------------------------
	///	BcPNTStatic描画コンポーネント
	//--------------------------------------------------------------------------------------
	class BcPNTStaticRender : public BcStaticRender {
		void CreatePipelineStates();
		ComPtr<ID3D12PipelineState> m_defaultPipelineState;
		ComPtr<ID3D12PipelineState> m_alphaPipelineState;
	protected:
		virtual void PopulateCommandList(BaseFrame* pBaseFrame)override;
	public:
		explicit BcPNTStaticRender(const shared_ptr<GameObject>& gameObjectPtr);
		virtual ~BcPNTStaticRender() {}
		//操作
		virtual void OnInitFrame(BaseFrame* pBaseFrame)override;
		virtual void WriteConstantBuffers(BaseFrame* pBaseFrame)override;
		virtual void OnCreate()override;
		virtual void OnUpdate()override {}
		virtual void OnDestroy()override {}
	};

}
// end namespace basecross
