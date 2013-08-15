#pragma once

#ifndef RENDERER_RENDERER_H
#define RENDERER_RENDERER_H

#include "TypeDefinitions.h"

bool Render_Initialize( SGlobalData *pGlobalData, void* );
bool Render_Finalize( SGlobalData *pGlobalData, void* );

bool Render_BeginFrame( VisLight*, VisInstance *, VisInstance *, double, int );

bool Object_Render( VisGeneric* pObject, VisInstance *pInstance,  VisInstance *pParent, double deltaTime, int nFrames );
bool Light_Render( VisLight* pObject, VisInstance *pInstance, VisInstance *pParent, double deltaTime, int nFrames );
bool Camera_Render( VisCamera* pObject, VisInstance *pInstance, VisInstance *pParent, double deltaTime, int nFrames );
bool Model_Render( VisModel* pObject, VisInstance *pInstance, VisInstance *pParent, double deltaTime, int nFrames );

#endif // ndef RENDERER_RENDERER_H