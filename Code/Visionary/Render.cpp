
#include "stdafx.h"
#include "Render.h"
#include "Resources.h"
#include "LoadTexture.h"

renderresourcecbf renderResourceCbf[VIT_VisionaryIdTypesMax];
SFrameNotify *beginFrameCbf = NULL;
SFrameNotify *endFrameCbf = NULL;

bool Render_Iterate( VisInstance* pInstance, VisInstance *pParent, double deltaTime, int nFrames );

// This is the default function
bool Object_Render( VisGeneric* pObject, VisInstance *pInstance, VisInstance *pParent, double deltaTime, int nFrames )
{
	return true;
}

void PushMatricies( VisInstance *pInstance )
{
	// push on matrix stack
	{visint prevMode = -1;
	for( VisMatrixPtr *curMat = pInstance->pMatricies,
		 *endMat = pInstance->pMatricies + pInstance->nMatricies; curMat < endMat; curMat++ )
	{
		VisMatrix *pMatrix = curMat->ptr;

#ifdef _DEBUG // Verifies that matricies are sorted ... can check this in the Patch routine as well
		int matrixType[4] = {0,0,0,0},
			freeplatz = -1;
		bool marker = false;
		for(int i =0;i<sizeof(matrixType)/sizeof(*matrixType);i++)
		{ 
			if( freeplatz < 0 && matrixType[i] == 0 ) freeplatz = i;
			if( marker == true && matrixType[i] != 0 && matrixType[i] != pMatrix->matrixMode) { _ASSERT( 0 && "Matricies need to be sorted by matrix mode!"); freeplatz = -1; break; }
			if( matrixType[i] == pMatrix->matrixMode ) marker = true;
		}

		if(freeplatz>=0)
			matrixType[freeplatz] = pMatrix->matrixMode;
#endif 

		if( prevMode != pMatrix->matrixMode )
		{
			prevMode = pMatrix->matrixMode;
			glMatrixMode( pMatrix->matrixMode );
			glPushMatrix();
		}

		glMultMatrixf(pMatrix->matrix);
	}}
}

void PopMatricies( VisInstance *pInstance )
{
	// Pop Instance Matrix
	{visint prevMode = -1;
	for( VisMatrixPtr *curMat = pInstance->pMatricies,
		 *endMat = pInstance->pMatricies + pInstance->nMatricies; curMat < endMat; curMat++ )
	{
		VisMatrix *pMatrix = curMat->ptr;
		if( prevMode != pMatrix->matrixMode )
		{
			prevMode = pMatrix->matrixMode;
			glMatrixMode( pMatrix->matrixMode );
			glPopMatrix();
		}
	}}
}

bool Instance_Render( VisGeneric* pObject, VisInstance* pInstance, VisInstance *pParent, double deltaTime, int nFrames )
{
	PushMatricies( pInstance );

	// run object render
	_ASSERT( renderResourceCbf[pInstance->object.type] );
	renderResourceCbf[pInstance->object.type]( pInstance->object.ptr, pInstance, pParent, deltaTime, nFrames );

	// run child render
	_ASSERT( renderResourceCbf[pInstance->child.type] );
	Render_Iterate( pInstance->child.ptr, pParent, deltaTime, nFrames );

	PopMatricies( pInstance );

	return true;
}

bool CameraInstance_Render( VisGeneric* pObject, VisInstance* pInstance, VisInstance *pParent, double deltaTime, int nFrames )
{
	// run object render
	_ASSERT( renderResourceCbf[pInstance->object.type] );
	renderResourceCbf[pInstance->object.type]( pInstance->object.ptr, pInstance, pParent, deltaTime, nFrames );

	// Camera must update before changing the matricies
	PushMatricies( pInstance );

	// run child render
	Render_Iterate( pInstance->child.ptr, pParent, deltaTime, nFrames );

	PopMatricies( pInstance );
	
	return true;
}

bool Render_Iterate( VisInstance* pInstance, VisInstance *pParent, double deltaTime, int nFrames )
{
	while( pInstance )
	{
		// Call the instance function
#ifdef _DEBUG
		switch( pInstance->header.data.id.type )
		{
		case 0x0b: case 0x1a: case 0x1b:
			break;
		default:
			_ASSERT( 0 && "This object was not an instance object!\n" );
			break;
		}
#endif
		renderResourceCbf[pInstance->header.data.id.type]( NULL, pInstance, pParent, deltaTime, nFrames );

		// update pInstance to sibling
		pInstance = pInstance->sibling.ptr;
	}

	return true;
}

// GLuint g_glTexId = 0;

bool Render_Update( void * )
{
	static bool lastState = true;
	static double deltaTime;
	static int nFrames = 0;
	static LARGE_INTEGER frameEndTime, frameBegTime;

#ifndef _DEBUG

	///////////////////////////////////////////////////////
	// Temporary - try python script, placed here to make 
	//			 - the camera update and the character update
	//           - happen at the same time 

	extern void PythonModule_Script( VisScript *pScript, int nCount, char* fmt, ... );
	VisScript script = {
		{{0,{0,VIT_SCRIPT}},{0,VIT_SCRIPT}},
			"..\\..\\Data\\Scripts\\ProcessInput.py"};

	PythonModule_Script( &script, 0, NULL );

#endif

	{for( SFrameNotify *pBeg = beginFrameCbf; pBeg; pBeg = (SFrameNotify*)pBeg->node.next )
	{
		pBeg->modulecbf( NULL, NULL, NULL, deltaTime, nFrames );
	}}

	// Test code
	// glBindTexture(GL_TEXTURE_2D, g_glTexId);

	VisInstancePtr *pEnd = gp_GameObject->m_pObjectList + gp_GameObject->m_nObjectList;
	for( VisInstancePtr *pBeg = gp_GameObject->m_pObjectList; pBeg < pEnd; pBeg++ )
	{
		_ASSERT( renderResourceCbf[pBeg->type] && renderResourceCbf[pBeg->type] == renderResourceCbf[pBeg->ptr->header.data.id.type] );
		Render_Iterate( pBeg->ptr, NULL, deltaTime, nFrames );
	}

	{for( SFrameNotify *pBeg = endFrameCbf; pBeg; pBeg = (SFrameNotify*)pBeg->node.next )
	{
		pBeg->modulecbf( NULL, NULL, NULL, deltaTime, nFrames );
	}}

	QueryPerformanceCounter(&frameEndTime);

	// calculate delta!
	g_GlobalData.deltaTime = \
		deltaTime = ((visfloat)(frameEndTime.QuadPart - frameBegTime.QuadPart)/(visfloat)g_GlobalData.timerFrequency.QuadPart);

	QueryPerformanceCounter(&frameBegTime);

	nFrames++;
	if(nFrames > 100)
	{	
		// Calculate and display frame rate!
		char szOutBuffer[32];
		visfloat fps;

		// Get the current time
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);

		fps = (visfloat)nFrames/((visfloat)(currentTime.QuadPart - 
												g_GlobalData.startTime.QuadPart)/
										(visfloat)g_GlobalData.timerFrequency.QuadPart);
		
		sprintf(szOutBuffer,"%0.1f",fps);
		SetWindowText(g_GlobalData.hWnd, szOutBuffer);
		nFrames = 0;

		// Resets the timer
		QueryPerformanceCounter(&g_GlobalData.startTime);
	}

	return true;										// Everything Went OK
}

/////////////////////////////////////////////////////////////////////////////////
// Patch Functions

bool VertexGroup_Patch( VisVertexGroup *pObject, visint base )
{
	VisVertexArrayPtr *pBeg, *pEnd;

	// should move this into unique function
	if( pObject->material.type == VIT_OFFSET 
	&&	Resource_PatchOffset( (visint&)pObject->material.ptr, (visint)pObject ) )
	{
		pObject->material.type = pObject->material.ptr->header.id.type;

		// Patch texture data!
		patchResourceCbf[pObject->material.type]( pObject->material.ptr, (visint)pObject); // probably should be doing this
		
		pBeg = pObject->material.ptr->vertexArray,
		pEnd = pObject->material.ptr->vertexArray + sizeof(pObject->material.ptr->vertexArray)/sizeof(*(pObject->material.ptr->vertexArray)); // always 5

		// Patch Material
		while( pBeg<pEnd )
		{
			if( pBeg->type == VIT_OFFSET 
			&&	Resource_PatchOffset( (visint&)pBeg->ptr, (visint)pObject ) )
			{
				pBeg->type = pBeg->ptr->header.id.type;
				patchResourceCbf[pBeg->type]( pBeg->ptr, (visint)pObject);
			}

			pBeg++;
		}
	}

	{
		pBeg = pObject->vertexArray,
		pEnd = pObject->vertexArray + pObject->nArrays;

		// Patch Verticies
		while( pBeg<pEnd )
		{
			if( pBeg->type == VIT_OFFSET 
			&&	Resource_PatchOffset( (visint&)pBeg->ptr, (visint)pObject ) )
			{
				pBeg->type = pBeg->ptr->header.id.type;
				patchResourceCbf[pBeg->type]( pBeg->ptr, (visint)pObject );
			}		

			pBeg++;
		}
	}

	return true;
}

bool Model_Patch( VisModel* pObject, visint base)
{
	if( Resource_PatchOffset( (visint&)pObject->vertexGroup, (visint)pObject ) )
	{
		VisVertexGroupPtr *pBeg = pObject->vertexGroup,
						  *pEnd = pObject->vertexGroup + pObject->nGroups;

		// Patch Vertex Group
		while( pBeg<pEnd )
		{
			if( pBeg->type == VIT_OFFSET
			&&	Resource_PatchOffset( (visint&)pBeg->ptr, (visint)pObject ) )
			{
				pBeg->type = pBeg->ptr->header.id.type;
				_ASSERT( pBeg->type >= 0 && pBeg->type < VIT_VisionaryIdTypesMax );
				patchResourceCbf[pBeg->type]( pBeg->ptr, (visint)pObject );
			}

			pBeg++;
		}
	}

	return true;
}

bool Material_Patch( VisMaterial* pObject, visint base)
{
	// Patch the texture data
	if( Resource_PatchOffset( (visint&)pObject->textureArray, base ) )
	{
		VisTexturePtr *pBeg = pObject->textureArray,
					  *pEnd = pObject->textureArray + pObject->nTextures;

		while( pBeg<pEnd )
		{
			if( pBeg->type == VIT_OFFSET
			&&	Resource_PatchOffset( (visint&)pBeg->ptr, (visint)pBeg )
			||  pBeg->type == VIT_IDENT 
			&&  Resource_Resolve( (VisGenericPtr*)pBeg ) & 0           )
			{
				pBeg->type = pBeg->ptr->header.id.type;
				_ASSERT( pBeg->type >= 0 && pBeg->type < VIT_VisionaryIdTypesMax );
				patchResourceCbf[pBeg->type]( pBeg->ptr, (visint)pObject );
			}

			pBeg++;
		}

	}

	return true;
}

bool Texture_Patch( VisTexture* pObject, visint base)
{
	if( Resource_PatchOffset( (visint&)pObject->bytes, (visint)pObject ) )
	{
		return LoadTextureIntoGL((GLuint&)pObject->iTexture, pObject->bytes, pObject->width, pObject->height, pObject->depth, pObject->size, GL_UNSIGNED_BYTE, (pObject->xrepeat|pObject->yrepeat) > 1);
	}

	return false;
}

bool Camera_Patch( VisCamera* pObject, visint base )
{
	_ASSERT(gp_GameObject);

	if( gp_GameObject->TopCamera() == NULL )
	{
		// Register the first camera found
		return gp_GameObject->PushCamera( (VisInstance*)base );
	}

	return true;
}

bool Light_Patch( VisLight* pObject, visint base )
{
	if( pObject->pVerticies // patch me!
	&&  Resource_PatchOffset( (visint&)pObject->pVerticies, (visint)pObject ) )
	{
		VisVertexArrayPtr *pBeg = pObject->pVerticies,
						  *pEnd = pObject->pVerticies + pObject->nVerticies;
		
		// Patch Verticies
		while( pBeg<pEnd )
		{
			if( pBeg->type == VIT_OFFSET
			&&	Resource_PatchOffset( (visint&)pBeg->ptr, (visint)pObject ) )
			{
				pBeg->type = pBeg->ptr->header.id.type;
				patchResourceCbf[pBeg->type]( pBeg->ptr, (visint)pObject );
			}

			pBeg++;
		}

	}

	return true;
}

bool Instance_Patch( VisInstance* pObject, visint base )
{
	// HACK to get the Player Object (until menus are working!)
	if( pObject->header.id.number == 0x61E560F5 ) // Plane.Player
	{
		gp_GameObject->m_PlayerObject.type = VIT_INSTANCE;
		gp_GameObject->m_PlayerObject.ptr = pObject;
	}

	if( pObject->pMatricies 
	&&  Resource_PatchOffset( (visint&)pObject->pMatricies, (visint)pObject ) )
	{
		for( VisMatrixPtr *curMat = pObject->pMatricies,
			*endMat = pObject->pMatricies + pObject->nMatricies; curMat < endMat; curMat++)
		{
			if( curMat->type == VIT_OFFSET 
			&&  Resource_PatchOffset( (visint&)curMat->ptr, (visint)pObject ) )
			{
				curMat->type = curMat->ptr->header.data.id.type;
				patchResourceCbf[curMat->type]( curMat->ptr, (visint)pObject);				
			}
		}
	}

	// Patch main object
	if( pObject->object.type == VIT_OFFSET
	&&	Resource_PatchOffset( (visint&)pObject->object.ptr, (visint)pObject ) )
	{
		pObject->object.type = pObject->object.ptr->header.data.id.type;
		patchResourceCbf[pObject->object.type]( pObject->object.ptr, (visint)pObject );
	}

	// Patch child
	if( pObject->child.type == VIT_OFFSET
	&&	Resource_PatchOffset( (visint&)pObject->child.ptr, (visint)pObject ) )
	{
		pObject->child.type = pObject->child.ptr->header.data.id.type;
		patchResourceCbf[pObject->child.type]( pObject->child.ptr, (visint)pObject );
	}

	// patch sibling
	if( pObject->sibling.type == VIT_OFFSET
  	&&	Resource_PatchOffset( (visint&)pObject->sibling.ptr, (visint)pObject ) )
	{
		pObject->sibling.type = pObject->sibling.ptr->header.data.id.type;
		patchResourceCbf[pObject->sibling.type]( pObject->sibling.ptr, (visint)pObject );
	}

	return true;
}
