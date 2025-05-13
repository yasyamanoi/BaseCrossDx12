/*!
@file Transform.h
@brief 座標変換コンポーネント
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#pragma once
#include "stdafx.h"


namespace basecross {

	using namespace std;
	using namespace bsm;

	//--------------------------------------------------------------------------------------
	///	座標変換コンポーネント
	//--------------------------------------------------------------------------------------
	class Transform : public Component {
	protected:
		TransParam m_param;
	public:
		Transform(const shared_ptr<GameObject>& gameObjectPtr,const TransParam& param);
		virtual ~Transform(){}
		TransParam& GetTransParam(){
			return m_param;
		}
		void SetTransParam(const TransParam& param) {
			m_param = param;
		}
	};


}
// end namespace basecross
