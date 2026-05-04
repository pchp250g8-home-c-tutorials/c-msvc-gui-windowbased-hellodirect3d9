// HelloDirect3D.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "HelloDirect3D.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
byte nRedColor;
HWND m_hWnd;
BOOL bActive;
D3DPRESENT_PARAMETERS d3dpp;
IDirect3D9* m_d3d9;
IDirect3DDevice9* m_d3d_Device;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
//------------------------------------------------------------------------------------------------------
BOOL InitializeDirect3D();
void DestroyDirect3D();
void RenderScene();
//------------------------------------------------------------------------------------------------------
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    MSG msg;
    BOOL bDone = FALSE;
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HELLODIRECT3D));
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_HELLODIRECT3D, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    // Main message loop:
    while (!bDone)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (GetMessage(&msg, NULL, 0, 0))
            {
                if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            else
            {
                bDone = TRUE;
            }
        }
        else
        {
            if (bActive)
                RenderScene();
        }
        bDone = (msg.message == WM_QUIT);
    }
    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HELLODIRECT3D));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_HELLODIRECT3D);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   m_hWnd = hWnd;
   if (!InitializeDirect3D())
   {
       MessageBox(NULL, _T("Error Loading DirectX"), _T("Error"), MB_OK | MB_ICONERROR);
       return FALSE;
   }
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_ACTIVATE:
        {
            bActive = LOWORD(wParam);
            break;
        }
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            RenderScene();
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        DestroyDirect3D();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


BOOL InitializeDirect3D()
{
    D3DDISPLAYMODE d3ddm;
    HRESULT hr;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    ZeroMemory(&d3ddm, sizeof(d3ddm));
    m_d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
    if (m_d3d9 == NULL) return FALSE;
    hr = m_d3d9->lpVtbl->GetAdapterDisplayMode(m_d3d9, 0, &d3ddm);
    if (FAILED(hr)) return FALSE;
    d3dpp.Windowed = TRUE;
    d3dpp.BackBufferFormat = d3ddm.Format;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    hr = m_d3d9->lpVtbl->CreateDevice
    (
        m_d3d9, 
        0, 
        D3DDEVTYPE_HAL, 
        m_hWnd, 
        D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
        &d3dpp, 
        &m_d3d_Device
    );
    if (FAILED(hr)) return FALSE;
    return TRUE;
}

void DestroyDirect3D()
{
    if (m_d3d_Device != NULL)
    {
        m_d3d_Device->lpVtbl->Release(m_d3d_Device);
        m_d3d_Device = NULL;
    }
        
    if (m_d3d9 != NULL)
    {
        m_d3d9->lpVtbl->Release(m_d3d9);
        m_d3d9 = NULL;
    }
        
}

void RenderScene()
{
    if (m_d3d9 == NULL) return;
    if (m_d3d_Device == NULL) return;
    m_d3d_Device->lpVtbl->Clear
    (
        m_d3d_Device,
        0, NULL, 
        D3DCLEAR_TARGET, 
        D3DCOLOR_XRGB(nRedColor++, 0, 0), 
        0, 0
    );
    m_d3d_Device->lpVtbl->BeginScene(m_d3d_Device);
    m_d3d_Device->lpVtbl->EndScene(m_d3d_Device);
    m_d3d_Device->lpVtbl->Present(m_d3d_Device, NULL, NULL, 0, NULL);
}