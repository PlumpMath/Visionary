
#include "stdafx.h"
#include "Keys.h"
#include "TypeDefinitions.h"

SKeyInfo g_KeyData[2];
static short g_bReadKey[2] = { 0, 0 };
static short g_nFrame[2] = { 0, 1 };

bool Keys_Initialize( SGlobalData*, void* )
{
	SKeyInfo *pBeg = &g_KeyData[0],
			 *pEnd = pBeg + ( sizeof(g_KeyData)/sizeof(*g_KeyData) );

	for(;pBeg<pEnd;pBeg++)
	{
		pBeg->header.data.size = sizeof(*pBeg);
		pBeg->header.data.id.number = 0x1;
		pBeg->header.data.id.type = VIT_KEYS;
		pBeg->header.id = pBeg->header.data.id;
	}

	return true;
}

SKeyInfo* Keys_GetKeyData( void )
{
	return &g_KeyData[g_nFrame[0]];
}

bool Keys_HasChanged( void )
{
	return g_bReadKey[g_nFrame[0]] > 0;
}

bool Keys_EndFrame( void* pObject, void* pInstance, void *pParent, double deltaTime, int nFrames )
{
	// Switch current frame and next frame pointers
	// clear all entries

	int nTmp = g_nFrame[0];
	g_nFrame[0] = g_nFrame[1];
	g_nFrame[1] = nTmp;	
	
	// Clear the data for the next frame
	ZeroMemory( &g_KeyData[g_nFrame[1]], sizeof(g_KeyData[g_nFrame[1]]));
	g_bReadKey[g_nFrame[1]] = 0;

	return true;
}

void Keys_Process( WPARAM wParam, LPARAM lParam )
{
	g_bReadKey[g_nFrame[1]] += 1;

	// Get proper key entry
	SKeyData &key = g_KeyData[g_nFrame[1]].keys[wParam][(lParam >> 0x18) & 1];

	unsigned short keystate = GetKeyState( wParam );

	key.value = wParam;
	key.toggle = (keystate & 0xff) >> 6 | LOBYTE(keystate); // sets a toggle state
	key.state.transition = (lParam >> 0x1f) & 1;
	key.state.previous   = (lParam >> 0x1e) & 1;
	key.state.context	 = (lParam >> 0x1d) & 1;
	key.state.reserved	 = (lParam >> 0x19) & 0xf;
	key.state.extended	 = (lParam >> 0x18) & 1;
	key.state.scancode	 = (lParam >> 0x10) & 0xff;
	key.state.repeat	 = lParam & 0xffff;

	/*DBGPRINT((msg, "Key %s: (\"%c\", %x) <scn x%X> <xtnd x%X> <tran %d>\n",
		(key.state.transition)?"Up":"Down",key.value, key.value,
		key.state.scancode,key.state.extended,key.state.transition));*/
}
