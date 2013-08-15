
#include "stdafx.h"
#include "viewport.h"

static bool	  fullscreen;

bool Viewport_Initialize( SGlobalData *pGlobalData, void* /*pViewport*/) 
{
	//////////////// Remove during clean up ///////////////////
	char* title = "Visionary";
	int bits = 16;
	bool fullscreenflag = false;
	//////////////// Remove during clean up ///////////////////

	// hard code for now!

	static visint left = 0, top =0,
				  width = 640, height = 480;

	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	pGlobalData->hInstance	= GetModuleHandle(NULL);			// Grab An Instance For Our Window
	wc.style			= CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) pGlobalData->WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= pGlobalData->hInstance;				// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= title;								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(pGlobalData->hWnd=CreateWindowEx(	dwExStyle,				// Extended Style For The Window
								title,								// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								pGlobalData->hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		Viewport_Finalize(pGlobalData, NULL);								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(pGlobalData->hDC=GetDC(pGlobalData->hWnd)))	// Did We Get A Device Context?
	{
		Viewport_Finalize(pGlobalData, NULL);								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(pGlobalData->hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		Viewport_Finalize(pGlobalData, NULL);								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(pGlobalData->hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		Viewport_Finalize(pGlobalData, NULL);								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(pGlobalData->hRC=wglCreateContext(pGlobalData->hDC)))				// Are We Able To Get A Rendering Context?
	{
		Viewport_Finalize(pGlobalData, NULL);								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(pGlobalData->hDC,pGlobalData->hRC))					// Try To Activate The Rendering Context
	{
		Viewport_Finalize(pGlobalData, NULL);								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(pGlobalData->hWnd,SW_SHOW);			// Show The Window
	SetForegroundWindow(pGlobalData->hWnd);			// Slightly Higher Priority
	SetFocus(pGlobalData->hWnd);					// Sets Keyboard Focus To The Window
	Viewport_Resize(left, top, width, height);		// Set Up Our Perspective GL Screen

	return true;	
}

bool Viewport_Resize( visint left, visint top, visint width, visint height )
{
	glViewport(left, top, width, height);

/*	// Creates camera model
	glMatrixMode(GL_PROJECTION);				// Select The Projection Matrix
	glLoadIdentity();							// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	visfloat fAspect = width/height,
			 fFov = 60.0f,
			 fNear = 1.0f,
			 fFar = 400.0f;

	gluPerspective(fFov,fAspect,fNear,fFar);

	glMatrixMode(GL_MODELVIEW);					// Select The Modelview Matrix
	glLoadIdentity();							// Reset The Modelview Matrix
*/
	return true;
}

bool Viewport_Finalize( SGlobalData *pGlobalData, void* /*pViewport*/ )
{
	if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
	{
		OutputDebugString("Release Of DC And RC Failed.");
	}

	if (pGlobalData->hRC
	&& !wglDeleteContext(pGlobalData->hRC))		// Are We Able To Delete The RC?
	{
		OutputDebugString("Release Rendering Context Failed.");
		pGlobalData->hRC=NULL;							// Set RC To NULL
	}

	if (pGlobalData->hDC 
	&& !ReleaseDC(pGlobalData->hWnd,pGlobalData->hDC))	// Are We Able To Release The DC
	{
		OutputDebugString("Release Device Context Failed.");
		pGlobalData->hDC=NULL;							// Set DC To NULL
	}

	if (pGlobalData->hWnd 
	&& !DestroyWindow(pGlobalData->hWnd))				// Are We Able To Destroy The Window?
	{
 		OutputDebugString("Could Not Release hWnd.");
		pGlobalData->hWnd=NULL;			 				// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",pGlobalData->hInstance))	// Are We Able To Unregister Class
	{
		OutputDebugString("Could Not Unregister Class.");
		pGlobalData->hInstance=NULL;						// Set hInstance To NULL
	}

	return true;
}