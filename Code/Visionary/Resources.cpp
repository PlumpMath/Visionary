
#include "stdafx.h"
#include "Resources.h"
#include "Render.h"
#include "TypeDefinitions.h"
#include "LibLoader.h"
#include "Keys.h"
#include "Mouse.h"

#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>

#include "stack.h"

// Test include
#include "LoadTexture.h"

bool VerifyIntegrity( SCommonObjectHeader * pObject, visint base );
bool Object_Verify( SCommonObjectHeader * pObject, visint base );
bool Camera_Verify( SCommonObjectHeader * pObject, visint base );
bool Light_Verify( SCommonObjectHeader * pObject, visint base );
bool VertexGroup_Verify( VisVertexGroup * pObject, visint base );
bool Model_Verify( VisModel * pObject, visint base );
bool Material_Verify( VisMaterial * pObject, visint base );
bool Matrix_Verify( VisMatrix * pObject, visint base );
bool Instance_Verify( VisInstance * pObject, visint base );
bool Offset_Verify( SCommonObjectHeader * pObject, visint base );
bool VertexArray_Verify( VisVertexArray * pObject, visint base );

patchresourcecbf verifyResourceCbf[VIT_VisionaryIdTypesMax];

struct SNotifyVars
{
	SFrameNotify buffer[300], // for both the freelist and the 
				*freelist;

} g_NotifyVars;

patchresourcecbf patchResourceCbf[VIT_VisionaryIdTypesMax];
patchresourcecbf patchSharedResourceCbf[2];

// need to move these to a better place?
resourcecbf initializeResourceCbf[VPT_VisionaryPassTypesMax];
resourcecbf finalizeResourceCbf[VPT_VisionaryPassTypesMax];

bool Initialize_Object( SGlobalData&, void* ){ return true; }
bool Finalize_Object( SGlobalData&, void* ){ return true; }

bool Resource_Initialize( void )
{
	// Initialize
	g_NotifyVars.freelist = (SFrameNotify*)SStack_InitializeBuffer( (SStack*)g_NotifyVars.buffer, sizeof(g_NotifyVars.buffer)/sizeof(*g_NotifyVars.buffer), sizeof(*g_NotifyVars.buffer));
	_ASSERT( g_NotifyVars.freelist && "Error: Attempting to initialize freelist but returned NULL!\n");

	// setup patch functions ... void / NULL not valid
	{for(int i=0; i<VPT_VisionaryPassTypesMax; i++)
	{
		initializeResourceCbf[i] = (resourcecbf)Initialize_Object;
		finalizeResourceCbf[i] = (resourcecbf)Finalize_Object;
	}}

	{for(int i=0; i<VIT_VisionaryIdTypesMax;i++)
	{
		// Place all callback initialization here
		patchResourceCbf[i] = (patchresourcecbf)Resource_Patch;
		renderResourceCbf[i] = (renderresourcecbf)Object_Render;
	}}

	{for(int i=0; i<2;i++){
		// Place all callback initialization here
		patchSharedResourceCbf[i] = (patchresourcecbf)Resource_Patch;
	}}

	/* Test Code ********************************************************/
	ZeroMemory( verifyResourceCbf, sizeof(verifyResourceCbf) );
	
	verifyResourceCbf[VIT_OFFSET] = (patchresourcecbf)Offset_Verify;
	verifyResourceCbf[VIT_CAMERA] = (patchresourcecbf)Camera_Verify;
	verifyResourceCbf[VIT_LIGHT] = (patchresourcecbf)Light_Verify;
	verifyResourceCbf[VIT_VERTEXARRAY] = (patchresourcecbf)VertexArray_Verify;
	verifyResourceCbf[VIT_VERTEXGROUP] = (patchresourcecbf)VertexGroup_Verify;
	verifyResourceCbf[VIT_MATERIAL] = (patchresourcecbf)Material_Verify;
	verifyResourceCbf[VIT_MODEL] = (patchresourcecbf)Model_Verify;
	verifyResourceCbf[VIT_MATRIX] = (patchresourcecbf)Matrix_Verify;
	verifyResourceCbf[VIT_INSTANCE] = (patchresourcecbf)Instance_Verify;
	verifyResourceCbf[VIT_CAMERAINSTANCE] = (patchresourcecbf)Instance_Verify;

	/******************************************************** Test Code */

	//////////////// Default Patch Routines ////////////////

	patchResourceCbf[VIT_OFFSET] = (patchresourcecbf)Resource_PatchOffset;
	patchResourceCbf[VIT_CAMERA] = (patchresourcecbf)Camera_Patch;
	patchResourceCbf[VIT_LIGHT] = (patchresourcecbf)Light_Patch;
	patchResourceCbf[VIT_VERTEXGROUP] = (patchresourcecbf)VertexGroup_Patch;
	patchResourceCbf[VIT_MODEL] = (patchresourcecbf)Model_Patch;
	patchResourceCbf[VIT_INSTANCE] = (patchresourcecbf)Instance_Patch;
	patchResourceCbf[VIT_CAMERAINSTANCE] = (patchresourcecbf)Instance_Patch;
	patchResourceCbf[VIT_TEXTURE] = (patchresourcecbf)Texture_Patch;
	patchResourceCbf[VIT_MATERIAL] = (patchresourcecbf)Material_Patch;

	/////////////////////////////////////////////////////////

	patchSharedResourceCbf[0] = (patchresourcecbf)Linear_Patch;
	patchSharedResourceCbf[1] = (patchresourcecbf)Binary_Patch;

	//////////////// Default Render Routines ////////////////

	renderResourceCbf[VIT_INSTANCE] = (renderresourcecbf)Instance_Render;
	renderResourceCbf[VIT_CAMERAINSTANCE] = (renderresourcecbf)CameraInstance_Render;

	extern SLoadLibs g_LoadLibs[];
	extern int g_nLoadLibs;

	// walk the modules and attempt to patch to the appropriate function
	{SLoadLibs *pEnd = g_LoadLibs + g_nLoadLibs;
	for( SLoadLibs *pBeg = g_LoadLibs; pBeg < pEnd; pBeg++ )
	{
		SLibFuncTabl *pEnd2 = pBeg->pLibFuncTabl + pBeg->nLibFuncTabl;
		for( SLibFuncTabl *pBeg2 = pBeg->pLibFuncTabl; pBeg2 < pEnd2; pBeg2++ )
		{			
			switch( pBeg2->ePass )
			{
			case VPT_INITIALIZE:
				if( initializeResourceCbf[pBeg2->eType] == (resourcecbf)Initialize_Object )
				{
					initializeResourceCbf[pBeg2->eType] = (resourcecbf)pBeg2->fpFunct;
					DBGPRINT( (msg, "Added %s to initializeResourceCbf as %d\n", pBeg2->sName, pBeg2->eType ) )
				}
				break;
			case VPT_FINALIZE:
				if( finalizeResourceCbf[pBeg2->eType] == (resourcecbf)Finalize_Object )
				{
					finalizeResourceCbf[pBeg2->eType] = (resourcecbf)pBeg2->fpFunct;
					DBGPRINT( (msg, "Added %s to finalizeResourceCbf as %d\n", pBeg2->sName, pBeg2->eType ) )
				}
				break;
			case VPT_RESOURCE:
				if( patchResourceCbf[pBeg2->eType] == (patchresourcecbf)Resource_Patch )
				{
					patchResourceCbf[pBeg2->eType] = (patchresourcecbf)pBeg2->fpFunct;
					DBGPRINT( (msg, "Added %s to patchResourceCbf as %d\n", pBeg2->sName, pBeg2->eType ) )
				}
				break;
			case VPT_RENDER:
				if( renderResourceCbf[pBeg2->eType] == (renderresourcecbf)Object_Render )
				{
					renderResourceCbf[pBeg2->eType] = (renderresourcecbf)pBeg2->fpFunct;
					DBGPRINT( (msg, "Added %s to renderResourceCbf as %d\n", pBeg2->sName, pBeg2->eType ) )
				}
				break;
			case VPT_BEGINFRAME:
				{
					SFrameNotify *pItem = (SFrameNotify*)SStack_GetFree( (SStack*&)g_NotifyVars.freelist );
					pItem->modulecbf = (renderresourcecbf)pBeg2->fpFunct;
					SStack_Push( (SStack*&)beginFrameCbf, (SStack*)pItem );
					DBGPRINT( (msg, "Added %s to beginFrameCbf as %d\n", pBeg2->sName, pBeg2->eType ) )
				}				
				break;
			case VPT_ENDFRAME:
				{
					SFrameNotify *pItem = (SFrameNotify*)SStack_GetFree( (SStack*&)g_NotifyVars.freelist );
					pItem->modulecbf = (renderresourcecbf)pBeg2->fpFunct;
					SStack_Push( (SStack*&)endFrameCbf, (SStack*)pItem );
					DBGPRINT( (msg, "Added %s to endFrameCbf as %d\n", pBeg2->sName, pBeg2->eType ) )
				}				
				break;
			case VPT_TIMER:
			case VPT_SCRIPT:
			case VPT_INPUT:
			case VPT_COLLISION:
			case VPT_SPATIALITY:
			case VPT_POSTPROCESSING:
			case VPT_FILEPROCESSING:
			default:
				DBGPRINT( (msg, "Attempt to register %s unknown pass as %d, ignored\n", pBeg2->sName, pBeg2->eType ) )
				break;
			}
		}
	}}

	{
		SFrameNotify *pItem = (SFrameNotify*)SStack_GetFree( (SStack*&)g_NotifyVars.freelist );
		pItem->modulecbf = (renderresourcecbf)Keys_EndFrame;
		SStack_Push( (SStack*&)endFrameCbf, (SStack*)pItem );
		DBGPRINT( (msg, "Added %s to endFrameCbf as %d\n", "Keys_EndFrame", VPT_ENDFRAME ) );

		pItem = (SFrameNotify*)SStack_GetFree( (SStack*&)g_NotifyVars.freelist );
		pItem->modulecbf = (renderresourcecbf)Mouse_EndFrame;
		SStack_Push( (SStack*&)endFrameCbf, (SStack*)pItem );
		DBGPRINT( (msg, "Added %s to endFrameCbf as %d\n", "Mouse_EndFrame", VPT_ENDFRAME ) );
	}

	// Launch initialize functions here ...
	// pass in utility functions

	// Call the initialize functions

	Keys_Initialize( &g_GlobalData, NULL ); // these are special routines that aren't in
	Mouse_Initialize( &g_GlobalData, NULL ); // the normal list (fix later)

	{resourcecbf *pEnd = initializeResourceCbf + VPT_VisionaryPassTypesMax;
	for( resourcecbf *pBeg=initializeResourceCbf; pBeg < pEnd; pBeg++ )
	{
		(*pBeg)( &g_GlobalData, NULL );
	}}

	/////////////////////////////////////////// Test Code ///////////////////////////////////////////
	//extern GLuint g_glTexId;
	//LoadTexture( g_glTexId );
	/////////////////////////////////////////// Test Code ///////////////////////////////////////////

	FILE* fh = fopen("start.lst","r");
	if( fh )
	{
		char szFilename[300];
		bool success = true;
		while( 1 )
		{
			if( fgets( szFilename, 300, fh) )
			{
				switch( szFilename[0] )
				{
				case '#': case '\r': case '\n':
					break; // skip me!
				default:
					strtok(szFilename, "\r\n" ); // should be unix and win compat
					success = success && Resource_Load( szFilename );
					break;
				}

				continue;
			}

			if( ! feof(fh) )
			{
				DBGPRINT( (msg, "Error: Reading line in start.lst (%d)\n",ferror(fh)) );
				success = false;
			}

			break;
		}

		fclose(fh);
		return success;
	}

	DBGPRINT( (msg,"Error: start.lst could not be found/opened\n") );
	return false;
}

bool Resource_Finalize( void )
{
	// Resource_Unload

	// Call the finalize functions
	{resourcecbf *pEnd = finalizeResourceCbf + VPT_VisionaryPassTypesMax;
	for( resourcecbf *pBeg=finalizeResourceCbf; pBeg < pEnd; pBeg++ )
	{
		(*pBeg)( &g_GlobalData, NULL );
	}}

	return true;
}

bool Resource_Patch( void*, visint )
{
	return true;
}

bool Resource_PatchOffset( visint& offset, visint base )
{
	if( offset )
	{
		offset = base + offset;

		return true;
	}

	return false;
}

bool Resource_Load( SFileHeader *pHeader, visint lBuffer )
{
	bool ret = true;

	// Verify the initial pointer is setup correctly!
	_ASSERT( gp_GameObject->m_pObjectList && "Error: Main Global Scene pointer is NULL, is illegal");

	if(pHeader == NULL)			
		return false;

	if( Resource_PatchOffset( (visint&)pHeader->pSections, (visint) pHeader ) == false )
		return false;

	SSection *pSection = pHeader->pSections,
	         *pEnd = pSection + pHeader->iSections;
	for( ; pSection < pEnd; pSection++ )
	{
		if( Resource_PatchOffset( (visint&)pSection->objects, (visint)pHeader ) == false )
			continue;

		switch( pSection->type )
		{
		default:
			printf("This type (0x%x) has not be defined! (%s,%i)\n",pSection->type,__FILE__,__LINE__);
			break;
		case 0xec730b44: // shared
			{
				SSearchObject *pBeg = (SSearchObject *)pSection->objects;
				if( !patchSharedResourceCbf[pBeg->type](pBeg, (visint)pBeg) )
				{
					OutputDebugString("Shared Resources Not patched!");
					ret = false;
					break;
				}

				ret = ret && gp_GameObject->AddObject( (SSearchObject*)pBeg );
			}
			break;
		case 0x6ba36695: // render
			{
				SCommonObjectHeader *pBeg = (SCommonObjectHeader *)pSection->objects;

				// VerifyIntegrity( pBeg, (visint)pBeg );
				// return false;

				if( !patchResourceCbf[pBeg->id.type]( pBeg, (visint)pBeg ) )
				{
					OutputDebugString("Render Resources Not patched!");
					ret = false;
					break;
				}

				ret = ret && gp_GameObject->AddObject( (VisInstance*)pBeg );
			}
			break;
		}

	}

	return ret;
}


bool Resource_Load( char * szFilename )
{	
	bool ret = false;

	void * buffer;
	visint lBuffer;

	int fdFile = _open( szFilename, _O_SEQUENTIAL | _O_BINARY, _S_IREAD );
	if( fdFile > -1 )
	{
		// Get file size
		lBuffer = _filelength( fdFile );
		if( lBuffer > 0 )
		{		
			// Allocate buffer
			buffer = malloc( lBuffer );

			if( _read( fdFile, buffer, lBuffer ) == lBuffer )				
			{
				ret = Resource_Load( (SFileHeader*)buffer, lBuffer );
			}
		}

		_close( fdFile );
	}

	return ret;
}

// Simple routine to resolve the first level of object pointer
// Also only resolves for the head pointer
bool Resource_Resolve( VisGenericPtr *pObject )
{
	_ASSERT( pObject && "NULL is an invalid argument for this function\n");

	// It's a linear search for now
	// ptr->ptr = gp_GameObject->FindObject( ptr );
	VisGenericPtr* pBeg = gp_GameObject->m_pSearchObjectList->objects,
				 * pEnd = gp_GameObject->m_pSearchObjectList->objects  
					    + gp_GameObject->m_pSearchObjectList->nObjects;

	while( pBeg < pEnd )
	{
		_ASSERT( pBeg->ptr && "Pointer was NULL, which is invalid\n" );
		if( pBeg->ptr->header.id.number == (visint)pObject->ptr )
		{
			pObject->ptr = pBeg->ptr;
			return true;
		}

		pBeg++;
	}

	return false;
}

/**********************************************************************
***********************************************************************/

bool VerifyIntegrity( SCommonObjectHeader * pObject, visint base ){

	Object_Verify( pObject, base );

	if( verifyResourceCbf[pObject->data.id.type] )
	{
		verifyResourceCbf[pObject->data.id.type](pObject, base);
	}
	
	return true;
}

bool Object_Verify( SCommonObjectHeader * pObject, visint base )
{
		DBGPRINT((msg,"Object <%x> Type <%02x,%d> Number <%x,%d>\n"
				  "\tSize <%x,%d> Next <%x,%d>\n", 
				  pObject, pObject->data.id.type, pObject->data.id.type,
				  pObject->data.id.number, pObject->data.id.number,
				  pObject->data.size, pObject->data.size, 
				  (visint)pObject + (visint)pObject->data.size,
				  (visint)pObject + (visint)pObject->data.size));

		return true;
}

bool Camera_Verify( SCommonObjectHeader * pObject, visint base )
{
	return true;
}

bool Light_Verify( SCommonObjectHeader * pObject, visint base )
{
	return true;
}

bool VertexArray_Verify( VisVertexArray * pObject, visint base )
{
	int count = pObject->nVerticies * pObject->vertexsize;
	char *data = new char[10 * count + 25], *string=data;
	for (int i=0;i<count;i++)
	{
		string += sprintf( string, "%s%.3f", ((i>0)?", ":""), pObject->verticies[i] );
	}

//	DBGPRINT((msg,"header <%x,%d>\n", sizeof(pObject->header), sizeof(pObject->header)));

	int size = sizeof(*pObject) + sizeof(*pObject->verticies) * count;

	DBGPRINT((msg,"Vertex Array: type <%x,%d> width <%x,%d> verticies <%x,%d>\n\tSize <%x,%d> [ %s ]\n",
			pObject->vertextype, pObject->vertextype, pObject->vertexsize, pObject->vertexsize,
			pObject->nVerticies, pObject->nVerticies, size, size, data));

	delete [] data;
	
	return true;
}

bool Material_Verify( VisMaterial * pObject, visint base )
{
	// Add this to the other values and see what you get
	int size = sizeof(*pObject) + sizeof(VisTexturePtr) * pObject->nTextures;

	DBGPRINT((msg,"Material: face <%x,%d> mode <%x,%d>\n"
				   "textureobject <%x,%d>\n",
			pObject->face, pObject->face, pObject->mode, pObject->mode,
			pObject->textureObjectArray, pObject->nTextureObjects));

	{{ VisVertexArrayPtr* pBeg = (VisVertexArrayPtr*)pObject->vertexArray,
						* pEnd = pBeg + sizeof(pObject->vertexArray)/sizeof(*pObject->vertexArray);
		while( pBeg<pEnd )
		{
			VerifyIntegrity((SCommonObjectHeader*)((visint)pBeg->ptr + base), base);
			pBeg++;
		}
	}}

	{{ VisTexturePtr *pBeg = (VisTexturePtr*)((visint)pObject->textureArray + base),
				     *pEnd = pBeg + pObject->nTextures;
		while( pBeg<pEnd )
		{
			VerifyIntegrity((SCommonObjectHeader*)((visint)pBeg->ptr + (visint)base), (visint)base);
			pBeg++;
		}
	}}
	
	return true;
}

bool VertexGroup_Verify( VisVertexGroup * pObject, visint base )
{
	DBGPRINT((msg,"VertexGroups: material <%x,%d> prim <%x,%d> vertex array <%x, %d>\n", 
		pObject->material.ptr, pObject->material.type,
		pObject->primType, pObject->primType,
		pObject->vertexArray, pObject->nArrays
		));

	VerifyIntegrity((SCommonObjectHeader*)((visint)pObject->material.ptr + (visint)pObject), (visint)pObject);

	{{ VisVertexGroupPtr* pBeg = (VisVertexGroupPtr*)pObject->vertexArray,
						* pEnd = pBeg + pObject->nArrays;
		while( pBeg<pEnd)
		{
			VerifyIntegrity((SCommonObjectHeader*)((visint)pBeg->ptr + (visint)pObject), (visint)pObject);
			pBeg++;
		}
	}}
	
	return true;
}

bool Model_Verify( VisModel * pObject, visint base )
{
	// calculate size
	int size = sizeof(*pObject) + sizeof(pObject->vertexGroup) * pObject->nGroups; // add on vertex groups

	DBGPRINT((msg,"Model: subtype <%x,%d> vertex groups <%x,%d>\n", 
		pObject->subtype.subtype, pObject->subtype.subtype, 
		pObject->vertexGroup, pObject->nGroups
		));

	{{ VisVertexGroupPtr* pBeg = (VisVertexGroupPtr*)((visint)pObject->vertexGroup + (visint)pObject),
						* pEnd = pBeg + pObject->nGroups;
		while( pBeg<pEnd)
		{
			VerifyIntegrity((SCommonObjectHeader*)((visint)pBeg->ptr + (visint)pObject), (visint)pObject);
			pBeg++;
		}
	}}
	
	return true;
}

bool Matrix_Verify( VisMatrix * pObject, visint base )
{
	char matrixdata[150], *string=matrixdata;
	int count = sizeof(pObject->matrix)/sizeof(*pObject->matrix);
	for (int i=0;i<count;i++)
	{
		string += sprintf( string, "%s%.3f", ((i>0)?", ":""), pObject->matrix[i] );
	}

	int size = sizeof(*pObject);

	DBGPRINT((msg,"Matrix: mode <%x,%d> size <%x,%d> calc <%x,%d>\n\t [ %s ]\n", pObject->matrixMode, 
		pObject->header.data.size, pObject->header.data.size, size, size,
		pObject->matrixMode, matrixdata));	

	return true;
}

bool Offset_Verify( SCommonObjectHeader * pObject, visint base )
{
	return verifyResourceCbf[pObject->id.type]( pObject, base );
}

bool Instance_Verify( VisInstance * pObject, visint base )
{
	DBGPRINT((msg,"Instance: child <%x,%d> sibling <%x,%d>\n"
		"\tmatricies <%x, %d> flags <%x>\n"
		"\t object <%x,%d>\n",
		pObject->child.ptr, pObject->child.type, pObject->sibling.ptr, pObject->sibling.type,
		pObject->pMatricies, pObject->nMatricies, pObject->flags,
		pObject->object.ptr, pObject->object.type));

	// Validate Matricies
	{VisMatrixPtr * pBeg = (VisMatrixPtr*)((visint)pObject->pMatricies + base),
				  * pEnd = pBeg + pObject->nMatricies;
	while( pBeg < pEnd )
	{
		VerifyIntegrity((SCommonObjectHeader*)((visint)pBeg->ptr + base), base);
		pBeg++;
	}}
	
	// Validate Object
	if(pObject->object.ptr)
	{{SCommonObjectHeader* obj = (SCommonObjectHeader*)((visint)pObject->object.ptr + base);
	VerifyIntegrity(obj, base);}}

	// Validate Children
	if(pObject->child.ptr)
	{{SCommonObjectHeader* obj = (SCommonObjectHeader*)((visint)pObject->child.ptr + base);
	VerifyIntegrity(obj, base);}}
	
	// Validate Siblings
	if(pObject->sibling.ptr)
	{{SCommonObjectHeader* obj = (SCommonObjectHeader*)((visint)pObject->sibling.ptr + base);
	VerifyIntegrity(obj, base);}}
	
	return true;
}

	
bool Linear_Patch( SSearchObject *pObject, visint base )
{
	// patch the pointers in the header ...
	if( Resource_PatchOffset( (visint&)pObject->objects, (visint)pObject ) )
	{
		VisGenericPtr *pBeg = pObject->objects,
					  *pEnd = pObject->objects + pObject->nObjects;

		while( pBeg < pEnd ) 
		{
			if( pBeg->type == VIT_OFFSET
			&&  Resource_PatchOffset( (visint&)pBeg->ptr, (visint)pObject ) )
			{
				pBeg->type = pBeg->ptr->header.id.type;
				_ASSERT( pBeg->type >= 0 && pBeg->type < VIT_VisionaryIdTypesMax );
				patchResourceCbf[pBeg->type]( pBeg->ptr, (visint)pObject );
			}

			pBeg++;
		}
	}

	// Keep this one second because pObject->objects will be patched
	if( Resource_PatchOffset( (visint&)pObject->searchobj, (visint)pObject ) )
	{
		SBtree *pBeg = (SBtree*)pObject->searchobj,
			   *pEnd = (SBtree*)pObject->objects;

		while( pBeg < pEnd ) 
		{
			Resource_PatchOffset( (visint&)pBeg->left, (visint)pObject );

			Resource_PatchOffset( (visint&)pBeg->right, (visint)pObject );

			Resource_PatchOffset( (visint&)pBeg->object, (visint)pObject );

			pBeg++;
		}
	}

	return true;
}

bool Binary_Patch( SSearchObject *pObject, visint base )
{
	return Linear_Patch(pObject, base);
}
