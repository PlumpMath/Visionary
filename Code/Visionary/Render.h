#pragma once

#include "stack.h"

typedef bool (*renderresourcecbf)( void* pObject, void* pInstance, void *pParent, double deltaTime, int nFrames );

struct SFrameNotify // for begin and end callbacks!
{
	SStack node;
	renderresourcecbf modulecbf;	
};

extern renderresourcecbf renderResourceCbf[VIT_VisionaryIdTypesMax];
extern SFrameNotify *beginFrameCbf;
extern SFrameNotify *endFrameCbf;

bool Render_Update( void * );

// Default Function Call (noop)
bool Object_Render( VisGeneric* pObject, VisInstance* pInstance, VisInstance *pParent, double deltaTime, int nFrames );
bool Instance_Render( VisGeneric* pObject, VisInstance* pInstance, VisInstance *pParent, double deltaTime, int nFrames );
bool CameraInstance_Render( VisGeneric* pObject, VisInstance* pInstance, VisInstance *pParent, double deltaTime, int nFrames );

bool VertexGroup_Patch( VisVertexGroup *pGroup, visint base );
bool Camera_Patch( VisCamera* pObject, visint base );
bool Light_Patch( VisLight* pObject, visint base );
bool Model_Patch( VisModel* pModel, visint base);
bool Instance_Patch( VisInstance* pObject, visint base );
bool Texture_Patch( VisTexture* pTexture, visint base);
bool Material_Patch( VisMaterial* pObject, visint base);