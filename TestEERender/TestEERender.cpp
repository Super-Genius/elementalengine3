// TestEERender.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TestEERender.h"
#include <string>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

IToolBox			*gToolBox;
DLLPRIORITYMAP		gDLLPMap;			// the plugin DLL loaded by priority

static CHashString getRI(_T("GetRendererInterface"));
HINSTANCE DX9DLL;
IRenderer *gRenderer;
IRenderContext *gRenderContext;
ITextureObject *gTestTexture;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TESTEERENDER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TESTEERENDER));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTEERENDER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TESTEERENDER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


// Create a string with last error message
std::string GetLastErrorStdStr()
{
  DWORD error = GetLastError();
  if (error)
  {
    LPVOID lpMsgBuf;
    DWORD bufLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );
    if (bufLen)
    {
      LPCSTR lpMsgStr = (LPCSTR)lpMsgBuf;
      std::string result(lpMsgStr, lpMsgStr+bufLen);
      
      LocalFree(lpMsgBuf);

      return result;
    }
  }
  return std::string();
}

/// Load a texture from a file
/// param: filename - const TCHAR pointer to filename
/// returns: ITextureObject
ITextureObject *LoadTexture(const TCHAR *filename)
{
	if (!filename || !filename[0])
		return NULL;

	CHashString szName(filename);
	TEXTUREOBJECTPARAMS tobj;
	tobj.bLoad = true;
	tobj.Name = &szName;
	tobj.TextureObjectInterface = NULL;
	// AddTexture message loads and adds texture to texture Manager...
	static DWORD msgHash_AddTexture = CHashString(_T("AddTexture")).GetUniqueID();
	if( EngineGetToolBox()->SendMessage(msgHash_AddTexture, sizeof(tobj), &tobj ) == MSG_HANDLED)
	{
		return (ITextureObject *)tobj.TextureObjectInterface;
	}

	return NULL;
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
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}
	
	gToolBox = EngineGetToolBox();
	if (gToolBox->LoadPlugins(_T(".\\Plugins\\*.dlo"), gDLLPMap) != ERROR_SUCCESS)
	{
		MessageBox(hWnd, _T("Unable to load plugings, check log file"), _T("Error!"), MB_OK);
		return FALSE;
	}
	
	if (gToolBox->InitPlugins(gDLLPMap) != ERROR_SUCCESS)
	{
		MessageBox(hWnd, _T("Unable to initialize plugings, check log file"), _T("Error!"), MB_OK);
		return FALSE;
	}

	gToolBox->SendMessage(getRI.GetUniqueID(), sizeof(IRenderer *), &gRenderer);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	RECT winRect;
	int windowWidth, windowHeight, bitsPerPixel;

	::GetClientRect(hWnd, &winRect);
	windowWidth = winRect.right - winRect.left;
	windowHeight = winRect.bottom - winRect.top;
	HDC hdc = GetDC(NULL);
	bitsPerPixel = GetDeviceCaps(hdc, BITSPIXEL);
	ReleaseDC(NULL, hdc);

	// this initializes the screen
	gRenderer->Initialize(hWnd, false, windowWidth, windowHeight, 24, bitsPerPixel);
	gRenderContext = gRenderer->CreateNewContext(hWnd, windowWidth, windowHeight, 24, bitsPerPixel);

	gTestTexture = LoadTexture(_T("Textures\\Forest_grass.dds"));

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
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
		break;
	case WM_PAINT:
		gRenderer->SetBackgroundColor(255, 255, 255, 0);
		gRenderer->ClearScreen(true, true);
		gRenderer->RenderToContext(gRenderContext);
		gRenderer->BeginScene(true);
		gRenderer->Draw2DQuad(100.0f, 100.0f, 250.0f, 250.0f, NULL, 0xff0000ff);
		// basic texture modulate
		gRenderer->SetMaterial(0, NULL);
		gRenderer->Draw2DQuad(350.0f, 350.0f, 250.0f, 250.0f, gTestTexture, 0xffffffff);
		gRenderer->EndScene();
		gRenderer->Present(gRenderContext);
		break;
	case WM_DESTROY:
		gRenderContext->DestroyContext();
		delete gRenderContext;
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
