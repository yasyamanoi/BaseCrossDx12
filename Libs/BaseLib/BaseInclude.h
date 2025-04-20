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
//C++版C宣言
#include <cfloat>
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


//BaseLib
//基本レベルオブジェクト
#include "Basic/BaseMath.h"
#include "Basic/DxHelper.h"
#include "Basic/BaseHelper.h"
#include "Basic/ObjectInterface.h"
#include "Basic/BaseCommon.h"
#include "Basic/VertexHelper.h"
#include "Basic/MeshHelper.h"
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
#include "Basic/Shadowmap.h"
#include "Basic/BaseScene.h"

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



