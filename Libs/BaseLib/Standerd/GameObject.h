/*!
@file GameObject.h
@brief ゲームオブジェクトクラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"


namespace basecross {

	//--------------------------------------------------------------------------------------
	// 配置されるオブジェクトの親
	//--------------------------------------------------------------------------------------
	class GameObject : public ObjectInterface {
	protected:
		GameObject();
		virtual ~GameObject();
		std::map<std::wstring, std::shared_ptr<Component>> m_compMap;
	public:
		template<typename T>
		std::shared_ptr<T> AddComponent(
			ID3D12GraphicsCommandList* pCommandList,
			const std::wstring& wstr
		){
			auto it = m_compMap.find(wstr);
			if (it != m_compMap.end()) {
				return it->second;
			}
			else {
				std::shared_ptr<T> ptr = std::shared_ptr<T>(new T(GetThis<GameObject>()));
				m_compMap[wstr] = ptr;
				//初期化関数呼び出し
				ptr->OnPreCreate();
				ptr->OnCreate(pCommandList);
				ptr->SetCreated(true);
				return ptr;
			}
		}
	public:
		virtual void OnCreate(ID3D12GraphicsCommandList* pCommandList)override;
		virtual void OnUpdate(double elapsedTime)override;
	};


}
// end namespace basecross
