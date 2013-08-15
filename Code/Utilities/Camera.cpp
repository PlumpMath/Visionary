#include "TypeDefinitions.h"
#include "Camera.h"

#include <stdio.h>

#define MAXROTX		60.0
#define MAXZOOM		1000.0
#define MINZOOM		0.1


/////////////////////////////////////////////////////////////////////
//
//	Reset camera properties
//
/////////////////////////////////////////////////////////////////////

void Camera_Reset(SCamera *pCamera)
{
	pCamera->dirtyLookat	= true;
	pCamera->dirtyPos		= true;

	pCamera->rotX			= 0.0;
	pCamera->rotY			= 0.0;
	pCamera->dist			= 25.0;

	// Set vector defaults
	Vector_Set(&pCamera->lookatVec, 0.0, 0.0, 0.0);
	Vector_Set(&pCamera->upVec, 0.0, 0.0, 1.0);
	Vector_Set(&pCamera->posVec, 0.0, 0.0, 0.0);
}


/////////////////////////////////////////////////////////////////////
//
//	Update the camera look at point
//
/////////////////////////////////////////////////////////////////////

void Camera_SetLookAt(SCamera *pCamera, visfloat x, visfloat y, visfloat z)
{
	Vector_Set(&pCamera->lookatVec, x, y, z);

	// Update camera position
	pCamera->dirtyPos		= true;
}


/////////////////////////////////////////////////////////////////////
//
//	Update the camera up vector
//
/////////////////////////////////////////////////////////////////////

void Camera_SetUp(SCamera *pCamera, visfloat x, visfloat y, visfloat z)
{
	Vector_Set(&pCamera->upVec, x, y, z);
	pCamera->dirtyLookat	= true;
}


/////////////////////////////////////////////////////////////////////
//
//	Update the camera distance vector
//
/////////////////////////////////////////////////////////////////////

void Camera_SetDist(SCamera *pCamera, visfloat val)
{
	pCamera->dist			= val;

	// Update camera position
	pCamera->dirtyPos		= true;
}


/////////////////////////////////////////////////////////////////////
//
//	Update the camera position
//
/////////////////////////////////////////////////////////////////////

void Camera_SetPosition(SCamera *pCamera, visfloat x, visfloat y, visfloat z)
{
	Vector_Set(&pCamera->posVec, x, y, z);
}


/////////////////////////////////////////////////////////////////////
//
//	Update the camera position
//
/////////////////////////////////////////////////////////////////////

void Camera_Translate(SCamera *pCamera, visfloat x, visfloat y, visfloat z)
{
	Vector_Add(&pCamera->lookatVec, x, y, z);

	// Update camera position
	pCamera->dirtyPos		= true;
}


/////////////////////////////////////////////////////////////////////
//
//	Update the camera distance
//
/////////////////////////////////////////////////////////////////////

void Camera_Distance(SCamera *pCamera, visfloat val)
{
	pCamera->dist	+= val;
	if (pCamera->dist < MINZOOM)		pCamera->dist = (visfloat)MINZOOM;
	if (pCamera->dist > MAXZOOM)		pCamera->dist = (visfloat)MAXZOOM;

	// Update camera position
	pCamera->dirtyPos		= true;
}


/////////////////////////////////////////////////////////////////////
//
//	Update the camera rotation (fixed)
//
/////////////////////////////////////////////////////////////////////

void Camera_SetRotation(SCamera *pCamera, visfloat degx, visfloat degy)
{
	pCamera->rotX			= degx;
	pCamera->rotY			= degy;
	
	// constrain values.. there must be better way
	if (pCamera->rotX > MAXROTX)	pCamera->rotX -= MAXROTX;		// special case for cutscenes??
	if (pCamera->rotX < -MAXROTX)	pCamera->rotX += MAXROTX;		// don't want to expose values...
	if (pCamera->rotY > 360.0)		pCamera->rotY -= 360.0;
	if (pCamera->rotY < -360.0)		pCamera->rotY += 360.0;

	// Calculate the new camera position
	pCamera->dirtyPos		= true;
}


/////////////////////////////////////////////////////////////////////
//
//	Update the camera rotation (relative)
//
////////////////////////////////////////////////////////////////////

void Camera_Rotate(SCamera *pCamera, visfloat degx, visfloat degy)
{
	pCamera->rotX			+= degx;
	pCamera->rotY			+= degy;

	// constrain values.. there must be better way
	if (pCamera->rotX > MAXROTX)		pCamera->rotX = MAXROTX;		// special case for cutscenes??
	if (pCamera->rotX < -MAXROTX)		pCamera->rotX = -MAXROTX;		// don't want to expose values
	if (pCamera->rotY > 360.0)			pCamera->rotY -= 360.0;
	if (pCamera->rotY < -360.0)			pCamera->rotY += 360.0;

	// Calculate the new camera position
	pCamera->dirtyPos		= true;
}


/////////////////////////////////////////////////////////////////////
//
//	Update the camera position
//
/////////////////////////////////////////////////////////////////////

void Camera_UpdatePos(SCamera *pCamera)
{
	VisMatrix rotMat;
	VisVector d;

	// Calculate the new camera position
	Matrix_RotateXZ(&rotMat, pCamera->rotX * (visfloat)DEG2RAD, pCamera->rotY * (visfloat)DEG2RAD);
	Vector_Set(&d, 0.0, pCamera->dist, 0.0);
	Vector_MultMat(&pCamera->posVec, &d, &rotMat);
	Vector_Add(&pCamera->posVec, pCamera->lookatVec.x, pCamera->lookatVec.y, pCamera->lookatVec.z);

	// Set dirty flag for next update
	pCamera->dirtyPos			= false;
	pCamera->dirtyLookat		= true;		// Force recalc of camera matrix
}


/////////////////////////////////////////////////////////////////////
//
//	Update the camera matrix
//
//	Users are left to call this once all the vectors have been set
//	to minimize matrix calculations as much as possible.
//
/////////////////////////////////////////////////////////////////////

void Camera_UpdateMatrix(SCamera *pCamera)
{
	VisVector n, s, u, distVec;
	VisMatrix trans;
	VisMatrix *pMat	= &pCamera->camMat;

	// Update camera position
	if (pCamera->dirtyPos)				Camera_UpdatePos(pCamera);

	// Update camera matrix
	if (pCamera->dirtyLookat)
	{
		Vector_SubtractV(&distVec, &pCamera->lookatVec, &pCamera->posVec);
		Vector_Normalize(&pCamera->forwardVec, &distVec);
		Vector_Normalize(&n, &pCamera->upVec);

		Vector_Cross(&s, &pCamera->forwardVec, &n);
		Vector_Cross(&u, &s, &pCamera->forwardVec);

		// Set the lookat matrix
		Matrix_SetIdentity(pMat);				// OpenGL matrix is column-major
		pMat->matrix[0]	= s.x;
		pMat->matrix[4]	= s.y;
		pMat->matrix[8]	= s.z;

		pMat->matrix[1]	= u.x;
		pMat->matrix[5]	= u.y;
		pMat->matrix[9]	= u.z;

		pMat->matrix[2]	= -pCamera->forwardVec.x;
		pMat->matrix[6]	= -pCamera->forwardVec.y;
		pMat->matrix[10]	= -pCamera->forwardVec.z;

		// Translate opposite the distance vector
		Matrix_Translate(&trans, pCamera->lookatVec.x + distVec.x, pCamera->lookatVec.y + distVec.y, pCamera->lookatVec.z + distVec.z);
		Matrix_Multiply(pMat, &trans, pMat);

		// Reset dirty flag
		pCamera->dirtyLookat	= false;
	}
}



/////////////////////////////////////////////////////////////////////
//
//	Retrieve the camera matrix
//
//	Use this matrix to set the world.  Set here to minimize
//	calculations.  
//
/////////////////////////////////////////////////////////////////////

VisMatrix* Camera_GetMatrix(SCamera *pCamera)
{
	return &(pCamera->camMat);
}


/////////////////////////////////////////////////////////////////////
//
//	Retrieve the forward vector
//
/////////////////////////////////////////////////////////////////////

void Camera_GetForwardVec(VisVector *pForward, SCamera *pCamera)
{
	memcpy(pForward, &pCamera->forwardVec, sizeof(VisVector));
}

