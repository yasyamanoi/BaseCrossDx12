/*!
@file WinMain.cpp
@brief �A�v���P�[�V�����G���g���|�C���g
*/

#include "stdafx.h"
#include "Project.h"


//--------------------------------------------------------------------------------------
// �A�v���P�[�V�����G���g���|�C���g
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{

	basecross::BaseDevice device(1280, 800, L"BaseCrossDx12Title");
	basecross::Scene scene;
	return basecross::App::Run(&device, &scene, hInstance, nCmdShow);

}