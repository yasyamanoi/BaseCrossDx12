/*!
@file BaseCommon.h
@brief ライブラリインクルード
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/


#pragma once

#include "Basic/targetver.h"

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する

#include <windows.h>
#include <wrl.h>
#include <initguid.h>
#include <mmsystem.h>
#include <atlbase.h>
#include <commctrl.h> // for InitCommonControls() 
#include <shellapi.h> // for ExtractIcon()
#include <new.h>
#include <shlobj.h>
#include <Winhttp.h>
//Dx12
#include <d2d1_3.h>
#include <dwrite.h>
#include <d3d11on12.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include "Basic/d3dx12.h"
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <DirectXTex.h>
#include <xinput.h>

#include <xaudio2.h>	//サウンド
#include <xaudio2fx.h>
#include <mmreg.h>
#include <mfidl.h>
#include <mfapi.h>
#include <mfreadwrite.h>
#include <msxml6.h>

//C
#include <cassert>
#include <cwchar>
#include <cmath>
#include <climits>
#include <ctime>
// STL
#include <vector>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <functional>
#include <algorithm>
#include <iostream>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <string>
#include <new>
#include <memory>
#include <exception>
#include <stdexcept>
#include <type_traits>
#include <typeindex>
#include <codecvt>

#pragma comment( lib, "d3d12.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "d3dcompiler.lib" )
#pragma comment( lib, "DirectXTex.lib" )
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "comctl32.lib" )
// XInput
#pragma comment( lib, "XInput.lib" )
//オーディオ
#pragma comment( lib, "xaudio2.lib" )
#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
//WinHTTP
#pragma comment( lib, "Winhttp.lib" )


using namespace std;
using namespace DirectX;
using Microsoft::WRL::ComPtr;


#include "Basic/BaseMath.h"
using namespace basecross::bsm;
#include "Basic/Event.h"
#include "Basic/BaseHelper.h"
#include "Basic/VertexHelper.h"
#include "Basic/MeshHelper.h"
#include "Basic/physics_effects/include/physics_effects.h"
#include "Basic/TransHelper.h"
#include "Basic/InputDevice.h"
#include "Basic/BaseBlendState.h"
#include "Basic/BasicPipelineState.h"
#include "Basic/BaseTimer.h"
#include "Basic/BaseDevice.h"
#include "Basic/BaseFrame.h"
#include "Basic/BaseTexture.h"
#include "Basic/BaseMesh.h"
#include "Basic/BasePhysics.h"
#include "Basic/BaseScene.h"
#include "Basic/App.h"
#include "Standerd/Objects/StateMachine.h"
#include "Standerd/Objects/Behavior.h"
#include "Standerd/Objects/Steering.h"
#include "Standerd/Objects/SteeringBehavior.h"
#include "Standerd/Objects/GameObject.h"
#include "Standerd/Objects/Camera.h"
#include "Standerd/Objects/Light.h"
#include "Standerd/Objects/Stage.h"
#include "Standerd/Components/Component.h"
#include "Standerd/Components/Transform.h"
#include "Standerd/Components/Gravity.h"
#include "Standerd/Components/Action.h"
#include "Standerd/Components/Collision.h"
#include "Standerd/Objects/CollisionManager.h"
#include "Standerd/Components/Rigidbody.h"
#include "Standerd/Components/ShadowmapDraw.h"
#include "Standerd/Components/SpStaticDraw.h"
#include "Standerd/Components/BcStaticDraw.h"


