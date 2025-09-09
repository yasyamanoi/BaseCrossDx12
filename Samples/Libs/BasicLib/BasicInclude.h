#pragma once


#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する

#include <windows.h>
#include "targetver.h"
//DirectX
#define USE_DXGI_1_6
#include <dxgi1_6.h>
#include <d3d12.h>
#include <dxgidebug.h>
#include <DirectXMath.h>
#include "d3dx12.h"
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
//assimp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

//BaseLib
//シンプルオブジェクト
#include "BaseMath.h"
using namespace basecross::bsm;

#include "BaseCommon.h"
#include "DxHelper.h"
#include "BaseHelper.h"
#include "ObjectInterface.h"
#include "VertexHelper.h"
#include "MeshHelper.h"
#include "TransHelper.h"
#include "InputDevice.h"
#include "BaseMesh.h"
#include "BaseTexture.h"
#include "App.h"
#include "PrimDevice.h"
#include "StepTimer.h"
#include "UILayer.h"
#include "BlendState.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "BaseDevice.h"
#include "FrameResource.h"
#include "BaseScene.h"

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

#if defined(_DEBUG)
#pragma comment(lib, "assimp-vc143-mtd.lib")
#pragma comment(lib, "zlibstaticd.lib")
#else
#pragma comment(lib, "assimp-vc143-mt.lib")
#pragma comment(lib, "zlibstatic.lib")
#endif


