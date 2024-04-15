

#pragma once

#include "stdafx.h"
#include "D3D12Application.h"


class D3D12Application;

class Win32Application
{
public:
    static int Run(D3D12Application* pApp, HINSTANCE hInstance, int nCmdShow);
    static HWND GetHwnd() { return m_hwnd; }

protected:
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    static HWND m_hwnd;
};
