// Renderer.cpp : Defines the entry point for the DLL application.
//

// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dllproc/base/loadlibrary.asp

#include "stdafx.h"
#include "Renderer.h"
#include "Viewport.h"
#include <TypeDefinitions.h>
#include <Camera.h>
#include <assert.h>

SGlobalData *gp_GlobalData = NULL;

SCamera	cam;


//////////////////////////////////////////////////////////////////////////////////
// Dll Entry Point

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

extern void PythonImport_Initialize( void );

__inline bool BindTexture(GLenum target, GLuint texture)
{
	glBindTexture(target, texture);
	return true;
}

bool Render_Initialize( SGlobalData *pGlobalData, void* ) // arg must be the game structure
{
#ifndef _DEBUG
	// Load python modules!
	PythonImport_Initialize();
#endif

	// Initialize Viewport
	if( Viewport_Initialize( pGlobalData, NULL ) )
	{
		// Initialize Model State
		glShadeModel(GL_SMOOTH);							// Enables Smooth Shading model
		glClearColor(0.0f, 0.0f, 0.25f, 0.25f);				// Black Background
		glClearDepth(1.0f);									// Depth Buffer Setup
		glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
		glEnable(GL_CULL_FACE);
//		glEnable( GL_NORMALIZE );		// slower remove (just for testing)

		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_BLEND);

		glDepthFunc(GL_LEQUAL);								// The Type Of Depth Test To Do
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

//		glBlendFunc( GL_SRC_ALPHA_SATURATE, GL_ONE );
		glEnable( GL_POLYGON_SMOOTH );
//		glDisable( GL_DEPTH_TEST );

		// glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )

		glHint (GL_FOG_HINT, GL_DONT_CARE);					// Set the default calculation mode to best possible
		glEnable (GL_FOG);

		QueryPerformanceFrequency(&pGlobalData->timerFrequency);
		QueryPerformanceCounter(&pGlobalData->startTime);

		gp_GlobalData = pGlobalData;
	
		// Initialize the camera system
		Camera_Reset(&cam);
		Camera_SetLookAt(&cam, -79.271591f,	-17.914f, 1.997463f);	// Need to make this a nice function...  attach to object...

		return true;
	}

	return false;
}

bool Render_Finalize( SGlobalData *pGlobalData, void* )
{
	Viewport_Finalize( pGlobalData, NULL );

	return true;
}

/**********************************************
//	Renderer Feature List:
//		Completed:
//			Vertex Arrays (user defined vertex size)
//			Normal Arrays (user defined vertex size)
//			Color Arrays (user defined vertex size)
//			Matrix Groups per Instance
//			Per Vertex Array Material Definition
//			Sibling/Parenting
//			Custom Rendering Routines (Instance, Model, Light, Camera)
//
//		Planning:
//			Textures per Material
//			UV Arrays
//			Custom Rendering Routines (All Objects)
//
***********************************************/

int g_Lights[] = { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7 };
int g_MaxLight = sizeof(g_Lights)/sizeof(*g_Lights);
int g_CurLight = 0;

int Get_NextLight( int &iLight )
{
	iLight = g_Lights[g_CurLight++];
	return g_CurLight - g_MaxLight;
}

int Get_PrevLight( int &iLight )
{
	iLight = g_Lights[g_CurLight--];
	return g_CurLight;
}

// Start at front, move to back, move from back to front ... so on so forth
int Get_LightIndex()
{
	int iLight = -1;
	static int (*GetLight)( int &iLight ) = Get_NextLight;

	switch( GetLight( iLight ) )
	{
	case -1:
		GetLight = Get_PrevLight;
		break;
	case 0:
		GetLight = Get_NextLight;
		break;
	default:
		break;

	}

	return iLight;
}

bool Render_BeginFrame( VisLight*, VisInstance *, VisInstance *, double, int )
{
	g_CurLight = 0; // reset the light counter!

	// this is hard coded craziness
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glColor3f(0.7f,0.7f,0.7f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);					// Select The Modelview Matrix
	glLoadIdentity();							// Reset The Modelview Matrix

	GLfloat fogColor[] = {0.2f, 0.2f, 0.2f, 1}; // Setup array for fog color of grey

	glFogi (GL_FOG_MODE, GL_LINEAR);// Fog fade in is linear (this looks best IMHO)
	glFogfv (GL_FOG_COLOR, fogColor);// Set the fog color in GL
	glFogf (GL_FOG_DENSITY, .35f);     // Set the default density
	
	// Matrix Transformation order
	//	Viewing
	//	Modeling - Modelview
	//	Projection -> Perspective
	//	Viewport

	// let's turn on some lights!
	/*{
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
		glDisable(GL_LIGHT3);
		glDisable(GL_LIGHT4);
		glDisable(GL_LIGHT5);
		glDisable(GL_LIGHT6);
		glDisable(GL_LIGHT7);		

		GLfloat mat_ambient[] = { .5f, .5f, .5f, 1 };
		GLfloat mat_diffuse[] = { .5f, .5f, .5f, 1 };
	    GLfloat mat_specular[] = { .1f, .1f, .1f, 1 };
		GLfloat mat_emission[] = { .1f, .1f, .1f, 1 };
		GLfloat mat_shininess[] = { 5 };
		GLfloat light_position[] = { 35.0f, 35.0f, 35.0f, 1.0f };

		glLightfv(GL_LIGHT0, GL_AMBIENT, mat_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, mat_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, mat_specular);
		glLightfv(GL_LIGHT0, GL_SHININESS, mat_shininess);
		glLightfv(GL_LIGHT0, GL_EMISSION, mat_emission);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);

		glEnable(GL_LIGHT0);
		
		// glDepthFunc(GL_LEQUAL);
		// glEnable(GL_DEPTH_TEST);
	}*/

	Camera_UpdateMatrix(&cam);
	glMultMatrixf(Camera_GetMatrix(&cam)->matrix);

	return true;
}

bool Light_Render( VisLight* pObject, VisInstance *pInstance, VisInstance *pParent, double deltaTime, int nFrames )
{
	int iLight = Get_LightIndex();

	for( int i=0; i<pObject->nVerticies;i++ )
	{
		glLightfv(iLight, pObject->pVerticies[i].ptr->vertextype, pObject->pVerticies[i].ptr->verticies);
	}

	glEnable(iLight);
	
	return true;
}

bool Camera_Render( VisCamera* pObject, VisInstance* pInstance, VisInstance *pParent, double deltaTime, int nFrames )
{
	if( pObject->fAspect == 0)
	{
		RECT winRect;
		if( GetClientRect( gp_GlobalData->hWnd, &winRect ) == FALSE)
		{
			return false;
		}

		// Calculate The Aspect Ratio Of The Window
		pObject->fAspect = winRect.right*1.0f/winRect.bottom;
	}

	glFogf (GL_FOG_START, pObject->fNear);      // Set the near fog clipping plane to 1
	glFogf (GL_FOG_END, pObject->fFar);			// Set the far clipping plane to the server setting
	
	// Creates camera model
	glMatrixMode(GL_PROJECTION);				// Select The Projection Matrix
	glLoadIdentity();							// Reset to keep from messing with other settings!

	// Set Camera	
	if( pObject->iType == 0) // perspective
	{	
		gluPerspective(pObject->fCustom,pObject->fAspect,pObject->fNear,pObject->fFar);
	}
	else
	{
		visfloat nRange = pObject->fCustom, nEgnar = 0-nRange;
		glOrtho (nEgnar*pObject->fAspect, nRange*pObject->fAspect, nEgnar, nRange, pObject->fNear, pObject->fFar);
	}

	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);

	return true;
}

void _RenderCube( void )
{
	GLubyte cubeIndices[] = 
	{
		4,5,6,7,1,2,6,5, // front, right
		0,1,5,4,0,3,2,1, // bottom, back
		0,4,7,3,2,3,7,6  // left, top
	};

	struct SMatrixWeight
	{
		float bias[4];
	};

	GLfloat // x (left,right), y (top/bottom), z (back/front)
		colors[24] = {	1,1,0,
						1,1,0, 
						1,1,0, 
						1,1,0, 
						0,1,1, 
						0,1,1, 
						0,1,1,
						0,1,1  },
		normals[24] = { 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0 },
		vertices[24] = { -10,-10,-10, // 0 left  bottom back 
						  10,-10,-10, // 1 right bottom back 
						  10, 10,-10, // 2 right top    back 
						 -10, 10,-10, // 3 left  top    back 
						 -10,-10, 10, // 4 left  bottom front
						  10,-10, 10, // 5 right bottom front
						  10, 10, 10, // 6 right top    front
						 -10, 10, 10, // 7 left  top    front
	};

	SMatrixWeight vertWeight[8] = 
	{
		{  .50, .50, .00, .00 },	// vert 0, lbb
		{  .0, .0, .0, .0 },		// vert 1, rbb
		{  .0, .0, .0, .0 },		// vert 2, rtb
		{  .0, .0, .0, .0 },		

		{  .0, .0, .0, .0 },		// vert 4, lbf
		{  .0, .0, .0, .0 },
		{  .0, .0, .50, .50 },		// vert 6, rtf
		
		{  .0, .0, .0, .0 },
	};

	float matricies[][16] = 
	{ 
		{ 1, 0, 0, 2, 0, 1, 0, 2, 0, 0, 1, 2, 0, 0, 0, 1 },
		{ 1, 0, 0, 2, 0, 1, 0, 2, 0, 0, 1, 2, 0, 0, 0, 1 },
		{ 1, 0, 0, -2, 0, 1, 0, -2, 0, 0, 1, -2, 0, 0, 0, 1 },
		{ 1, 0, 0, -2,			// 0
		  0, 1, 0, -2,			// 5
		  0, 0, 1, -2,			// 10
		  0, 0, 0, 1 }};		// 15 x, y, z, w

	int nVert = (sizeof(vertWeight)/sizeof(*vertWeight));
	for( int i=0; i<nVert; i++ )
	{
		bool noInfluence = true;

		// Sum (vert[i] * matr * bias)
		int nVertex = i * 3;
		float vert[4] = {0,0,0,0};
		for( int j=0;j<4;j++) // for is max number of matrix
		{
			if( vertWeight[i].bias[j] == 0 )
				continue;

			noInfluence = false;

			// x = x1*a+y1*e+z1*i+m*w1
			vert[0] += ((vertices[nVertex+0]*matricies[j][0])
				      +(vertices[nVertex+1]*matricies[j][1])
					  +(vertices[nVertex+2]*matricies[j][2])
					  +( matricies[j][3]))
					  * vertWeight[i].bias[j];
			// y = x1*b+y1*f+z1*j+n*w1
			vert[1] += ((vertices[nVertex+0]*matricies[j][4])
				      +(vertices[nVertex+1]*matricies[j][5])
					  +(vertices[nVertex+2]*matricies[j][6])
					  +( matricies[j][7]))
					  * vertWeight[i].bias[j];
			// z = x1*c+y1*g+z1*k+o*w1
			vert[2] += ((vertices[nVertex+0]*matricies[j][8])
				      +(vertices[nVertex+1]*matricies[j][9])
					  +(vertices[nVertex+2]*matricies[j][10])
					  +( matricies[j][11]))
					  * vertWeight[i].bias[j];
			// w = x1*d+y1*h+z1*l+p*w1
			//vert[3] += ((vertices[i+0]*matricies[j][12])
			//	      +(vertices[i+1]*matricies[j][13])
			//		  +(vertices[i+2]*matricies[j][14])
			//		  +( matricies[j][15]))
			//		  * vertWeight[i].bias[j];
		}

		if( noInfluence == false )
		{
			// this is a replace algo, I want an augument algo
			// memcpy( (vertices+nVertex), vert, sizeof(float) * 3 );

			// this could be done faster with 4 floating point registers!
			switch(3) // number of verticies
			{
			case 4:
				vertices[nVertex+3] += vert[3];
			case 3:
				vertices[nVertex+2] += vert[2];
			case 2:
				vertices[nVertex+1] += vert[1];
			case 1:
				vertices[nVertex+0] += vert[0];
			default:
				break;
			}
		}
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	// glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);	

	glColorPointer( 3, GL_FLOAT, 0, colors );
	// glNormalPointer( GL_FLOAT, 0, normals );
	glVertexPointer( 3, GL_FLOAT, 0, vertices );

	// glColor3f( .8f,.9f,.99f );
	glDrawElements( GL_QUADS, 24, GL_UNSIGNED_BYTE, cubeIndices );

}

bool Model_Render( VisModel* pObject, VisInstance* pInstance, VisInstance *pParent, double deltaTime, int nFrames )
{
	static GLuint iCurrentTexture = 0;
	static float xRot = 0, yRot = 0, zRot = 90, step = 25.0f;

	// animation
	if(nFrames != 0)
	{
		xRot += static_cast<float>(step * deltaTime);
		yRot += static_cast<float>(step * deltaTime);
		zRot += static_cast<float>(step * deltaTime);
	}

#define ROLLANIMATION(x,cap) if( (x)>(cap) ) { (x) = static_cast<float>(((int)(x))%(cap)); }

	ROLLANIMATION( xRot, 360 );
	ROLLANIMATION( yRot, 360 );
	ROLLANIMATION( zRot, 360 );

#undef ROLLANIMATION

//	glEnableClientState(GL_VERTEX_ARRAY);
//	glEnableClientState(GL_NORMAL_ARRAY);
//	glEnableClientState(GL_COLOR_ARRAY);
//	glEnableClientState(GL_TEXTURE_COORD_ARRAY); // new enabled for textures

//	glDisableClientState(GL_NORMAL_ARRAY);
//	glDisableClientState(GL_COLOR_ARRAY);
//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	// HACK //
	/*glMatrixMode(GL_MODELVIEW); // assume I'm in this mode!
	glPushMatrix(); // Animation, uncomment to make rotate!
		glRotatef( xRot, 1.0f, 1.0f, 0.0f );
		glRotatef( zRot, 0.0f, 0.0f, 1.0f );*/

//  Code for doing bone animations
//	_RenderCube(); return true;

	/*int // vertexstate = 0, 
		count[3] = {0, 0, pObject->nGroups};
		// count[3] = {0, 2, 3}; */
	/*{
		// glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		// glDisableClientState(GL_TEXTURE_COORD_ARRAY); // new enabled for textures

		GLfloat uvs[] = { 0.0f,  0.0f,   1.0f,  0.0f,   1.0f,  1.0f,   0.0f,  1.0f,
						  1.0f,  0.0f,   1.0f,  1.0f,   0.0f,  1.0f,   0.0f,  0.0f,
						  0.0f,  1.0f,   0.0f,  0.0f,   1.0f,  0.0f,   1.0f,  1.0f,
						  1.0f,  1.0f,   0.0f,  1.0f,   0.0f,  0.0f,   1.0f,  0.0f,
						  1.0f,  0.0f,   1.0f,  1.0f,   0.0f,  1.0f,   0.0f,  0.0f,
						  0.0f,  0.0f,   1.0f,  0.0f,   1.0f,  1.0f,   0.0f,  1.0f };

		GLfloat verticies[] = {	-1.0f, -1.0f,  1.0f,   1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
							-1.0f,  1.0f,  1.0f,   -1.0f, -1.0f, -1.0f,   -1.0f,  1.0f, -1.0f,
							 1.0f,  1.0f, -1.0f,   1.0f, -1.0f, -1.0f,   -1.0f,  1.0f, -1.0f,
							-1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,
							-1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f,
							-1.0f, -1.0f,  1.0f,   1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f,
							 1.0f,  1.0f,  1.0f,   1.0f, -1.0f,  1.0f,   -1.0f, -1.0f, -1.0f,
							-1.0f, -1.0f,  1.0f,   -1.0f,  1.0f,  1.0f,   -1.0f,  1.0f, -1.0f	};

		glEnable(GL_TEXTURE_2D);					 // Enable Texture Mapping ( NEW )
		glEnableClientState(GL_TEXTURE_COORD_ARRAY); // new enabled for textures
		glTexCoordPointer( 2, GL_FLOAT, 0, uvs );
		glVertexPointer( 3, GL_FLOAT, 0, verticies );
		glDrawArrays( GL_QUADS, 0, 24);

		return true;
	}*/

	for( VisVertexGroupPtr *pGroup = pObject->vertexGroup, *pGroupEnd = pObject->vertexGroup + pObject->nGroups; pGroup < pGroupEnd /*&& count[0] < count[2]*/; pGroup++/*, count[0]++*/ )
	{
		/*if( count[0] < count[1] )
		{
			continue;
		}*/

		// Do material Initialization
		visint face = pGroup->ptr->material.ptr->face;

		// Set material
		VisVertexArrayPtr *pVertex = pGroup->ptr->material.ptr->vertexArray,
						  *pVertEnd= pGroup->ptr->material.ptr->vertexArray + 5;
		VisTexturePtr	  *pTexture = pGroup->ptr->material.ptr->textureArray;
		glColorMaterial(face, pGroup->ptr->material.ptr->mode);
		while( pVertex < pVertEnd )
		{
			// DBGPRINT((msg,"%i, %i, \n", , ,))'
			glMaterialfv( face, pVertex->ptr->vertextype, pVertex->ptr->verticies );
			pVertex++;
		}

		// Bind the texture ... this will eventually need to be done at a different stage
		// this call might be costly
		// glBindTexture(GL_TEXTURE_2D, /*opengl texture index*/);

		/*if( vertexstate != pGroup->ptr->nArrays )
		{
			vertexstate = pGroup->ptr->nArrays;
			if( vertexstate > 2 )
			{
				glEnableClientState(GL_COLOR_ARRAY);
			}
			else
				glDisableClientState(GL_COLOR_ARRAY);
		}*/

		pVertex = pGroup->ptr->vertexArray;

		// push Model Matrix
		switch(pGroup->ptr->nArrays)
		{
		// case 5: // secondary texture pointer
			// glBindTexture(GL_TEXTURE_2D, texture[0]);
			// glTexCoordPointer( 2, GL_FLOAT, 0, uvs );
		case 4:
			glEnable(GL_TEXTURE_2D);					 // Enable Texture Mapping ( NEW )
			glEnableClientState(GL_TEXTURE_COORD_ARRAY); // new enabled for textures
			(pTexture->ptr->iTexture == iCurrentTexture) || 
				BindTexture(GL_TEXTURE_2D, pTexture->ptr->iTexture) &&
					(iCurrentTexture = pTexture->ptr->iTexture);
			glTexCoordPointer( pVertex[3].ptr->vertexsize, GL_FLOAT, 0, pVertex[3].ptr->verticies );
		case 3:
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer( pVertex[2].ptr->vertexsize, GL_FLOAT, 0, pVertex[2].ptr->verticies );
		case 2:
			glNormalPointer( GL_FLOAT, 0, pVertex[1].ptr->verticies );
		case 1:
			glVertexPointer( pVertex[0].ptr->vertexsize, GL_FLOAT, 0, pVertex[0].ptr->verticies );
			glDrawArrays( pGroup->ptr->primType, 0, pVertex[0].ptr->nVerticies );
		default:
			break;
		}

		glDisableClientState(GL_TEXTURE_COORD_ARRAY); // new enabled for textures
		glDisable(GL_TEXTURE_2D);					  // Enable Texture Mapping ( NEW )
	}

	// HACK //
	/*glMatrixMode(GL_MODELVIEW);
	glPopMatrix();*/

	return true;
}
