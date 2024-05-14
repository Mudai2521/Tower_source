#include "Win32Application.h"

HWND Win32Application::m_hwnd = nullptr;
bool Win32Application::m_fullscreenMode = false;
RECT Win32Application::m_windowRect;

using Microsoft::WRL::ComPtr;

int Win32Application::Run(D3D12Application* pApp, HINSTANCE hInstance, int nCmdShow
#if defined(DEBUG) || defined(_DEBUG)
    , ID3D12DebugDevice** debugDevice
#endif
)
{
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    pApp->ParseCommandLineArgs(argv, argc);
    LocalFree(argv);

    // ウィンドウクラス初期化
    WNDCLASSEX windowClass = { 0 };
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = L"DXApp";
    RegisterClassEx(&windowClass);

    RECT windowRect = { 0, 0, static_cast<LONG>(pApp->GetWidth()), static_cast<LONG>(pApp->GetHeight()) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    m_hwnd = CreateWindow(
        windowClass.lpszClassName,
        pApp->GetTitle(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,        
        nullptr,        
        hInstance,
        pApp);

   

    ShowWindow(m_hwnd, nCmdShow);

    pApp->OnInit(hInstance, m_hwnd);

#if defined(DEBUG) || defined(_DEBUG)
    pApp->GetDevice()->QueryInterface(debugDevice);
#endif

    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
    }

    pApp->OnDestroy();


    return static_cast<char>(msg.wParam);
}

void Win32Application::ToggleFullscreenWindow(IDXGISwapChain* pSwapChain)
{
    if (m_fullscreenMode)
    {
        SetWindowLong(m_hwnd, GWL_STYLE, m_windowStyle);

        SetWindowPos(
            m_hwnd,
            HWND_NOTOPMOST,
            m_windowRect.left,
            m_windowRect.top,
            m_windowRect.right - m_windowRect.left,
            m_windowRect.bottom - m_windowRect.top,
            SWP_FRAMECHANGED | SWP_NOACTIVATE);

        ShowWindow(m_hwnd, SW_NORMAL);
    }
    else
    {
        GetWindowRect(m_hwnd, &m_windowRect);

        SetWindowLong(m_hwnd, GWL_STYLE, m_windowStyle & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

        RECT fullscreenWindowRect;
        try
        {
            if (pSwapChain)
            {
                ComPtr<IDXGIOutput> pOutput;
                ThrowIfFailed(pSwapChain->GetContainingOutput(&pOutput));
                DXGI_OUTPUT_DESC Desc;
                ThrowIfFailed(pOutput->GetDesc(&Desc));
                fullscreenWindowRect = Desc.DesktopCoordinates;
            }
            else
            {
                throw HrException(S_FALSE);
            }
        }
        catch (HrException& e)
        {
            UNREFERENCED_PARAMETER(e);

            DEVMODE devMode = {};
            devMode.dmSize = sizeof(DEVMODE);
            EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode);

            fullscreenWindowRect = {
                devMode.dmPosition.x,
                devMode.dmPosition.y,
                devMode.dmPosition.x + static_cast<LONG>(devMode.dmPelsWidth),
                devMode.dmPosition.y + static_cast<LONG>(devMode.dmPelsHeight)
            };
        }

        SetWindowPos(
            m_hwnd,
            HWND_TOPMOST,
            fullscreenWindowRect.left,
            fullscreenWindowRect.top,
            fullscreenWindowRect.right,
            fullscreenWindowRect.bottom,
            SWP_FRAMECHANGED | SWP_NOACTIVATE);


        ShowWindow(m_hwnd, SW_MAXIMIZE);
    }

    m_fullscreenMode = !m_fullscreenMode;
}


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
            //pApp->OnKeyDown(static_cast<UINT8>(wParam));
        }
        return 0;

    case WM_KEYUP:
        if (pApp)
        {
            //pApp->OnKeyUp(static_cast<UINT8>(wParam));
        }
        return 0;

    case WM_PAINT:
        
        if (pApp)
        {
            pApp->OnUpdate();
            pApp->OnRender(hWnd);
        }
        
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
