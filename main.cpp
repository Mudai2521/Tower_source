
#pragma once

#include "stdafx.h"
#include "Win32Application.h"
#include "D3D12Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    D3D12Application App(1280, 720, L"DX12 test");
    return Win32Application::Run(&App, hInstance, nCmdShow);
}


