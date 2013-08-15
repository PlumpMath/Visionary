
#include "stdafx.h"
#include "Mouse.h"
#include "TypeDefinitions.h"

SMouseInfo g_MouseData[2];
static short g_nFrame[2] = { 0, 1 };

bool Mouse_Initialize( SGlobalData*, void* )
{
	SMouseInfo *pBeg = &g_MouseData[0],
			   *pEnd = pBeg + ( sizeof(g_MouseData)/sizeof(*g_MouseData) );

	for(;pBeg<pEnd;pBeg++)
	{
		pBeg->header.data.size = sizeof(*pBeg);
		pBeg->header.data.id.number = 0x1;
		pBeg->header.data.id.type = VIT_MOUSE;
		pBeg->header.id = pBeg->header.data.id;
	}

	return true;
}

SMouseInfo* Mouse_GetMouseData( void )
{
	return &g_MouseData[g_nFrame[0]];
}

bool Mouse_EndFrame( void* pObject, void* pInstance, void *pParent, double deltaTime, int nFrames )
{
	// Switch current frame and next frame pointers
	// clear all entries

	int nTmp = g_nFrame[0];
	g_nFrame[0] = g_nFrame[1];
	g_nFrame[1] = nTmp;

	SMouseInfo &prevMouse = g_MouseData[g_nFrame[1]],
			   &currMouse = g_MouseData[g_nFrame[0]];

	// If the mouse has a valid position update using it otherwise
	// use the previous position
	currMouse.prevPosition = prevMouse.position;

	// Copy the current so buttons state changes
	// will be reflected correctly
	prevMouse = currMouse;

	// Set values to Zero that shouldn't be carried
	// across frames
	prevMouse.zDelta = 0;
	prevMouse.button = 0;
	prevMouse.click = 0;

	/*if( currMouse.position.x != currMouse.prevPosition.x
	||	currMouse.position.y != currMouse.prevPosition.y )
	{
		NDBGPRINT((msg,"INFO: Mouse curr (%i, %i)\n\t  Mouse prev (%i,%i)\n",currMouse.position.x,currMouse.position.y,currMouse.prevPosition.x, currMouse.prevPosition.y));
	}*/

	return true;
}

void Mouse_Process( UINT message, WPARAM wParam, LPARAM lParam )
{
	enum { MOVE, LBUTTON, RBUTTON, MBUTTON };

	SMouseInfo &mouse = g_MouseData[g_nFrame[1]];

	unsigned int fwButton = (message - WM_LBUTTONDOWN) / 3 + 1, // L=1,R=2,M=3, None = 0
		fwClick = (message - WM_LBUTTONDOWN) % 3,
		fwKeys = wParam, zDelta = 0;

	switch( message )
	{
		default:
			
			break;
		//case WM_XBUTTONDOWN:
		//case WM_XBUTTONUP:
		//case WM_XBUTTONDBLCLK:
		//	fwButton = MBUTTON+GET_XBUTTON_WPARAM (wParam);
		
		case WM_MOUSEWHEEL:
			fwClick = 0;
			mouse.zDelta = ((int)wParam>>16) / WHEEL_DELTA;
			// mouse.zDelta = GET_WHEEL_DELTA_WPARAM(wParam)/ WHEEL_DELTA;
			break;

		case WM_MOUSEMOVE:
			fwClick = 0;
			fwButton = 0;
		 	break;
	}
	
	mouse.button = fwButton;
	mouse.click = fwClick;
	mouse.ctrl = (wParam & MK_CONTROL) == MK_CONTROL;
	mouse.shift= (wParam & MK_SHIFT) == MK_SHIFT;
	mouse.lbtn = (wParam & MK_LBUTTON) == MK_LBUTTON;
	mouse.rbtn = (wParam & MK_RBUTTON) == MK_RBUTTON;
	mouse.mbtn = (wParam & MK_MBUTTON) == MK_MBUTTON;
	// mouse.xbtn1= (wParam & MK_XBUTTON1)== MK_XBUTTON1;
	// mouse.xbtn2= (wParam & MK_XBUTTON2)== MK_XBUTTON2;
	mouse.position = MAKEPOINTS(lParam);

	// NDBGPRINT((msg,"Mouse %s (zd %i) (l %d) (r %d)\n", (fwClick==1)?"Up":(fwClick==2)?"Down":(fwClick)?"Dbl":"Move", mouse.zDelta, mouse.lbtn, mouse.rbtn));
}
