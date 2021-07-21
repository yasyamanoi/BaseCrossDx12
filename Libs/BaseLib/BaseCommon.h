#pragma once
#include "targetver.h"
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
#include <pix3.h>

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
#include "Basic/BaseHelper.h"
#include "Basic/VertexHelper.h"
#include "Basic/MeshHelper.h"
#include "Basic/Camera.h"
#include "Basic/Light.h"
#include "Basic/StepTimer.h"
#include "Basic/BaseDevice.h"
#include "Basic/DefaultDevice.h"
#include "Basic/FrameResource.h"
#include "Basic/BaseTexture.h"
#include "Basic/BaseMesh.h"
#include "Standerd/Component.h"
#include "Standerd/Transform.h"
#include "Standerd/Velocity.h"
#include "Standerd/RenderComponents.h"
#include "Standerd/GameObject.h"
#include "Basic/BaseStage.h"
#include "Basic/BaseScene.h"
#include "Basic/App.h"



