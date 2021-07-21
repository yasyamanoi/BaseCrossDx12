/*!
@file WinMain.cpp
@brief アプリケーションエントリポイント
@copyright Copyright (c) 2021 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#include "stdafx.h"
#include "Project.h"

//--------------------------------------------------------------------------------------
// アプリケーションエントリポイント
//--------------------------------------------------------------------------------------
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	basecross::DefaultDevice device(1280, 720, L"BaseCrossDx12Title");
	basecross::Scene scene;
	return basecross::App::Run(&device,&scene,hInstance, nCmdShow);
}
