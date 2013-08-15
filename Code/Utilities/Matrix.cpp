
//*************************************************************************
//
//	For the storage and manipulation of 4x4 matrices
//	Functions assume that all pointers passed as arguments are valid to
//		minimize checks.
//
//*************************************************************************

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "Matrix.h"



////////////////////////////////////////////////////////////////////////
//
// Clear the given matrix (set all members to 0)
//
////////////////////////////////////////////////////////////////////////

void Matrix_Clear(VisMatrix *pMatrix)
{
	memset(pMatrix, 0, sizeof(VisMatrix));
}


////////////////////////////////////////////////////////////////////////
//
// Set the given matrix to the identity matrix
//
////////////////////////////////////////////////////////////////////////

void Matrix_SetIdentity(VisMatrix *pMatrix)
{
	Matrix_Clear(pMatrix);
	pMatrix->matrix[0] = pMatrix->matrix[5] = pMatrix->matrix[10] = pMatrix->matrix[15] = 1.0;
}


////////////////////////////////////////////////////////////////////////
//
// Multiply the given matrices together (first * second = result)
//
//	NOTE: Don't repeat result matrix as its being calculated
//
////////////////////////////////////////////////////////////////////////

void Matrix_Multiply(VisMatrix *pResult, VisMatrix *pFirst, VisMatrix *pSecond)
{	
	for (int i = 0; i < 16; i += 4)
	{ 
		pResult->matrix[i]		=	pFirst->matrix[i] * pSecond->matrix[0] + 
									pFirst->matrix[i + 1] * pSecond->matrix[4] +
									pFirst->matrix[i + 2] * pSecond->matrix[8] + 
									pFirst->matrix[i + 3] * pSecond->matrix[12];
		pResult->matrix[i + 1]	=	pFirst->matrix[i] * pSecond->matrix[1] + 
									pFirst->matrix[i + 1] * pSecond->matrix[5] +
									pFirst->matrix[i + 2] * pSecond->matrix[9] + 
									pFirst->matrix[i + 3] * pSecond->matrix[13];
		pResult->matrix[i + 2]	=	pFirst->matrix[i] * pSecond->matrix[2] + 
									pFirst->matrix[i + 1] * pSecond->matrix[6] +
									pFirst->matrix[i + 2] * pSecond->matrix[10] + 
									pFirst->matrix[i + 3] * pSecond->matrix[14];
		pResult->matrix[i + 3]	=	pFirst->matrix[i] * pSecond->matrix[3] + 
									pFirst->matrix[i + 1] * pSecond->matrix[7] +
									pFirst->matrix[i + 2] * pSecond->matrix[11] + 
									pFirst->matrix[i + 3] * pSecond->matrix[15];
	}
}


////////////////////////////////////////////////////////////////////////
//
// Create a translation matrix with the given vector values
//
////////////////////////////////////////////////////////////////////////

void Matrix_Translate(VisMatrix *pResult, visfloat x, visfloat y, visfloat z)
{
	Matrix_SetIdentity(pResult);
	pResult->matrix[12]	= x;
	pResult->matrix[13]	= y;
	pResult->matrix[14]	= z;
}


////////////////////////////////////////////////////////////////////////
//
// Create a scale matrix with the given values
//
////////////////////////////////////////////////////////////////////////

void Matrix_Scale(VisMatrix *pResult, visfloat x, visfloat y, visfloat z)
{
	Matrix_SetIdentity(pResult);
	pResult->matrix[0]	= x;
	pResult->matrix[5]	= y;
	pResult->matrix[10]	= z;
}


////////////////////////////////////////////////////////////////////////
//
// Create a rotation matrix with the given value
//
////////////////////////////////////////////////////////////////////////

void Matrix_RotateX(VisMatrix *pResult, visfloat radx)
{	
	visfloat c	= (visfloat)cos(radx);
	visfloat s	= (visfloat)sin(radx);

	Matrix_SetIdentity(pResult);
	pResult->matrix[5]	= c;
	pResult->matrix[6]	= s;
	pResult->matrix[9]	= -s;
	pResult->matrix[10]	= c;
}


////////////////////////////////////////////////////////////////////////
//
// Create a rotation matrix with the given value
//
////////////////////////////////////////////////////////////////////////

void Matrix_RotateY(VisMatrix *pResult, visfloat rady)
{	
	visfloat c	= (visfloat)cos(rady);
	visfloat s	= (visfloat)sin(rady);

	Matrix_SetIdentity(pResult);
	pResult->matrix[0]	= c;
	pResult->matrix[2]	= -s;
	pResult->matrix[8]	= s;
	pResult->matrix[10]	= c;
}


////////////////////////////////////////////////////////////////////////
//
// Create a rotation matrix with the given value
//
////////////////////////////////////////////////////////////////////////

void Matrix_RotateZ(VisMatrix *pResult, visfloat radz)
{	
	visfloat c	= (visfloat)cos(radz);
	visfloat s	= (visfloat)sin(radz);

	Matrix_SetIdentity(pResult);
	pResult->matrix[0]	= c;
	pResult->matrix[1]	= s;
	pResult->matrix[4]	= -s;
	pResult->matrix[5]	= c;
}


////////////////////////////////////////////////////////////////////////
//
// Create a rotation matrix with the given values
//
////////////////////////////////////////////////////////////////////////

void Matrix_Rotate(VisMatrix *pResult, visfloat radx, visfloat rady, visfloat radz)
{
	visfloat cos_x = (visfloat)cos(radx);
	visfloat sin_x = (visfloat)sin(radx);
	visfloat cos_y = (visfloat)cos(rady);
	visfloat sin_y = (visfloat)sin(rady);
	visfloat cos_z = (visfloat)cos(radz);
	visfloat sin_z = (visfloat)sin(radz);

	visfloat cx_sy = cos_x * sin_y;
	visfloat sx_sy = sin_x * sin_y;

	Matrix_SetIdentity(pResult);
	pResult->matrix[0]	= (cos_y * cos_z);
	pResult->matrix[1]	= - (cos_y * sin_z);
	pResult->matrix[2]	= sin_y;
	pResult->matrix[4]	= (sx_sy * cos_z) + (cos_x * sin_z);
	pResult->matrix[5]	= - (sx_sy * sin_z) + (cos_x * cos_z);
	pResult->matrix[6]	= - (sin_x * cos_y);
	pResult->matrix[8]	= - (cx_sy * cos_z) + (sin_x * sin_z);
	pResult->matrix[9]	= (cx_sy * sin_z) + (sin_x * cos_z);
	pResult->matrix[10] = (cos_x * cos_y);
}


////////////////////////////////////////////////////////////////////////
//
// Create a rotation matrix with the given values (Special case for
//		camera)
//
////////////////////////////////////////////////////////////////////////

void Matrix_RotateXZ(VisMatrix *pResult, visfloat radx, visfloat radz)
{
	visfloat cos_x = (visfloat)cos(radx);
	visfloat sin_x = (visfloat)sin(radx);
	visfloat cos_z = (visfloat)cos(radz);
	visfloat sin_z = (visfloat)sin(radz);

	Matrix_SetIdentity(pResult);
	pResult->matrix[0]	=  cos_z;
	pResult->matrix[1]	= -sin_z;
	pResult->matrix[4]	=  cos_x * sin_z;
	pResult->matrix[5]	=  cos_x * cos_z;
	pResult->matrix[6]	= -sin_x;
	pResult->matrix[8]	=  sin_x * sin_z;
	pResult->matrix[9]	=  sin_x * cos_z;
	pResult->matrix[10] =  cos_x;
}

