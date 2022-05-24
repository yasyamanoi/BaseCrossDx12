/*!
@file BaseHelper.cpp
@brief ヘルパークラス、関数群
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	///	イベント配送クラス
	//--------------------------------------------------------------------------------------
	//構築と破棄
	EventDispatcher::EventDispatcher() 
	{}
	EventDispatcher::~EventDispatcher() {}

	void EventDispatcher::AddEventReceiverGroup(const wstring& groupKey, const shared_ptr<ObjectInterface>& receiver) {
		auto it = m_eventInterfaceGroupMap.find(groupKey);
		if (it != m_eventInterfaceGroupMap.end()) {
			//キーがあった
			it->second.push_back(receiver);
		}
		else {
			//グループがない
			vector<weak_ptr<ObjectInterface>> vec;
			m_eventInterfaceGroupMap[groupKey] = vec;
			m_eventInterfaceGroupMap[groupKey].push_back(receiver);
		}
	}


	//イベントのPOST（キューに入れる）
	void EventDispatcher::PostEvent(float delay, const shared_ptr<ObjectInterface>& sender, const shared_ptr<ObjectInterface>& receiver,
		const wstring& msgStr, const  shared_ptr<void>& info) {
		//イベントの作成 
		auto ptr = make_shared<Event>(delay, sender, receiver, msgStr, info);
		//キューにためる
		m_priorityQ.push_back(ptr);
	}

	void EventDispatcher::PostEvent(float dispatchTime, const shared_ptr<ObjectInterface>& sender, const wstring& receiverKey,
		const wstring& msgStr, const  shared_ptr<void>& info) {
		//receiverKeyによる相手の特定
		//重複キーの検査
		auto it = m_eventInterfaceGroupMap.find(receiverKey);
		if (it != m_eventInterfaceGroupMap.end()) {
			//キーがあった
			for (auto v : it->second) {
				auto shptr = v.lock();
				if (shptr) {
					//イベントの作成 
					auto ptr = make_shared<Event>(0.0f, sender, shptr, msgStr, info);
					//キューにためる
					m_priorityQ.push_back(ptr);
				}
			}
		}
		//キーが見つからなくても何もしない
	}


	//イベントのSEND（キューに入れずにそのまま送る）
	void EventDispatcher::SendEvent(const shared_ptr<ObjectInterface>& sender, const shared_ptr<ObjectInterface>& receiver,
		const wstring& msgStr, const  shared_ptr<void>& info) {
		//イベントの作成 
		auto ptr = make_shared<Event>(0.0f, sender, receiver, msgStr, info);
		//送信
		Discharge(ptr);
	}

	void EventDispatcher::SendEvent(const shared_ptr<ObjectInterface>& sender, const wstring& receiverKey,
		const wstring& msgStr, const  shared_ptr<void>& info) {
		//receiverKeyによる相手の特定
		//重複キーの検査
		auto it = m_eventInterfaceGroupMap.find(receiverKey);
		if (it != m_eventInterfaceGroupMap.end()) {
			//キーがあった
			for (auto v : it->second) {
				auto shptr = v.lock();
				if (shptr) {
					//イベントの作成 
					auto Ptr = make_shared<Event>(0.0f, sender, shptr, msgStr, info);
					//イベントの送出
					Discharge(Ptr);
				}
			}
		}
		//キーが見つからなくても何もしない
	}


	void EventDispatcher::DispatchDelayedEvent() {
		//前回のターンからの時間
		float elapsedTime = App::GetElapsedTime();
		auto it = m_priorityQ.begin();
		while (it != m_priorityQ.end()) {
			(*it)->m_dispatchTime -= elapsedTime;
			if ((*it)->m_dispatchTime <= 0.0f) {
				(*it)->m_dispatchTime = 0.0f;
				//メッセージの送信
				Discharge(*it);
				//キューから削除
				it = m_priorityQ.erase(it);
				//削除後のイテレータが「最後」の
				//ときはループを抜ける
				if (it == m_priorityQ.end()) {
					break;
				}
			}
			else {
				it++;
			}
		}
	}

	void EventDispatcher::Discharge(const shared_ptr<Event>& event) {
		auto shptr = event->m_receiver.lock();
		if (shptr) {
			//受け手が有効
			shptr->OnEvent(event);
		}
	}


	void EventDispatcher::ClearEventQ() {
		m_priorityQ.clear();
	}


	//--------------------------------------------------------------------------------------
	///	Objectインターフェイス
	//--------------------------------------------------------------------------------------
	void ObjectInterface::PostEvent(float dispatchTime, const shared_ptr<ObjectInterface>& sender, const shared_ptr<ObjectInterface>& receiver,
		const wstring& msgStr, const shared_ptr<void>& info) {
		App::GetEventDispatcher()->PostEvent(dispatchTime, sender, receiver, msgStr, info);
	}
	void ObjectInterface::PostEvent(float dispatchTime, const shared_ptr<ObjectInterface>& sender, const wstring& receiverKey,
		const wstring& msgStr, const shared_ptr<void>& info) {
		App::GetEventDispatcher()->PostEvent(dispatchTime, sender, receiverKey, msgStr, info);

	}

	void ObjectInterface::SendEvent(const shared_ptr<ObjectInterface>& sender, const shared_ptr<ObjectInterface>& receiver,
		const wstring& msgStr, const shared_ptr<void>& info) {
		App::GetEventDispatcher()->SendEvent(sender, receiver, msgStr, info);
	}

	void ObjectInterface::SendEvent(const shared_ptr<ObjectInterface>& sender, const wstring& receiverKey,
		const wstring& msgStr, const shared_ptr<void>& info) {
		App::GetEventDispatcher()->SendEvent(sender, receiverKey, msgStr, info);
	}





	namespace Dx12ShaderHelper {
		//--------------------------------------------------------------------------------------
		//　void CreateShaderFlomCso(
		//		const wstring& fileName,	//csoファイル
		//		ID3DBlob** pResult	//Blob
		//	);
		//	用途: CSOデータからシェーダ用のBlobを作成する
		//--------------------------------------------------------------------------------------
		void CreateShaderFlomCso(const wstring& fileName, ID3DBlob** pResult) {
			try {
				if (fileName == L"") {
					throw BaseException(
						L"ファイルが指定されていません\n",
						L"if(fileName == L\"\")\n",
						L"Dx12ShaderResource::CreateShaderFlomCso()"
					);
				}
				DWORD RetCode;
				RetCode = GetFileAttributes(fileName.c_str());
				if (RetCode == -1) {
					throw BaseException(
						L"ファイルが存在しません\n",
						fileName,
						L"\nDx12ShaderResource::CreateShaderFlomCso()"
					);
				}
				ThrowIfFailed(
					D3DReadFileToBlob(fileName.c_str(), pResult),
					L"Blpbの作成に失敗しました。\n",
					fileName,
					L"\nDx12ShaderResource::CreateShaderFlomCso()"
				);

			}
			catch (...) {
				throw;
			}
		}
	}


	//--------------------------------------------------------------------------------------
	//	class Dx12ShaderResource;
	//	用途: シェーダ関連リソースのインターフェイス
	//--------------------------------------------------------------------------------------
	Dx12ShaderResource::Dx12ShaderResource() {}
	Dx12ShaderResource::~Dx12ShaderResource() {}


	//シェーダアクセッサ
	ID3DBlob* Dx12ShaderResource::GetShaderBlob(const wstring& fileName, ComPtr<ID3DBlob>& shaderComPtr) {
		//ミューテックス
		std::mutex Mutex;
		//ラムダ式利用
		return Util::DemandCreate(shaderComPtr, Mutex, [&](ID3DBlob** pResult)
			{
				Dx12ShaderHelper::CreateShaderFlomCso(fileName, pResult);
			});
	}

	ComPtr<ID3DBlob>& Dx12ShaderResource::GetShaderBlobComPtr(const wstring& fileName, ComPtr<ID3DBlob>& shaderComPtr) {
		//ミューテックス
		std::mutex mutex;
		//ラムダ式利用
		Util::DemandCreate(shaderComPtr, mutex, [&](ID3DBlob** pResult)
			{
				Dx12ShaderHelper::CreateShaderFlomCso(fileName, pResult);
			});
		return shaderComPtr;
	}

}
// end namespace basecross
