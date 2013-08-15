#pragma once

#include <windows.h>

#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The GLaux Library

//////////////////////////////////////////////////////////////////////////
//	Types Definitions
//	This file defines all game object types, each type should be binary 
//	compatible:
//////////////////////////////////////////////////////////////////////////

typedef unsigned __int8		visidtype;
typedef unsigned __int32	hashkey;
typedef void *				commonpointertype; // this is just place holder!

typedef int					visint;
typedef unsigned int		visuint;
typedef long				vislong;
typedef unsigned long		visulong;
typedef float				visfloat;
typedef	double				visdouble;
typedef	__int64				visfunctptr;

#define DEG2RAD				(0.017453292519943)        // degrees to radians factor
#define PI					(3.14159265358979323846)

#define SCRIPTTABLEMACRO(x) { (x), ##(x) },

// Please, always add to the end of the Enums

enum eVisionaryPassTypes
{
	VPT_INITIALIZE,
	VPT_REGISTER,
	VPT_UNREGISTER,
	VPT_RESOURCE,
	VPT_TIMER,
	VPT_SCRIPT,
	VPT_INPUT,
	VPT_SPATIALITY,
	VPT_COLLISION,
	VPT_RENDER,
	VPT_POSTPROCESSING,
	VPT_FILEPROCESSING,
	VPT_BEGINFRAME,
	VPT_ENDFRAME,
	VPT_FINALIZE,

	VPT_VisionaryPassTypesMax,
};

enum eVisionaryIdTypes
{
	VIT_INVALID = 0x00, 

	// C Standard types
	VIT_CINT,				// 0x01
	VIT_CLONG,				// 0x02
	VIT_CFLOAT,				// 0x03
	VIT_CDOUBLE,			// 0x04
	VIT_CINT64,				// 0x05
	VIT_CSTRING,			// 0x06
							
	// Game types			
	VIT_IDENT,				// 0x07
	VIT_OFFSET,				// 0x08
							
	VIT_VIEWPORT,			// 0x09
	VIT_INSTANCEARRAY,		// 0x0a
	VIT_INSTANCE,			// 0x0b
	VIT_ANIMATION,			// 0x0c
	VIT_ANIMATIONINSTANCE,	// 0x0d
	VIT_CAMERA,				// 0x0e
	VIT_LIGHT,				// 0x0f
							
	VIT_MODEL,				// 0x10
	VIT_MATERIAL,			// 0x11
	VIT_TEXTURE,			// 0x12
	VIT_TEXTUREOBJECT,		// 0x13
	VIT_MATRIX,				// 0x14
	VIT_VERTEXGROUP,		// 0x15
	VIT_VERTEXARRAY,		// 0x16
							
	VIT_SCRIPT,				// 0x17
	VIT_MAPPEDINSTANCE,		// 0x18
	VIT_INTERPTABLE,		// 0x19

	// No Real objects for these ... only alternate routines!
	VIT_CAMERAINSTANCE,		// 0x1a
	VIT_LIGHTINSTANCE,		// 0x1b
	// No Real objects for these ... only alternate routines!

	VIT_INT,
	VIT_INT64,
	VIT_FLOAT,
	VIT_DOUBLE,
	VIT_STRING,

	// Internal types
	VIT_KEYS,
	VIT_MOUSE,

	VIT_VisionaryIdTypesMax,

	VIT_FORCEMAX = 0xff,
};

struct SGlobalData
{
	HINSTANCE	hInstance;
    HINSTANCE	hPrevInstance;
    LPTSTR		lpCmdLine;
    int			nCmdShow;

	HDC			hDC;		// Private GDI Device Context
	HGLRC		hRC;		// Permanent Rendering Context
	HWND		hWnd;		// Holds Our Window Handle

	WNDPROC		WndProc;	// Holds pointer to pump routine

	LARGE_INTEGER timerFrequency;
	LARGE_INTEGER startTime;
	double		  deltaTime;
};

struct SCommonObjectIdentifier
{
	hashkey		number;	// can cast this to a pointer if first in array!
	visidtype	type;
};

struct SCommonObjectData
{
	unsigned long					size;
	struct SCommonObjectIdentifier	id;
};

struct SCommonObjectHeader
{
	struct SCommonObjectData data;
	struct SCommonObjectIdentifier id;	// Store the unpatched ID here to maintain
										// data integrety
};

/////////////////////////////////////////////////////////////////////////
// Struct predeclares

struct VisGeneric;
struct VisCamera;
struct VisMatrix;
struct VisInstance;
struct VisLight;
struct VisModel;
struct VisVertexGroup;
struct VisMaterial;
//struct VisTextureObject;
struct VisTexture;
struct VisVertexArray;
struct VisAnimation;
struct VisAnimationInstance;

/////////////////////////////////////////////////////////////////////////
// Struct definitions

struct VisGenericPtr
{
	VisGeneric *ptr;
	visidtype	type;
};

struct VisGeneric
{
	SCommonObjectHeader header;
};

struct VisScript
{
	SCommonObjectHeader header;
	char *File;
};

struct VisCameraPtr
{
	VisCamera		*ptr;
	visidtype		type;
};

struct VisCamera
{
	SCommonObjectHeader header;

	visint	 iType; // camera type ortho or perspective

	visfloat fAspect;
	visfloat fNear;
	visfloat fFar;

	visfloat fCustom; // fov(per) or range(ortho)
};

struct VisVertexArrayPtr
{
	VisVertexArray *ptr;
	visidtype		type;
};

#pragma warning (push)
#pragma warning (disable:4200)

struct VisVertexArray
{
	SCommonObjectHeader header;

	visint		vertextype; // GL_NORMAL_ARRAY, GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_INDEX_ARRAY
	visint		vertexsize; // 2,3,4 ... coordinates in vertex
	visint		nVerticies; // doubles as GL_FONT/GL_BACK/GL_FRONT_AND_BACK Designation for VisMaterial
	visfloat	verticies[];
};
#pragma warning (pop)

struct VisTexturePtr
{
	VisTexture *ptr;
	visidtype	type;
};

struct VisTexture
{
	SCommonObjectHeader header;

	visuint iTexture; // opengl texture
	visint width;
	visint height;
	visint xrepeat;
	visint yrepeat;
	visint size; // RGB (3),RGBA(4) .. etc 
	visint depth; // GL_RGB, GL_RGBA ... etc
	BYTE *bytes;
};

/*struct VisTextureObjectPtr
{
	VisTextureObject *ptr;
	visidtype		type;
};*/

// This object should hold the information necessary to 
// define mip maps for textures/ effects(?)
/*struct VisTextureObject
{
	SCommonObjectHeader header;

	// glTexImage2D/glTexSubImage2D

	// target is from textureReference (GL_TEXTURE_2D/GL_PROXIE_TEXTURE_2D)
	// level is mipmap level

	bool			isSubimage;
	visint			internalFormal; // if 0 use texture 
	visint			width;			// if 0 use texture
	visint			height;			// if 0 use texture
	bool			border;			// 0 no border, 1 border
	
	// format use texture internalFormat
	// type use texture pixeltype
	// texels use from texture bits	

	VisVertexArrayPtr	textureEnvironment[1];
	VisTexturePtr		textureReference;		// pointer to texture	
};*/

struct VisMaterialPtr
{
	VisMaterial *ptr;
	visidtype	 type;
};

struct VisMaterial
{
	SCommonObjectHeader header;

	visint				face;			// glColorMaterial
	visint				mode;			// glColorMaterial
	VisVertexArrayPtr	vertexArray[5]; // GL_SPECULAR, GL_AMBIENT, GL_DIFFUSE, GL_EMISSION, GL_SHININESS

	visint				nTextures;
	VisTexturePtr		*textureArray;
};

struct VisVertexGroupPtr
{
	VisVertexGroup *ptr;
	visidtype		type;
};

#pragma warning (push)
#pragma warning (disable:4200)

struct VisVertexGroup
{
	// Group of verticies must be at least a GL_*_ARRAY
	SCommonObjectHeader header;

	VisMaterialPtr		material;

	visint				primType;
	visint				nArrays;	
	VisVertexArrayPtr	vertexArray[];
};
#pragma warning (pop)

struct VisModelPtr
{
	VisModel *ptr;		// maps the same as the Id
	visidtype	type;
};

struct VisModel
{
	static struct SSubType
	{
		visidtype	subtype;
	};

  	SCommonObjectHeader header;
	SSubType			subtype;
	
	VisVertexGroupPtr	*vertexGroup;
	visint				nGroups;
};

struct VisMatrixPtr
{
	VisMatrix *ptr;
	visidtype	type;
};

struct VisMatrix
{
	SCommonObjectHeader	header;

	visint		matrixMode; // GL_MODELVIEW, GL_VIEWPORT, GL_PROJECTION, GL_COLOR
	visfloat	matrix[16];
};

struct VisVector
{
	SCommonObjectHeader header;

	visfloat	x;
	visfloat	y;
	visfloat	z;
};

struct VisInstancePtr
{
	VisInstance *ptr;		// maps the same as the Id
	visidtype	type;
};

struct VisInstance
{
	SCommonObjectHeader	header;
	VisInstancePtr		child;
	VisInstancePtr		sibling;

	VisMatrixPtr		*pMatricies;
	visint				nMatricies;

	visint				flags;
	VisGenericPtr		object;
};

struct VisLightPtr
{
	VisLight		*ptr;
	visidtype		type;
};

struct VisLight
{
	SCommonObjectHeader header;

	vislong lightid;

	VisVertexArrayPtr	*pVerticies;
	vislong				nVerticies;
};

struct VisKeyFrame
{
	vislong				time;		// time keyframe is valid
	VisVertexArrayPtr	*pTangent;	// one tangent for each key frame (creates direction)
	VisMatrixPtr		*pMatrix;	// each matrix is a key frame
};

struct VisAnimationPtr
{
	VisAnimation	*ptr;
	visidtype		type;
};

struct VisAnimation
{
	SCommonObjectHeader header;

	vislong				animationType;	// None/Interp/Spline

	VisKeyFrame			*pKeys;
	vislong				nKeys;
};

struct VisAnimationInstance
{
	SCommonObjectHeader header;

	visint				looping;
	vislong				timeElapsed;
	vislong				currentKey;

	VisAnimationPtr		animation;
};

////////////////////////////////////////////////////
// Non Object Types

struct SBtree {
	SBtree *left, *right;

	int type;
	VisGenericPtr * object;
};

struct SSearchObject {
	int type; // linear, btree, hash, etc ...
	void * searchobj;
	int nObjects;
	VisGenericPtr * objects;
};

struct SSection {
	int type; // Hash for one of several types
			  // shared, render, collision, etc (hash)

	int size;
	void* objects;
};

struct SFileHeader {
	visfloat		nVersion;
	int				iSections; // number of sections
	SSection		*pSections;
};

struct SKeyData
{	
	unsigned int value:32,
		         toggle:2; // 0 dn, 1 up, 2 toggle. (00 dn, off) (11 up,on)
	struct SState
	{
	unsigned int transition:1,
				 previous:1,
				 context:1, // alt pressed?
				 reserved:3,
				 extended:1, // 101 keyboard?
				 scancode:5,
				 repeat:15;
	} state;
};

struct SKeyInfo
{
	SCommonObjectHeader header;

	SKeyData keys[VK_OEM_CLEAR+1][2];
};

struct SMouseInfo
{
	SCommonObjectHeader header;

	unsigned int 
		button,
		click:2, // up 0, down 1, 2 dbl, 3 scroll
		ctrl:1, shift:1, lbtn:1, rbtn:1, mbtn:1, xbtn1:1, xbtn2:1;
	int zDelta;				// for scroll wheel!
		POINTS position;	// current position
		POINTS prevPosition;	// prev position
};


/////////////////////////////////////////////////////////////
//
// In the global scheme of things, how will this object type:
//   A.  Be itentified.
//   B.  Be the target of other operations
//   C.  Located on the Search tree (when one is created)
//	 D.  How do I use this object from an external module?
//   E.  Why is this object using a typedef when non of the
//       other structs do? 
//
/////////////////////////////////////////////////////////////

typedef struct SCamera
{
	visfloat rotX;					// Degrees
	visfloat rotY;
	visfloat dist;

	VisVector forwardVec;
	VisVector lookatVec;
	VisVector upVec;
	VisVector posVec;				// Position of target object in the world (kept separate to minimize calcs)

	VisMatrix camMat;

	bool	dirtyLookat;
	bool	dirtyPos;

} SCamera;
