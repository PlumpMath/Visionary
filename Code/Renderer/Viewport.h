#pragma once

#include "TypeDefinitions.h"

bool Viewport_Initialize( SGlobalData *pGlobalData, void* /*pViewport*/);
bool Viewport_Resize( visint left, visint top, visint width, visint height );
bool Viewport_Finalize( SGlobalData *pGlobalData, void* );