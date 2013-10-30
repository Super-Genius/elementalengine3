// TestEERender.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TestEERender.h"
#include <string>

#define MAX_LOADSTRING 100

#define IDT_TIMER1 0xBADDCAFE

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

// full screen mode.
static bool fullScreen = true;

HINSTANCE DX9DLL;
IRenderer *gRenderer;
IRenderContext *gRenderContext;
IBaseTextureObject *gTestTexture;

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

	RECT winRect;
	int windowWidth, windowHeight, bitsPerPixel;

	HDC hdc = GetDC(NULL);



	bitsPerPixel = GetDeviceCaps(hdc, BITSPIXEL);
	ReleaseDC(NULL, hdc);

	hInst = hInstance; // Store instance handle in our global variable

	if (!fullScreen)
	{
		windowWidth = 1280;
		windowHeight = 1024;

		hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	}
	else
	{
		::GetClientRect(::GetDesktopWindow(), &winRect);
		windowWidth = winRect.right - winRect.left;
		windowHeight = winRect.bottom - winRect.top;

		hWnd = CreateWindowEx(NULL,
                          szWindowClass, szTitle, WS_EX_TOPMOST | WS_POPUP, 
						  0, 0, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);
	}


	if (!hWnd)
	{
		return FALSE;
	}

	// make sure the "SetWindowLong" window style changes take effect
	SetWindowPos(hWnd, NULL, 0, 0, 0, 0, 
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	
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

	gRenderer = EEGetRendererInterface();

 	// this initializes the screen
	gRenderer->Initialize(hWnd, fullScreen, windowWidth, windowHeight, 24, bitsPerPixel);
	gRenderContext = gRenderer->CreateNewContext(hWnd, windowWidth, windowHeight, 24, bitsPerPixel);

	gTestTexture = EELoadTexture(_T("Textures\\Forest_grass.dds"));

	if (!EELoadSound(_T("Sounds\\TreehouseMusic.wav")))
	{
		//MessageBox(hWnd, _T("Unable to load wav file for playback, check log file"), _T("Error!"), MB_OK);
	}
	else if (!EEPlaySound(_T("Sounds\\TreehouseMusic.wav"), true, false))
	{
		//MessageBox(hWnd, _T("Unable to play ogg file for playback, check log file"), _T("Error!"), MB_OK);
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	// setup a time of 60 fps to process ogg vorbis sound queue/streaming sounds
	SetTimer(hWnd, IDT_TIMER1, 1000/60, (TIMERPROC) NULL);
	return TRUE;
}

void Update()
{

	gRenderer->SetBackgroundColor(255, 255, 255, 0);
	gRenderer->ClearScreen(true, true);
	gRenderer->RenderToContext(gRenderContext);
	gRenderer->BeginScene(true);
	gRenderer->Draw2DQuad(100.0f, 100.0f, 250.0f, 250.0f, NULL, 0xffff0000);
	// basic texture modulate
	gRenderer->SetMaterial(0, NULL);
	gRenderer->Draw2DQuad(350.0f, 350.0f, 250.0f, 250.0f, gTestTexture, 0xffffffff);
	gRenderer->EndScene();
	gRenderer->Present(gRenderContext);
	
	static DWORD iPreviousTickCount = GetTickCount();
	DWORD iCurrentTickCount = GetTickCount();
	DWORD iDeltaTickCount = iCurrentTickCount - iPreviousTickCount;
	iPreviousTickCount = iCurrentTickCount;

	GLOBALTICKPARAMS gtp;
	gtp.tickCount = iDeltaTickCount;
	static DWORD msgGlobalUpdateTick = CHashString( _T("GlobalUpdateTick") ).GetUniqueID();
	gToolBox->SendMessage(msgGlobalUpdateTick, sizeof(GLOBALTICKPARAMS), &gtp, NULL, NULL);


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
	case WM_DESTROY:
		KillTimer(hWnd, IDT_TIMER1);
		gRenderContext->DestroyContext();
		delete gRenderContext;
		PostQuitMessage(0);
		break;
	//case WM_PAINT:
	case WM_TIMER:
		switch (wParam) 
		{ 
			case IDT_TIMER1:
			{
				Update();
			}
			break; 
		}
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
