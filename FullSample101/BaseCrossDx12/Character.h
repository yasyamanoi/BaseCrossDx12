/*!
@file Character.h
@brief キャラクタークラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	class FixedBox : public GameObject;
	//--------------------------------------------------------------------------------------
	class FixedBox : public GameObject {
		Vec3 m_scale;
		Vec3 m_rotation;
		Vec3 m_position;
	public:
		//構築と破棄
		FixedBox(const shared_ptr<Stage>& stage,
			const Vec3& scale,
			const Vec3& rotation,
			const Vec3& position
		);
		virtual ~FixedBox() {}
		//初期化
		virtual void OnInit() override;
		//操作
	};


	class WallObject : public GameObject {
		float m_posSpan;
	public:
		WallObject(const shared_ptr<Stage>& stage) :GameObject(stage), m_posSpan(1.0f) {}
		virtual ~WallObject() {}
		virtual void OnInit()override;
		virtual void OnUpdate() override;

	};

	//--------------------------------------------------------------------------------------
	///	壁模様のスプライト
	//--------------------------------------------------------------------------------------
	class WallSprite : public GameObject {
		bool m_Trace;
		Vec2 m_StartScale;
		Vec3 m_StartPos;
		wstring m_TextureKey;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief コンストラクタ
		@param[in]	StagePtr	ステージ
		@param[in]	TextureKey	テクスチャキー
		@param[in]	Trace	透明処理するかどうか
		@param[in]	StartScale	初期スケール
		@param[in]	StartPos	初期位置
		*/
		//--------------------------------------------------------------------------------------
		WallSprite(const shared_ptr<Stage>& StagePtr, const wstring& TextureKey, bool Trace,
			const Vec2& StartScale, const Vec3& StartPos);
		//--------------------------------------------------------------------------------------
		/*!
		@brief デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~WallSprite();
		//--------------------------------------------------------------------------------------
		/*!
		@brief 初期化
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnInit() override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief 更新
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate()override {}
	};


	//--------------------------------------------------------------------------------------
	///	スクロールするスプライト
	//--------------------------------------------------------------------------------------
	class ScrollSprite : public GameObject {
		bool m_Trace;
		Vec2 m_StartScale;
		Vec3 m_StartPos;
		wstring m_TextureKey;
		float m_TotalTime;
		//バックアップ頂点データ
		vector<VertexPositionTexture> m_BackupVertices;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief コンストラクタ
		@param[in]	StagePtr	ステージ
		@param[in]	TextureKey	テクスチャキー
		@param[in]	Trace	透明処理するかどうか
		@param[in]	StartScale	初期スケール
		@param[in]	StartPos	初期位置
		*/
		//--------------------------------------------------------------------------------------
		ScrollSprite(const shared_ptr<Stage>& StagePtr, const wstring& TextureKey, bool Trace,
			const Vec2& StartScale, const Vec3& StartPos);
		//--------------------------------------------------------------------------------------
		/*!
		@brief デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~ScrollSprite();
		//--------------------------------------------------------------------------------------
		/*!
		@brief 初期化
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnInit() override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief 更新
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate()override;
	};

	//--------------------------------------------------------------------------------------
	///	スコア表示のスプライト
	//--------------------------------------------------------------------------------------
	class ScoreSprite : public GameObject {
		bool m_Trace;
		Vec2 m_StartScale;
		Vec3 m_StartPos;
		wstring m_TextureKey;
		float m_Score;
		//桁数
		UINT m_NumberOfDigits;
		//バックアップ頂点データ
		vector<VertexPositionTexture> m_BackupVertices;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief コンストラクタ
		@param[in]	StagePtr	ステージ
		@param[in]	NumberOfDigits	桁数
		@param[in]	TextureKey	テクスチャキー
		@param[in]	Trace	透明処理するかどうか
		@param[in]	StartScale	初期スケール
		@param[in]	StartPos	初期位置
		*/
		//--------------------------------------------------------------------------------------
		ScoreSprite(const shared_ptr<Stage>& StagePtr, UINT NumberOfDigits,
			const wstring& TextureKey, bool Trace,
			const Vec2& StartScale, const Vec3& StartPos);
		//--------------------------------------------------------------------------------------
		/*!
		@brief デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~ScoreSprite() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief スコアのセット
		@param[in]	f	値
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetScore(float f) {
			m_Score = f;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief 初期化
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnInit() override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief 更新
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate()override;
	};

	//--------------------------------------------------------------------------------------
	///	PC球
	//--------------------------------------------------------------------------------------
	class PcSphere : public GameObject {
		Vec3 m_StartPos;
	public:
		//構築と破棄
		PcSphere(const shared_ptr<Stage>& StagePtr, const Vec3& StartPos);
		virtual ~PcSphere();
		//初期化
		virtual void OnInit() override;

	};

	//--------------------------------------------------------------------------------------
	///	PT球
	//--------------------------------------------------------------------------------------
	class PtSphere : public GameObject {
		Vec3 m_StartPos;
	public:
		//構築と破棄
		PtSphere(const shared_ptr<Stage>& StagePtr, const Vec3& StartPos);
		virtual ~PtSphere();
		//初期化
		virtual void OnInit() override;
	};

	//--------------------------------------------------------------------------------------
	///	PN球
	//--------------------------------------------------------------------------------------
	class PnSphere : public GameObject {
		Vec3 m_StartPos;
	public:
		//構築と破棄
		PnSphere(const shared_ptr<Stage>& StagePtr, const Vec3& StartPos);
		virtual ~PnSphere();
		//初期化
		virtual void OnInit() override;

	};

	//--------------------------------------------------------------------------------------
	///	PCT球
	//--------------------------------------------------------------------------------------
	class PctSphere : public GameObject {
		Vec3 m_StartPos;
	public:
		//構築と破棄
		PctSphere(const shared_ptr<Stage>& StagePtr, const Vec3& StartPos);
		virtual ~PctSphere();
		//初期化
		virtual void OnInit() override;
	};

	//--------------------------------------------------------------------------------------
	///	Pnt球
	//--------------------------------------------------------------------------------------
	class PntSphere : public GameObject {
		Vec3 m_StartPos;
		bool m_TextureUse;
	public:
		//構築と破棄
		PntSphere(const shared_ptr<Stage>& StagePtr, const Vec3& StartPos, bool TextureUse);
		virtual ~PntSphere();
		//初期化
		virtual void OnInit() override;
	};

}
//end namespace basecross
