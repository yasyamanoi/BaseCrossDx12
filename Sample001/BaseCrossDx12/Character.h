/*!
@file Character.h
@brief ���܂��܂ȃI�u�W�F�N�g
*/

#pragma once

#include "stdafx.h"

using namespace bsm;
namespace basecross{
	//--------------------------------------------------------------------------------------
	//	PcSprite�N���X
	//--------------------------------------------------------------------------------------
	class PcSprite : public GameObject {
		Vec3 m_scale;
		Vec3 m_position;
		double m_totalTime;

		std::shared_ptr<BaseMesh> m_spriteMesh;
		std::vector<VertexPositionColor> m_backupVertices;
		std::vector<ConstBuffParam> m_constBuffParamVec;
	public:
		//�\�z�Ɣj��
		PcSprite();
		virtual ~PcSprite();
		//�������i�p�����[�^�t���j
		void OnCreate(ID3D12GraphicsCommandList* pCommandList,const Vec3& scale,const Vec3& position);
		//����
		virtual void OnUpdate(double elapsedTime) override;

		virtual void OnSceneDraw(ID3D12GraphicsCommandList* pCommandList) override;

	};


}
using namespace basecross;
// end namespace basecross