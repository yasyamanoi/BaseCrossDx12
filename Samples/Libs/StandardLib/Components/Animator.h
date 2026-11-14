/*!
@file Animator.h
@brief アニメーションコンポーネント（レイヤー、クロスフェード、ルートモーション、イベント、ボーン取り付け）
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	レイヤーの合成方法
	//--------------------------------------------------------------------------------------
	enum class AnimLayerBlend {
		Override,	///< 上書き（重み×マスクの分だけ、下の姿勢から置き換える）
		Additive	///< 加算（基準ポーズとの差分を、下の姿勢に足す）
	};

	//--------------------------------------------------------------------------------------
	///	アニメーションイベントの引数
	//--------------------------------------------------------------------------------------
	struct AnimationEventArgs {
		std::wstring clipName;		///< イベントを持つクリップ名
		std::wstring eventName;		///< イベント名
		std::wstring param;			///< パラメータ
		double time = 0.0;			///< クリップ内の時刻（秒）
		size_t layer = 0;			///< レイヤー番号
	};
	using AnimationEventCallback = std::function<void(const AnimationEventArgs&)>;
	using AnimationFinishedCallback = std::function<void(size_t layer, const std::wstring& clipName)>;

	//--------------------------------------------------------------------------------------
	///	Animatorコンポーネント。
	///	BaseModelのアニメーションを再生して、ボーンの行列を計算する。
	///	BcPNTModelDrawやShadowmapBoneが、ここで計算したボーン行列を使って描画する。
	///	- レイヤー：0番はベース層（自動で存在）。上位の層は、上書き（マスク付きにもできる）か加算で重ねる。
	///	- クロスフェード、再生速度（逆再生を含む）、ループ、一時停止、シーク
	///	- ルートモーション：データのまま／その場再生／移動量をTransformに反映
	///	- アニメーションイベント：コールバック、またはポーリング
	///	- ボーン取り付け：ボーンの行列の取得と、オブジェクトの追従
	///	更新順：このコンポーネントのOnUpdate（ComponentUpdate）で、ボーン行列・ルートモーション・取り付けが更新される。
	///	描画用の定数バッファは、その後に更新されるので、同じフレームの姿勢で描かれる。
	///	注意：イベントのコールバックの中で、ステージの切り替え（ResetActiveStage）をしてはいけない。
	//--------------------------------------------------------------------------------------
	class Animator : public Component {
		//再生状態（1クリップぶん）
		struct Playback {
			const AnimationClip* clip = nullptr;
			double timeU = 0.0;		//時刻（ループで巻き戻さない連続した値。ループしないときは0..長さに収める）
			double prevU = 0.0;		//前回の更新時の時刻（ルートモーション用）
			double evPrevU = 0.0;	//前回のイベント判定時の時刻
			float speed = 1.0f;
			bool loop = true;
			bool paused = false;
			bool startFlag = true;	//再生開始直後（時刻0のイベントを含める）
			bool finished = false;
			bool finishedNotified = false;
		};
		//レイヤー
		struct Layer {
			std::wstring name;
			AnimLayerBlend blend = AnimLayerBlend::Override;
			BoneMask mask;
			bool hasMask = false;
			float weight = 1.0f;
			float weightTarget = 1.0f;
			float weightRate = 0.0f;	//1秒あたりの重みの変化量（0なら即時）
			bool stopping = false;		//重みが0になったら止める
			Playback cur;
			Playback prev;				//クロスフェード中のフェードアウト側
			bool fading = false;
			float fadeProgress = 1.0f;	//0..1
			float fadeDuration = 0.0f;
			bool hasSnapshot = false;	//フェード中の割り込み時の、フェード元の姿勢
			LocalPose snapshot;
			LocalPose lastPose;			//直近に評価した、このレイヤー自身の姿勢
			enum class RefMode { FirstFrame, Clip, BindPose } refMode = RefMode::FirstFrame;
			std::wstring refClip;
			double refTime = 0.0;
			bool eventsEnabled = true;
		};
		//取り付け
		struct Attachment {
			std::weak_ptr<GameObject> object;
			int node = -1;
			Mat4x4 offset;
			bool inheritScale = true;
		};
		std::shared_ptr<BaseModel> m_model;
		std::vector<Layer> m_layers;
		RootMotionSettings m_rootMotion;
		RootMotionDelta m_lastRootMotion;
		LocalPose m_pose;
		std::vector<aiMatrix4x4> m_nodeGlobal;
		std::vector<aiMatrix4x4> m_skinMatrices;
		std::vector<Mat4x4> m_boneTransforms;
		std::vector<Attachment> m_attachments;
		std::vector<std::pair<size_t, AnimationEventCallback>> m_listeners;
		size_t m_nextListenerId;
		AnimationFinishedCallback m_finishedCallback;
		std::vector<AnimationEventArgs> m_firedEvents;

		static double WrapTime(const Playback& pb);
		void AdvancePlayback(Playback& pb, double dt);
		void AdvanceLayers(double dt);
		void EvaluatePose(bool allowRootMotion);
		void EvaluateLayer(size_t index, LocalPose& out, aiVector3D& rmTrans, float& rmYaw);
		bool SamplePlayback(const Playback& pb, LocalPose& pose, bool extractRoot, float rmWeight, aiVector3D& rmTrans, float& rmYaw) const;
		void ApplyRootMotion();
		void UpdateMatrices();
		void UpdateAttachments();
		void DispatchEvents(std::vector<AnimationEventArgs>& fired);
		Mat4x4 GetObjectWorldMatrix() const;
		Mat4x4 GetBoneModelMatrixByNode(int node) const;
		void CheckModel() const;
	public:
		Animator(const std::shared_ptr<GameObject>& gameObjectPtr);
		virtual ~Animator() {}
		//--------------------------------------------------------------------------------------
		// モデル
		//--------------------------------------------------------------------------------------
		//モデルを、Sceneに登録したキーで設定する
		void SetModel(const std::wstring& key);
		//モデルを設定する（複数のAnimatorで共有してよい）
		void SetModel(const std::shared_ptr<BaseModel>& model);
		//モデルを得る
		const std::shared_ptr<BaseModel>& GetModel() const {
			return m_model;
		}
		//--------------------------------------------------------------------------------------
		// レイヤー
		//--------------------------------------------------------------------------------------
		//レイヤーを追加する（戻り値はレイヤー番号。0番はベース層として最初からある）。maskがnullptrなら全身
		size_t AddLayer(const std::wstring& name, AnimLayerBlend blend, const BoneMask* mask = nullptr);
		//名前からレイヤー番号を得る（無ければSIZE_MAX）
		size_t FindLayer(const std::wstring& name) const;
		size_t GetLayerCount() const {
			return m_layers.size();
		}
		//レイヤーの重みを設定する。fadeSecが0より大きければ、その秒数をかけて変える
		void SetLayerWeight(size_t layer, float weight, float fadeSec = 0.0f);
		float GetLayerWeight(size_t layer) const;
		//レイヤーのマスクを設定する
		void SetLayerMask(size_t layer, const BoneMask& mask);
		//レイヤーのイベント発火を有効・無効にする（既定は有効）
		void SetLayerEventsEnabled(size_t layer, bool enabled);
		//加算層の基準ポーズ：別のクリップの指定時刻（T-Poseなど。長さ0のクリップも使える）
		void SetAdditiveReference(size_t layer, const std::wstring& refClip, double refTime = 0.0);
		//加算層の基準ポーズ：加算するクリップ自身の先頭フレーム（既定）
		void SetAdditiveReferenceToFirstFrame(size_t layer);
		//加算層の基準ポーズ：モデルのバインドポーズ
		void SetAdditiveReferenceToBindPose(size_t layer);
		//--------------------------------------------------------------------------------------
		// 再生
		//--------------------------------------------------------------------------------------
		//--------------------------------------------------------------------------------------
		/*!
		@brief	クリップを再生する
		@param[in]	clip	クリップ名（無ければ例外）
		@param[in]	fadeSec	クロスフェードの秒数（0なら即時に切り替える）
		@param[in]	loop	ループするか
		@param[in]	layer	レイヤー番号
		*/
		//--------------------------------------------------------------------------------------
		void Play(const std::wstring& clip, float fadeSec = 0.0f, bool loop = true, size_t layer = 0);
		//レイヤーを止める。上位層は重みを0までフェードして外す。ベース層は初期姿勢に戻る
		void Stop(size_t layer, float fadeSec = 0.0f);
		//クリップがあるか
		bool HasClip(const std::wstring& clip) const;
		//再生速度（負の値で逆再生）
		void SetSpeed(float speed, size_t layer = 0);
		float GetSpeed(size_t layer = 0) const;
		//一時停止・再開（layerにSIZE_MAXを指定すると全レイヤー）
		void Pause(size_t layer = SIZE_MAX);
		void Resume(size_t layer = SIZE_MAX);
		bool IsPaused(size_t layer = 0) const;
		//時刻を設定する（シーク）。既定ではイベントを出さず、ルートモーションも出さない
		void SetTime(double sec, size_t layer = 0, bool fireEvents = false);
		double GetTime(size_t layer = 0) const;
		//正規化した時刻（0..1。ループでは巻き戻した値）
		double GetNormalizedTime(size_t layer = 0) const;
		//ループしない再生が終端に達し、フェードも済んだか（ループ再生ではfalse）
		bool IsFinished(size_t layer = 0) const;
		//クロスフェード中か
		bool IsFading(size_t layer = 0) const;
		//現在のクリップ名（再生していなければ空）
		std::wstring GetCurrentClipName(size_t layer = 0) const;
		//--------------------------------------------------------------------------------------
		// ルートモーション（ベース層だけが対象）
		//--------------------------------------------------------------------------------------
		void SetRootMotion(const RootMotionSettings& rm);
		const RootMotionSettings& GetRootMotion() const {
			return m_rootMotion;
		}
		//直近のフレームの移動量（モデル空間）
		const RootMotionDelta& GetLastRootMotion() const {
			return m_lastRootMotion;
		}
		//--------------------------------------------------------------------------------------
		// イベント
		//--------------------------------------------------------------------------------------
		//イベントのコールバックを登録する（戻り値はID）
		size_t AddEventListener(const AnimationEventCallback& callback);
		void RemoveEventListener(size_t id);
		//直近のフレームで発火したイベント（ポーリング用）
		const std::vector<AnimationEventArgs>& GetFiredEvents() const {
			return m_firedEvents;
		}
		//ループしない再生が終わったときのコールバック
		void SetFinishedCallback(const AnimationFinishedCallback& callback) {
			m_finishedCallback = callback;
		}
		//--------------------------------------------------------------------------------------
		// ボーン取り付け
		//--------------------------------------------------------------------------------------
		//ボーン（ノード）のモデル空間の行列（行ベクトル向け）。無ければfalse
		bool GetBoneModelMatrix(const std::wstring& bone, Mat4x4& out) const;
		//ボーン（ノード）のワールド行列（このオブジェクトのTransformを含む。行ベクトル向け）。無ければfalse
		bool GetBoneWorldMatrix(const std::wstring& bone, Mat4x4& out) const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	オブジェクトをボーンに取り付ける。毎フレーム、このコンポーネントの更新でobjのTransformを書き換えて追従させる。
		取り付けたオブジェクトには、GravityやRigidBodyを付けないこと（位置を上書きされる）。
		このオブジェクト自身には親（Transformの親子付け）を設定しないこと（描画が親を見ないため、位置がずれる）
		@param[in]	bone	ボーン（ノード）名（例：L"handslot.r"）
		@param[in]	obj	取り付けるオブジェクト
		@param[in]	localOffset	ボーンからの相対の行列（行ベクトル向け）
		@param[in]	inheritScale	ボーンのスケールを引き継ぐか（falseなら、objのスケールを変えない）
		*/
		//--------------------------------------------------------------------------------------
		void AttachObject(const std::wstring& bone, const std::shared_ptr<GameObject>& obj,
			const Mat4x4& localOffset = Mat4x4(), bool inheritScale = true);
		void DetachObject(const std::shared_ptr<GameObject>& obj);
		//--------------------------------------------------------------------------------------
		// 出力
		//--------------------------------------------------------------------------------------
		//ボーンごとのスキン行列（シェーダーに渡す並び）
		const std::vector<Mat4x4>& GetBoneTransforms() const {
			return m_boneTransforms;
		}
		//姿勢を、今の状態から計算し直す（Playやシークの直後に、すぐ姿勢が必要なとき）
		void RefreshPose();
		virtual void OnUpdate(double elapsedTime) override;
	};

}
//end basecross
