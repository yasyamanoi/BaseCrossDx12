#pragma once


#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する

#include <windows.h>
#include "Basic/targetver.h"
//DirectX
#define USE_DXGI_1_6
#include <dxgi1_6.h>
#include <d3d12.h>
#include <dxgidebug.h>
#include <DirectXMath.h>
#include "Basic/d3dx12.h"
#include <d2d1_3.h>
#include <D3Dcompiler.h>
#include <dwrite.h>
#include <d3d11on12.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <DirectXTex.h>
#include <xinput.h>

//C
#include <process.h>
#include <wrl.h>
#include <shellapi.h>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//C++版C宣言
#include <cstdio>
#include <cfloat>
#include <clocale>
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
using namespace DirectX;

//PhysX
#include "PxPhysicsAPI.h"
//BaseLib
//シンプルオブジェクト
#include "Basic/BaseMath.h"
#include "Basic/BaseCommon.h"
#include "Basic/DxHelper.h"
#include "Basic/BaseHelper.h"
#include "Basic/ObjectInterface.h"
#include "Basic/VertexHelper.h"
#include "Basic/MeshHelper.h"
#include "Basic/TransHelper.h"
#include "Basic/InputDevice.h"
#include "Basic/BaseMesh.h"
#include "Basic/BaseTexture.h"
#include "Basic/App.h"
#include "Basic/Camera.h"
#include "Basic/Light.h"
#include "Basic/PrimDevice.h"
#include "Basic/StepTimer.h"
#include "Basic/UILayer.h"
#include "Basic/BlendState.h"
#include "Basic/RootSignature.h"
#include "Basic/PipelineState.h"
#include "Basic/BaseDevice.h"
#include "Basic/FrameResource.h"
#include "Basic/BaseScene.h"
#include "Standerd/Component.h"
#include "Standerd/GameObject.h"
#include "Standerd/Transform.h"
#include "Standerd/Gravity.h"
#include "Standerd/Collision.h"
#include "Standerd/CollisionManager.h"
#include "Standerd/Stage.h"

#include "Standerd/StateMachine.h"
#include "Standerd/Steering.h"
#include "Standerd/Behavior.h"
#include "Standerd/BehaviorSteering.h"
#include "Standerd/Shadowmap.h"
#include "Standerd/BcStaticDraw.h"



#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "d3d12.lib" )
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

//Dx11関連
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d2d1.lib" )
#pragma comment( lib, "dwrite.lib" )

#pragma comment(lib, "PhysX_64.lib")
#pragma comment(lib, "PhysXCommon_64.lib")
#pragma comment(lib, "PhysXCooking_64.lib")
#pragma comment(lib, "PhysXExtensions_static_64.lib")
#pragma comment(lib, "PhysXFoundation_64.lib")
#pragma comment(lib, "PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "PhysXTask_static_64.lib")
#pragma comment(lib, "SceneQuery_static_64.lib")
#pragma comment(lib, "SimulationController_static_64.lib")

