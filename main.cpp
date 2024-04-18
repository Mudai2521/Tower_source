
#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"
#include "Win32Application.h"
#include "D3D12Application.h"
#include "DX12Game.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
#if defined(DEBUG) || defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    
#if defined(DEBUG) || defined(_DEBUG)
    ComPtr<ID3D12DebugDevice> debugDevice;
#endif
    {
        DX12Game App(1280, 720, L"DX12_Application");
        Win32Application::Run(&App, hInstance, nCmdShow
#if defined(DEBUG) || defined(_DEBUG)
            , &debugDevice
#endif 
        );
    }
#if defined(DEBUG) || defined(_DEBUG)
    debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
#endif
    return 0;
}


