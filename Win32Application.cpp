

#include "stdafx.h"
#include "Win32Application.h"

HWND Win32Application::m_hwnd = nullptr;

int Win32Application::Run(D3D12Application* pApp, HINSTANCE hInstance, int nCmdShow)
{
    // Parse the command line parameters
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    pApp->ParseCommandLineArgs(argv, argc);
    LocalFree(argv);

    // Initialize the window class.
    WNDCLASSEX windowClass = { 0 };
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = L"DXSampleClass";
    RegisterClassEx(&windowClass);

    RECT windowRect = { 0, 0, static_cast<LONG>(pApp->GetWidth()), static_cast<LONG>(pApp->GetHeight()) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    // Create the window and store a handle to it.
    m_hwnd = CreateWindow(
        windowClass.lpszClassName,
        pApp->GetTitle(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,        // We have no parent window.
        nullptr,        // We aren't using menus.
        hInstance,
        pApp);

    pApp->OnInit();

    

    ShowWindow(m_hwnd, nCmdShow);

    // Main loop.
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        // Process any messages in the queue.
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        else 
        {
            pApp->OnUpdate();
            pApp->OnRender();
            
        }
        
    }

    pApp->OnDestroy();

    // Return this part of the WM_QUIT message to Windows.
    return static_cast<char>(msg.wParam);
}

// Main message handler.
LRESULT CALLBACK Win32Application::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    D3D12Application* pApp = reinterpret_cast<D3D12Application*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_CREATE:
    {
        LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
    }
    return 0;

    case WM_KEYDOWN:
        if (pApp)
        {
            pApp->OnKeyDown(static_cast<UINT8>(wParam));
        }
        return 0;

    case WM_KEYUP:
        if (pApp)
        {
            pApp->OnKeyUp(static_cast<UINT8>(wParam));
        }
        return 0;

    case WM_PAINT:
        
        if (pApp)
        {
            pApp->OnUpdate();
            pApp->OnRender();
        }
        
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
