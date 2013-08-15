// Visionary.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

// #include <Perl.h>								// for perl embedding
// #include <Extern.h>								// for perl embedding
#include "PythonModule.h"

#include "TypeDefinitions.h"
#include "BuildObject.h"
#include "GameObject.h"
#include "LibLoader.h"
#include "Visionary.h"
#include "Render.h"
#include "Resources.h"
#include "Keys.h"
#include "Mouse.h"

#include <stdio.h>

#define MAX_LOADSTRING 100

struct SGlobalData g_GlobalData;
CGameObject *gp_GameObject;

// Global Variables:
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
BOOL				InitInstance(HINSTANCE, int);
BOOL				FineInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{

	bool embedPerl = false,
		 embedPython = true;
	
//	PerlInterpreter *_perl = NULL;  /***    The Perl interpreter    ***/

	MSG msg;
	HACCEL hAccelTable;

	memset( &g_GlobalData, 0, sizeof(g_GlobalData) );

	g_GlobalData.hInstance = hInstance;
	g_GlobalData.hPrevInstance = hPrevInstance;
	g_GlobalData.lpCmdLine = lpCmdLine;
	g_GlobalData.nCmdShow = nCmdShow;
	g_GlobalData.WndProc = WndProc;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_VISIONARY, szWindowClass, MAX_LOADSTRING);

	if( embedPerl )
	{
/*		int argc = 1;
		char** argv = &lpCmdLine;
		PERL_SYS_INIT3(&argc,&argv,&env);
		_perl = perl_alloc();
		perl_construct(_perl);*/
	}
#ifndef _DEBUG
	if( embedPython )
	{
		PythonModule_Initialize( "Visionary.exe" );
	}
#endif

	/*
		char *args[] = { NULL };
        call_argv("showtime", G_DISCARD | G_NOARGS, args);
	*/

	// Perform application initialization:
	if (InitInstance (hInstance, nCmdShow)) 
	{
		hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_VISIONARY);

		// Main message loop:
		while (GetMessage(&msg, NULL, 0, 0)) 
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	FineInstance(hInstance, nCmdShow);

#ifndef _DEBUG
	if( embedPython )
	{
		PythonModule_Finalize();
	}
#endif

	if( embedPerl )
	{
/*		if( _perl )
		{
			perl_destruct(_perl);
			perl_free(_perl);
		}
        PERL_SYS_TERM();*/
	}

	return (int) msg.wParam;
}

//
//   FUNCTION: InitInstance(HANDLE, int)
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
	// Allocate main Game Object
	gp_GameObject = new CGameObject( g_GlobalData );
	if( gp_GameObject )
	{
		if( LibLoader_Initialize() )
		{
			if( Resource_Initialize() )
			{
				ShowWindow(g_GlobalData.hWnd, nCmdShow);
				// UpdateWindow(hWnd);
				
				return true;
			}
		}

		delete gp_GameObject;
		gp_GameObject = NULL;
	}

	return false;
}

BOOL FineInstance(HINSTANCE hInstance, int nCmdShow)
{
	if( Resource_Finalize() )
	{
		if( LibLoader_Finalize() )
		{
			delete gp_GameObject;
			return true;
		}
	}

	return false;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
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
	PAINTSTRUCT ps;
	HDC hdc;

	HINSTANCE hInst = g_GlobalData.hInstance;
	
	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		if( g_GlobalData.hDC == hdc )
		{
			// Update game logic
			Render_Update( (void*)&g_GlobalData );		// Draw The Scene
			SwapBuffers(hdc);		// Swap Buffers (Double Buffering)	
		}
		
		EndPaint(hWnd, &ps);

		// make windows update the window as fast as possible!
		InvalidateRect(hWnd,NULL,FALSE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN: 
	case WM_KEYUP:
		Keys_Process( wParam, lParam );
		break;
	case WM_MOUSEWHEEL: case WM_MOUSEMOVE: case WM_LBUTTONDOWN:
	case WM_LBUTTONUP: case WM_LBUTTONDBLCLK: case WM_RBUTTONDOWN:
	case WM_RBUTTONUP: case WM_RBUTTONDBLCLK: case WM_MBUTTONDOWN:
	case WM_MBUTTONUP: case WM_MBUTTONDBLCLK:
		Mouse_Process( message, wParam, lParam );		
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
