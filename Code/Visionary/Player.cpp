#include "stdafx.h"
#include "Player.h"
#include <Vector.h>
#include <Matrix.h>
#include <Camera.h>


//*************************************************************************
//
// For the manipulation of a player object
//
//*************************************************************************

////////////////////////////////////////////////////////////////////////
//
// Translate the given player object forward
//
////////////////////////////////////////////////////////////////////////

void Player_Forward(VisInstancePtr *pPlayer, SCamera *pCamera, double dist)
{
	// check type??? other valid ptrs????

	VisVector fVec;
	VisMatrix *origMat	= pPlayer->ptr->pMatricies->ptr;

	// This will move the player appropriately but we will need to still reorient the player and reset collision
	Camera_GetForwardVec(&fVec, pCamera);
	Vector_Multiply(&fVec, -dist);

	// constrain motion if need be...
	fVec.z = 0.0;

	// Adjust the translation of the player object
	origMat->matrix[12] -= fVec.x;
	origMat->matrix[13] -= fVec.y;
	origMat->matrix[14] -= fVec.z;

	// Translate the camera to match
	Camera_Translate(pCamera, fVec.x, fVec.y, fVec.z);
}


////////////////////////////////////////////////////////////////////////
//
// Translate the given player object sideways
//
////////////////////////////////////////////////////////////////////////

void Player_Strafe(VisInstancePtr *pPlayer, SCamera *pCamera, double dist)
{
	VisVector fVec, newVec;
	VisMatrix *origMat	= pPlayer->ptr->pMatricies->ptr;

	// This will move the player appropriately but we will need to still reorient the player and reset collision
	Camera_GetForwardVec(&fVec, pCamera);

	// Rotate the forward vector to the side...  can make faster...
	VisMatrix p;
	Matrix_RotateZ(&p, visfloat(-90.0 * DEG2RAD));
	Vector_MultMat(&newVec, &fVec, &p);

	Vector_Multiply(&newVec, -dist);

	// constrain motion if need be...
	newVec.z = 0.0;

	// Adjust the translation of the player object
	origMat->matrix[12] -= newVec.x;
	origMat->matrix[13] -= newVec.y;
	origMat->matrix[14] -= newVec.z;

	// Translate the camera to match
	Camera_Translate(pCamera, newVec.x, newVec.y, newVec.z);
}