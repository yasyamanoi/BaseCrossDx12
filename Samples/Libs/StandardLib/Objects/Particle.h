/*!
@file Particle.h
@brief パーティクルクラス
*/


#pragma once
#include "stdafx.h"

namespace basecross {


	struct ParticleParem;
	class Particle : public ObjectInterface {
	public:
		enum DrawOption {
			Billboard,	//ビルボード
			Faceing,	//フェイシング
			FaceingY,	//フェイシング(Y軸のみ)
			Normal		//通常(手動)
		};
		//構築時パラメータ
		DrawOption m_drawOption;		//表示オプション
		Vec3 m_emitterPos;			//エミッター位置
		float m_totalTime;				//有効時間
		std::shared_ptr<BaseMesh> m_baseMesh;	//メッシュ
		std::weak_ptr<BaseTexture> m_texture;	//テクスチャ
		std::wstring m_name; //名前
		//ランタイム時パラメータ
		float m_nowTime;

		explicit Particle(const ParticleParem& param);

		virtual void OnCreate()override {}
		virtual void OnUpdate(double elapsedTime)override {}
	};

	//Particleを作り出すためのパラメータ
	struct ParticleParem {
		Particle::DrawOption m_drawOption;		//表示オプション
		Vec3 m_emitterPos;			//エミッター位置
		float m_totalTime;				//有効時間
		std::shared_ptr<BaseMesh> m_baseMesh;	//メッシュ
		std::shared_ptr<BaseTexture> m_texture;	//テクスチャ
		bool m_addtype; ////加算処理するかどうか
		std::wstring m_name; //名前
	};



}
// end namespace basecross
