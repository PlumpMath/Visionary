
#ifndef VISIONARY_MOUSE_H
#define VISIONARY_MOUSE_H

bool Mouse_Initialize( SGlobalData*, void* );
bool Mouse_EndFrame( void* pObject, void* pInstance, void *pParent, double deltaTime, int nFrames );
void Mouse_Process( UINT message, WPARAM wParam, LPARAM lParam );

SMouseInfo* Mouse_GetMouseData( void );

#endif