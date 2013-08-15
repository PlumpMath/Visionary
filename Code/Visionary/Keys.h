
#ifndef VISIONARY_KEYS_H
#define VISIONARY_KEYS_H

bool Keys_Initialize( SGlobalData*, void* );
bool Keys_EndFrame( void* pObject, void* pInstance, void *pParent, double deltaTime, int nFrames );
void Keys_Process( WPARAM wParam, LPARAM lParam );


SKeyInfo* Keys_GetKeyData( void );
bool Keys_HasChanged( void );

#endif