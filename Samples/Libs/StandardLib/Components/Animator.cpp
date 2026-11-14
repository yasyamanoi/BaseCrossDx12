/*!
@file Animator.cpp
@brief アニメーションコンポーネント　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {

	Animator::Animator(const std::shared_ptr<GameObject>& gameObjectPtr) :
		Component(gameObjectPtr),
		m_nextListenerId(1)
	{
		//0番はベース層として、最初から用意する
		Layer base;
		base.name = L"Base";
		base.blend = AnimLayerBlend::Override;
		m_layers.push_back(base);
	}

	void Animator::CheckModel() const {
		if (!m_model) {
			throw BaseException(
				L"モデルが設定されていません",
				L"if (!m_model)",
				L"Animator"
			);
		}
	}

	double Animator::WrapTime(const Playback& pb) {
		if (!pb.clip) {
			return 0.0;
		}
		const double d = pb.clip->duration;
		if (d <= 0.0) {
			return 0.0;
		}
		if (pb.loop) {
			double t = std::fmod(pb.timeU, d);
			if (t < 0.0) {
				t += d;
			}
			return t;
		}
		return (std::max)(0.0, (std::min)(pb.timeU, d));
	}

	//--------------------------------------------------------------------------------------
	// モデル
	//--------------------------------------------------------------------------------------
	void Animator::SetModel(const std::wstring& key) {
		SetModel(BaseScene::Get()->GetModel(key));
	}

	void Animator::SetModel(const std::shared_ptr<BaseModel>& model) {
		m_model = model;
		for (auto& layer : m_layers) {
			layer.cur = Playback();
			layer.prev = Playback();
			layer.fading = false;
			layer.hasSnapshot = false;
			layer.stopping = false;
		}
		m_attachments.clear();
		m_lastRootMotion = RootMotionDelta();
		if (m_model) {
			m_pose = m_model->GetLoader().GetRestPose();
			UpdateMatrices();
		}
	}

	//--------------------------------------------------------------------------------------
	// レイヤー
	//--------------------------------------------------------------------------------------
	size_t Animator::AddLayer(const std::wstring& name, AnimLayerBlend blend, const BoneMask* mask) {
		Layer layer;
		layer.name = name;
		layer.blend = blend;
		if (mask) {
			layer.mask = *mask;
			layer.hasMask = true;
		}
		m_layers.push_back(layer);
		return m_layers.size() - 1;
	}

	size_t Animator::FindLayer(const std::wstring& name) const {
		for (size_t i = 0; i < m_layers.size(); i++) {
			if (m_layers[i].name == name) {
				return i;
			}
		}
		return SIZE_MAX;
	}

	void Animator::SetLayerWeight(size_t layer, float weight, float fadeSec) {
		if (layer >= m_layers.size()) {
			return;
		}
		Layer& L = m_layers[layer];
		weight = (std::max)(0.0f, (std::min)(1.0f, weight));
		L.stopping = false;
		L.weightTarget = weight;
		if (fadeSec <= 0.0f) {
			L.weight = weight;
			L.weightRate = 0.0f;
		}
		else {
			L.weightRate = std::fabs(weight - L.weight) / fadeSec;
		}
	}

	float Animator::GetLayerWeight(size_t layer) const {
		return layer < m_layers.size() ? m_layers[layer].weight : 0.0f;
	}

	void Animator::SetLayerMask(size_t layer, const BoneMask& mask) {
		if (layer < m_layers.size()) {
			m_layers[layer].mask = mask;
			m_layers[layer].hasMask = true;
		}
	}

	void Animator::SetLayerEventsEnabled(size_t layer, bool enabled) {
		if (layer < m_layers.size()) {
			m_layers[layer].eventsEnabled = enabled;
		}
	}

	void Animator::SetAdditiveReference(size_t layer, const std::wstring& refClip, double refTime) {
		if (layer < m_layers.size()) {
			m_layers[layer].refMode = Layer::RefMode::Clip;
			m_layers[layer].refClip = refClip;
			m_layers[layer].refTime = refTime;
		}
	}

	void Animator::SetAdditiveReferenceToFirstFrame(size_t layer) {
		if (layer < m_layers.size()) {
			m_layers[layer].refMode = Layer::RefMode::FirstFrame;
		}
	}

	void Animator::SetAdditiveReferenceToBindPose(size_t layer) {
		if (layer < m_layers.size()) {
			m_layers[layer].refMode = Layer::RefMode::BindPose;
		}
	}

	//--------------------------------------------------------------------------------------
	// 再生
	//--------------------------------------------------------------------------------------
	void Animator::Play(const std::wstring& clip, float fadeSec, bool loop, size_t layer) {
		CheckModel();
		if (layer >= m_layers.size()) {
			throw BaseException(
				L"レイヤー番号が範囲外です",
				std::to_wstring(layer),
				L"Animator::Play()"
			);
		}
		const AnimationClip* pClip = m_model->GetLoader().FindClip(clip);
		if (!pClip) {
			throw BaseException(
				L"クリップが見つかりません",
				clip,
				L"Animator::Play()"
			);
		}
		Layer& L = m_layers[layer];
		Playback next;
		next.clip = pClip;
		next.loop = loop;
		if (L.cur.clip) {
			//速度と一時停止の状態は引き継ぐ
			next.speed = L.cur.speed;
			next.paused = L.cur.paused;
		}
		if (fadeSec > 0.0f && L.cur.clip) {
			if (L.fading) {
				//フェード中の割り込み。今の合成結果を、フェード元の静止ポーズにする
				L.snapshot = L.lastPose;
				L.hasSnapshot = true;
				L.prev = Playback();
			}
			else {
				L.prev = L.cur;
				L.hasSnapshot = false;
			}
			L.fading = true;
			L.fadeProgress = 0.0f;
			L.fadeDuration = fadeSec;
		}
		else {
			L.fading = false;
			L.prev = Playback();
			L.hasSnapshot = false;
		}
		L.cur = next;
		//Stopで外した層（フェード中を含む）に、再びPlayしたら、重みを1に戻して有効にする
		const bool wasStopped = L.stopping || L.weightTarget <= 0.0f;
		L.stopping = false;
		if (layer > 0 && wasStopped) {
			L.weightTarget = 1.0f;
			if (fadeSec > 0.0f) {
				L.weightRate = (1.0f - L.weight) / fadeSec;
			}
			else {
				L.weight = 1.0f;
				L.weightRate = 0.0f;
			}
		}
		RefreshPose();
	}

	void Animator::Stop(size_t layer, float fadeSec) {
		if (layer >= m_layers.size()) {
			return;
		}
		Layer& L = m_layers[layer];
		if (layer == 0 || fadeSec <= 0.0f) {
			L.cur = Playback();
			L.prev = Playback();
			L.fading = false;
			L.hasSnapshot = false;
			L.stopping = false;
			if (layer > 0) {
				L.weight = 0.0f;
				L.weightTarget = 0.0f;
				L.weightRate = 0.0f;
			}
			if (m_model) {
				RefreshPose();
			}
			return;
		}
		L.weightTarget = 0.0f;
		L.weightRate = L.weight / fadeSec;
		L.stopping = true;
	}

	bool Animator::HasClip(const std::wstring& clip) const {
		return m_model && m_model->HasClip(clip);
	}

	void Animator::SetSpeed(float speed, size_t layer) {
		if (layer < m_layers.size()) {
			m_layers[layer].cur.speed = speed;
			m_layers[layer].prev.speed = speed;
		}
	}

	float Animator::GetSpeed(size_t layer) const {
		return layer < m_layers.size() ? m_layers[layer].cur.speed : 1.0f;
	}

	void Animator::Pause(size_t layer) {
		for (size_t i = 0; i < m_layers.size(); i++) {
			if (layer == SIZE_MAX || layer == i) {
				m_layers[i].cur.paused = true;
				m_layers[i].prev.paused = true;
			}
		}
	}

	void Animator::Resume(size_t layer) {
		for (size_t i = 0; i < m_layers.size(); i++) {
			if (layer == SIZE_MAX || layer == i) {
				m_layers[i].cur.paused = false;
				m_layers[i].prev.paused = false;
			}
		}
	}

	bool Animator::IsPaused(size_t layer) const {
		return layer < m_layers.size() && m_layers[layer].cur.paused;
	}

	void Animator::SetTime(double sec, size_t layer, bool fireEvents) {
		if (layer >= m_layers.size() || !m_layers[layer].cur.clip) {
			return;
		}
		Playback& pb = m_layers[layer].cur;
		double t = sec;
		if (!pb.loop) {
			t = (std::max)(0.0, (std::min)(t, pb.clip->duration));
		}
		pb.timeU = t;
		//ルートモーションは出さない（テレポート）
		pb.prevU = t;
		if (!fireEvents) {
			pb.evPrevU = t;
		}
		pb.finished = false;
		pb.finishedNotified = false;
		RefreshPose();
	}

	double Animator::GetTime(size_t layer) const {
		return layer < m_layers.size() ? WrapTime(m_layers[layer].cur) : 0.0;
	}

	double Animator::GetNormalizedTime(size_t layer) const {
		if (layer >= m_layers.size() || !m_layers[layer].cur.clip) {
			return 0.0;
		}
		const double d = m_layers[layer].cur.clip->duration;
		return d > 0.0 ? WrapTime(m_layers[layer].cur) / d : 0.0;
	}

	bool Animator::IsFinished(size_t layer) const {
		if (layer >= m_layers.size()) {
			return true;
		}
		const Layer& L = m_layers[layer];
		if (!L.cur.clip) {
			return true;
		}
		return L.cur.finished && !L.fading;
	}

	bool Animator::IsFading(size_t layer) const {
		return layer < m_layers.size() && m_layers[layer].fading;
	}

	std::wstring Animator::GetCurrentClipName(size_t layer) const {
		if (layer < m_layers.size() && m_layers[layer].cur.clip) {
			return m_layers[layer].cur.clip->name;
		}
		return std::wstring();
	}

	void Animator::SetRootMotion(const RootMotionSettings& rm) {
		m_rootMotion = rm;
		if (m_model && m_model->GetLoader().FindNode(rm.motionNode) < 0 && rm.mode != RootMotionMode::AsAuthored) {
			ModelUtil::Warn(L"Animator: root motion node not found: " + rm.motionNode);
		}
	}

	//--------------------------------------------------------------------------------------
	// イベント
	//--------------------------------------------------------------------------------------
	size_t Animator::AddEventListener(const AnimationEventCallback& callback) {
		const size_t id = m_nextListenerId++;
		m_listeners.push_back({ id, callback });
		return id;
	}

	void Animator::RemoveEventListener(size_t id) {
		for (auto it = m_listeners.begin(); it != m_listeners.end(); ++it) {
			if (it->first == id) {
				m_listeners.erase(it);
				return;
			}
		}
	}

	void Animator::DispatchEvents(std::vector<AnimationEventArgs>& fired) {
		m_firedEvents = fired;
		if (fired.empty() || m_listeners.empty()) {
			return;
		}
		//コールバックの中でリスナーの追加・削除や再生の変更をされても大丈夫なように、コピーして呼ぶ
		const auto listeners = m_listeners;
		for (auto& args : fired) {
			for (auto& l : listeners) {
				if (l.second) {
					l.second(args);
				}
			}
		}
	}

	//--------------------------------------------------------------------------------------
	// 更新
	//--------------------------------------------------------------------------------------
	void Animator::AdvancePlayback(Playback& pb, double dt) {
		if (!pb.clip) {
			return;
		}
		pb.prevU = pb.timeU;
		if (!pb.paused) {
			pb.timeU += dt * static_cast<double>(pb.speed);
		}
		const double d = pb.clip->duration;
		if (!pb.loop) {
			pb.timeU = d > 0.0 ? (std::max)(0.0, (std::min)(pb.timeU, d)) : 0.0;
			if (d <= 0.0 || (pb.speed >= 0.0f && pb.timeU >= d) || (pb.speed < 0.0f && pb.timeU <= 0.0)) {
				pb.finished = true;
			}
		}
	}

	void Animator::AdvanceLayers(double dt) {
		for (auto& L : m_layers) {
			//重みのフェード
			if (L.weightRate > 0.0f && L.weight != L.weightTarget) {
				const float step = L.weightRate * static_cast<float>(dt);
				if (L.weight < L.weightTarget) {
					L.weight = (std::min)(L.weightTarget, L.weight + step);
				}
				else {
					L.weight = (std::max)(L.weightTarget, L.weight - step);
				}
			}
			if (L.stopping && L.weight <= 0.0f) {
				L.cur = Playback();
				L.prev = Playback();
				L.fading = false;
				L.hasSnapshot = false;
				L.stopping = false;
			}
			AdvancePlayback(L.cur, dt);
			if (L.fading) {
				AdvancePlayback(L.prev, dt);
				L.fadeProgress += L.fadeDuration > 0.0f ? static_cast<float>(dt) / L.fadeDuration : 1.0f;
				if (L.fadeProgress >= 1.0f) {
					L.fading = false;
					L.fadeProgress = 1.0f;
					L.prev = Playback();
					L.hasSnapshot = false;
				}
			}
		}
	}

	bool Animator::SamplePlayback(const Playback& pb, LocalPose& pose, bool extractRoot, float rmWeight,
		aiVector3D& rmTrans, float& rmYaw) const {
		if (!pb.clip) {
			return false;
		}
		const ModelLoader& loader = m_model->GetLoader();
		loader.SampleClip(*pb.clip, WrapTime(pb), pose);
		if (extractRoot && m_rootMotion.mode != RootMotionMode::AsAuthored) {
			RootMotionDelta d;
			loader.ExtractRootMotion(*pb.clip, m_rootMotion, pb.prevU, pb.timeU, pb.loop, pose, d);
			rmTrans += d.translation * rmWeight;
			rmYaw += d.yaw * rmWeight;
		}
		return true;
	}

	void Animator::EvaluateLayer(size_t index, LocalPose& out, aiVector3D& rmTrans, float& rmYaw) {
		Layer& L = m_layers[index];
		const bool allowRoot = (index == 0);
		const float wCur = L.fading ? L.fadeProgress : 1.0f;
		LocalPose curPose;
		//ルートモーションは、姿勢から移動分を完全に取り除いているので、移動量も完全に（重み1で）反映する。
		//クロスフェードの重みで減らすと、ループしない再生（倒れるなど）の最終位置が、クリップの移動量より手前になってしまう。
		//フェードアウト側は、残りの重み（1 - フェードの進み）で反映する
		SamplePlayback(L.cur, curPose, allowRoot, 1.0f, rmTrans, rmYaw);
		if (L.fading) {
			LocalPose prevPose;
			if (L.prev.clip) {
				SamplePlayback(L.prev, prevPose, allowRoot, 1.0f - wCur, rmTrans, rmYaw);
			}
			else if (L.hasSnapshot) {
				prevPose = L.snapshot;
			}
			else {
				prevPose = curPose;
			}
			ModelLoader::BlendOverride(prevPose, curPose, wCur, nullptr, out);
		}
		else {
			out = curPose;
		}
		L.lastPose = out;
	}

	void Animator::EvaluatePose(bool allowRootMotion) {
		const ModelLoader& loader = m_model->GetLoader();
		LocalPose result = loader.GetRestPose();
		aiVector3D rmTrans(0.0f, 0.0f, 0.0f);
		float rmYaw = 0.0f;
		for (size_t i = 0; i < m_layers.size(); i++) {
			Layer& L = m_layers[i];
			if (!L.cur.clip || L.weight <= 0.0f) {
				continue;
			}
			LocalPose layerPose;
			aiVector3D layerTrans(0.0f, 0.0f, 0.0f);
			float layerYaw = 0.0f;
			EvaluateLayer(i, layerPose, layerTrans, layerYaw);
			if (i == 0) {
				rmTrans += layerTrans * L.weight;
				rmYaw += layerYaw * L.weight;
			}
			const BoneMask* mask = L.hasMask ? &L.mask : nullptr;
			if (L.blend == AnimLayerBlend::Override) {
				ModelLoader::BlendOverride(result, layerPose, L.weight, mask, result);
			}
			else {
				//加算：基準ポーズとの差分を足す
				LocalPose ref;
				if (L.refMode == Layer::RefMode::BindPose) {
					ref = loader.GetRestPose();
				}
				else if (L.refMode == Layer::RefMode::Clip) {
					const AnimationClip* refClip = loader.FindClip(L.refClip);
					if (refClip) {
						const double t = refClip->duration > 0.0 ? (std::max)(0.0, (std::min)(L.refTime, refClip->duration)) : 0.0;
						loader.SampleClip(*refClip, t, ref);
					}
					else {
						ref = loader.GetRestPose();
					}
				}
				else {
					loader.SampleClip(*L.cur.clip, 0.0, ref);
				}
				ModelLoader::ApplyAdditive(result, layerPose, ref, L.weight, mask);
			}
		}
		m_pose = result;
		if (allowRootMotion) {
			m_lastRootMotion.translation = rmTrans;
			m_lastRootMotion.yaw = rmYaw;
		}
		else {
			m_lastRootMotion = RootMotionDelta();
		}
	}

	void Animator::ApplyRootMotion() {
		if (m_rootMotion.mode != RootMotionMode::Extract) {
			return;
		}
		const aiVector3D& t = m_lastRootMotion.translation;
		if (t.x == 0.0f && t.y == 0.0f && t.z == 0.0f && m_lastRootMotion.yaw == 0.0f) {
			return;
		}
		auto trans = GetGameObject()->GetComponent<Transform>();
		const Vec3 scale = trans->GetScale();
		const Quat qt = trans->GetQuaternion();
		//モデル空間の移動量を、オブジェクトの向きと大きさでワールド空間にする
		XMVECTOR d = XMVectorSet(t.x * scale.x, t.y * scale.y, t.z * scale.z, 0.0f);
		d = XMVector3Rotate(d, (XMVECTOR)qt);
		trans->SetPosition(trans->GetPosition() + Vec3(d));
		if (m_lastRootMotion.yaw != 0.0f) {
			XMVECTOR ry = XMQuaternionRotationAxis(g_XMIdentityR1, m_lastRootMotion.yaw);
			trans->SetQuaternion(Quat(XMQuaternionMultiply(ry, (XMVECTOR)qt)));
		}
	}

	void Animator::UpdateMatrices() {
		const ModelLoader& loader = m_model->GetLoader();
		loader.ComputeGlobals(m_pose, m_nodeGlobal);
		loader.ComputeSkinMatrices(m_nodeGlobal, m_skinMatrices);
		m_boneTransforms.resize(m_skinMatrices.size());
		for (size_t i = 0; i < m_skinMatrices.size(); i++) {
			//Assimpの行列のまま（行ごと）シェーダーに渡す
			m_boneTransforms[i] = Mat4x4(m_skinMatrices[i]);
		}
	}

	Mat4x4 Animator::GetObjectWorldMatrix() const {
		auto trans = GetGameObject()->GetComponent<Transform>();
		const TransParam& p = trans->GetTransParam();
		//描画（BcPNTBoneDrawなど）と同じ式でワールド行列を作る
		return Mat4x4(XMMatrixAffineTransformation(p.scale, p.rotOrigin, p.quaternion, p.position));
	}

	Mat4x4 Animator::GetBoneModelMatrixByNode(int node) const {
		if (node < 0 || static_cast<size_t>(node) >= m_nodeGlobal.size()) {
			return Mat4x4();
		}
		//Assimpの行列（列ベクトル向け）を、行ベクトル向けにする（転置）
		Mat4x4 m(m_nodeGlobal[node]);
		m.transpose();
		return m;
	}

	bool Animator::GetBoneModelMatrix(const std::wstring& bone, Mat4x4& out) const {
		if (!m_model) {
			return false;
		}
		const int node = m_model->GetLoader().FindNode(bone);
		if (node < 0) {
			return false;
		}
		out = GetBoneModelMatrixByNode(node);
		return true;
	}

	bool Animator::GetBoneWorldMatrix(const std::wstring& bone, Mat4x4& out) const {
		Mat4x4 boneModel;
		if (!GetBoneModelMatrix(bone, boneModel)) {
			return false;
		}
		out = Mat4x4(XMMatrixMultiply((XMMATRIX)boneModel, (XMMATRIX)GetObjectWorldMatrix()));
		return true;
	}

	void Animator::AttachObject(const std::wstring& bone, const std::shared_ptr<GameObject>& obj,
		const Mat4x4& localOffset, bool inheritScale) {
		CheckModel();
		const int node = m_model->GetLoader().FindNode(bone);
		if (node < 0) {
			throw BaseException(
				L"取り付け先のボーンが見つかりません",
				bone,
				L"Animator::AttachObject()"
			);
		}
		DetachObject(obj);
		Attachment a;
		a.object = obj;
		a.node = node;
		a.offset = localOffset;
		a.inheritScale = inheritScale;
		m_attachments.push_back(a);
		UpdateAttachments();
		//取り付けた最初のフレームで、原点からの移動として衝突判定されないように、前回の値も合わせる
		auto trans = obj->GetComponent<Transform>(false);
		if (trans) {
			trans->SetToBefore();
		}
	}

	void Animator::DetachObject(const std::shared_ptr<GameObject>& obj) {
		for (auto it = m_attachments.begin(); it != m_attachments.end();) {
			auto locked = it->object.lock();
			if (!locked || locked == obj) {
				it = m_attachments.erase(it);
			}
			else {
				++it;
			}
		}
	}

	void Animator::UpdateAttachments() {
		if (m_attachments.empty()) {
			return;
		}
		const XMMATRIX objectWorld = (XMMATRIX)GetObjectWorldMatrix();
		for (auto it = m_attachments.begin(); it != m_attachments.end();) {
			auto obj = it->object.lock();
			if (!obj) {
				it = m_attachments.erase(it);
				continue;
			}
			auto trans = obj->GetComponent<Transform>(false);
			if (trans) {
				const XMMATRIX boneWorld = XMMatrixMultiply((XMMATRIX)GetBoneModelMatrixByNode(it->node), objectWorld);
				const XMMATRIX m = XMMatrixMultiply((XMMATRIX)it->offset, boneWorld);
				XMVECTOR s, r, t;
				if (XMMatrixDecompose(&s, &r, &t, m)) {
					trans->SetPosition(Vec3(t));
					trans->SetQuaternion(Quat(r));
					if (it->inheritScale) {
						trans->SetScale(Vec3(s));
					}
				}
			}
			++it;
		}
	}

	void Animator::RefreshPose() {
		if (!m_model) {
			return;
		}
		EvaluatePose(false);
		UpdateMatrices();
	}

	void Animator::OnUpdate(double elapsedTime) {
		if (!m_model) {
			return;
		}
		AdvanceLayers(elapsedTime);
		EvaluatePose(true);
		ApplyRootMotion();
		UpdateMatrices();
		UpdateAttachments();
		//イベントを集める（姿勢がすべて確定した後に配るので、コールバック内でボーン行列を使っても今フレームの値になる）
		std::vector<AnimationEventArgs> fired;
		struct FinishedNote { size_t layer; std::wstring clip; };
		std::vector<FinishedNote> finishedNotes;
		for (size_t i = 0; i < m_layers.size(); i++) {
			Layer& L = m_layers[i];
			Playback& pb = L.cur;
			if (!pb.clip) {
				continue;
			}
			if (L.eventsEnabled && L.weight > 0.0f && !pb.clip->events.empty()) {
				std::vector<const AnimationEvent*> events;
				ModelLoader::CollectEvents(*pb.clip, pb.evPrevU, pb.timeU, pb.loop, pb.startFlag, events);
				for (auto* e : events) {
					AnimationEventArgs args;
					args.clipName = pb.clip->name;
					args.eventName = e->name;
					args.param = e->param;
					args.time = e->time;
					args.layer = i;
					fired.push_back(args);
				}
			}
			pb.evPrevU = pb.timeU;
			pb.startFlag = false;
			if (pb.finished && !pb.finishedNotified && !L.fading) {
				pb.finishedNotified = true;
				finishedNotes.push_back({ i, pb.clip->name });
			}
		}
		DispatchEvents(fired);
		if (m_finishedCallback) {
			const auto callback = m_finishedCallback;
			for (auto& note : finishedNotes) {
				callback(note.layer, note.clip);
			}
		}
	}

}
//end basecross
