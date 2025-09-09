/*!
@file Character.h
@brief �z�u�I�u�W�F�N�g
*/


#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// �{�b�N�X�I�u�W�F�N�g
	//--------------------------------------------------------------------------------------
	class FixedBox : public GameObject {
	public:
		FixedBox(const std::shared_ptr<Stage>& stage, const TransParam& param);
		virtual ~FixedBox();
		virtual void OnCreate()override;
		virtual void OnUpdate(double elapsedTime)override {}
	};

	//--------------------------------------------------------------------------------------
	// ���f���I�u�W�F�N�g
	//--------------------------------------------------------------------------------------
	class FixedModel : public GameObject {
		std::shared_ptr<BaseMesh> m_baseMesh;
		double m_totalTime;
	public:
		FixedModel(const std::shared_ptr<Stage>& stage, const TransParam& param);
		virtual ~FixedModel();
		virtual void OnCreate()override;
		virtual void OnUpdate(double elapsedTime)override;
	};


}
// end namespace basecross
