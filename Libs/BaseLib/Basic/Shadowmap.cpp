/*!
@file Shadowmap.cpp
@brief シャドウマップ定数など（実体）
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/


#include "stdafx.h"

namespace basecross {


	float Shadowmap::m_lightHeight(200.0f);
	float Shadowmap::m_lightNear(1.0f);
	float Shadowmap::m_lightFar(220.0f);
	float Shadowmap::m_viewWidth(16.0f);
	float Shadowmap::m_viewHeight(16.0f);
	float Shadowmap::m_posAdjustment(0.1f);

}
// end namespace basecross
