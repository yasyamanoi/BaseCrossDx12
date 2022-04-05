#include "stdafx.h"
#include "Project.h"


//--------------------------------------------------------------------------------------
// アプリケーションエントリポイント
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{

	basecross::BaseDevice device(1280, 720, L"BaseCrossDx12Title");
	basecross::Scene scene;
	return basecross::App::Run(&device, &scene, hInstance, nCmdShow);

}
