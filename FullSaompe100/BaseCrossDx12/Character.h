/*!
@file Character.h
@brief キャラクタークラス
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	class WallObject : public GameObject {
		float m_posSpan;
	public:
		WallObject(const shared_ptr<BaseStage>& stage) :GameObject(stage), m_posSpan(1.0f) {}
		virtual ~WallObject() {}
		virtual void OnInit()override;
		virtual void OnUpdate() override;
		virtual void OnDestroy() override;

	};

	class SkyObject : public GameObject {
	public:
		SkyObject(const shared_ptr<BaseStage>& stage) :GameObject(stage) {}
		virtual ~SkyObject() {}
		virtual void OnInit()override;
		virtual void OnUpdate() override;
		virtual void OnDestroy() override;
	};


}
//end basecross
