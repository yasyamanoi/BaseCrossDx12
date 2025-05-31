/*!
@file WinMain.cpp
@brief エントリポイント
*/


#include "stdafx.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	// Declare this process to be high DPI aware, and prevent automatic scaling 
	HINSTANCE hUser32 = LoadLibrary(L"user32.dll");
	if (hUser32)
	{
		typedef BOOL(WINAPI * LPSetProcessDPIAware)(void);
		LPSetProcessDPIAware pSetProcessDPIAware = (LPSetProcessDPIAware)GetProcAddress(hUser32, "SetProcessDPIAware");
		if (pSetProcessDPIAware)
		{
			pSetProcessDPIAware();
		}
		FreeLibrary(hUser32);
	}

	basecross::BaseDevice device(1280, 800, L"BaseCrossDx12");
	return basecross::App::Run(&device, hInstance, nCmdShow);
}
