#pragma once

#include "TypeDefinitions.h"

typedef bool (*resourcecbf)( SGlobalData*, void* );
typedef bool (*patchresourcecbf)( void*, visint base );
typedef bool (*patchsharedresourcecbf)(void*, visint base);

bool Resource_Initialize( void );
bool Resource_Finalize( void );

bool Resource_Patch( void *, visint base );
bool Resource_Load( void * buffer, visint lBuffer );
bool Resource_Load( char * szFilename );

bool Resource_PatchOffset( visint& offset, visint base );
bool Resource_Resolve( VisGenericPtr *pObject );

// Search algorithim patch files
bool Linear_Patch( SSearchObject *pObject, visint base );
bool Binary_Patch( SSearchObject *pObject, visint base );

extern patchresourcecbf patchResourceCbf[VIT_VisionaryIdTypesMax];
extern patchresourcecbf patchSharedResourceCbf[2];
