#include "Vector.h"


//*************************************************************************
//
// For the storage and manipulation of 3-dimensional coordinates
//
//*************************************************************************

////////////////////////////////////////////////////////////////////////
//
// Set a vector with the given values (just for ease of use)
//
////////////////////////////////////////////////////////////////////////

void Vector_Set(VisVector *pResult, visfloat x, visfloat y, visfloat z)
{
	pResult->x	= x;
	pResult->y	= y;
	pResult->z	= z;
}


////////////////////////////////////////////////////////////////////////
//
// Add one vector to another
//
////////////////////////////////////////////////////////////////////////

void Vector_AddV(VisVector *pResult, VisVector *pFirst, VisVector *pSecond)
{
	pResult->x	= pFirst->x + pSecond->x;
	pResult->y	= pFirst->y + pSecond->y;
	pResult->z	= pFirst->z + pSecond->z;
}


////////////////////////////////////////////////////////////////////////
//
// Add one vector to another
//
////////////////////////////////////////////////////////////////////////

void Vector_Add(VisVector *pResult, visfloat x, visfloat y, visfloat z)
{
	pResult->x += x;
	pResult->y += y;
	pResult->z += z;
}


////////////////////////////////////////////////////////////////////////
//
// Subtract one vector from another
//
////////////////////////////////////////////////////////////////////////

void Vector_SubtractV(VisVector *pResult, VisVector *pFirst, VisVector *pSecond)
{
	pResult->x	= pFirst->x - pSecond->x;
	pResult->y	= pFirst->y - pSecond->y;
	pResult->z	= pFirst->z - pSecond->z;
}


////////////////////////////////////////////////////////////////////////
//
// Subtract one vector from another
//
////////////////////////////////////////////////////////////////////////

void Vector_Subtract(VisVector *pResult, visfloat x, visfloat y, visfloat z)
{
	pResult->x	-= x;
	pResult->y	-= y;
	pResult->z	-= z;
}


////////////////////////////////////////////////////////////////////////
//
// Divide one vector from another
//
////////////////////////////////////////////////////////////////////////

void Vector_DivideV(VisVector *pResult, VisVector *pFirst, VisVector *pSecond)
{
	pResult->x = pFirst->x / pSecond->x;
	pResult->y = pFirst->y / pSecond->y;
	pResult->z = pFirst->z / pSecond->z;
}


////////////////////////////////////////////////////////////////////////
//
// Divide one vector from another
//
////////////////////////////////////////////////////////////////////////

void Vector_Divide(VisVector *pResult, visfloat x, visfloat y, visfloat z)
{
	pResult->x /= x;
	pResult->y /= y;
	pResult->z /= z;
}


////////////////////////////////////////////////////////////////////////
//
// Multiply one vector from another
//
////////////////////////////////////////////////////////////////////////

visfloat Vector_MultiplyV(VisVector *pFirst, VisVector *pSecond)
{
	return pFirst->x * pSecond->x + pFirst->y * pSecond->y + pFirst->z * pSecond->z;
}


////////////////////////////////////////////////////////////////////////
//
// Multiply one vector from another
//
////////////////////////////////////////////////////////////////////////

void Vector_Multiply(VisVector *pResult, visfloat multiplier)
{
	pResult->x *= multiplier;
	pResult->y *= multiplier;
	pResult->z *= multiplier;
}


////////////////////////////////////////////////////////////////////////
//
// Handle the cross product of two vectors
//
////////////////////////////////////////////////////////////////////////

void Vector_Cross(VisVector *pResult, VisVector *pFirst, VisVector *pSecond)
{
	pResult->x	= pFirst->y * pSecond->z - pFirst->z * pSecond->y;
	pResult->y	= pFirst->z * pSecond->x - pFirst->x * pSecond->z;
	pResult->z	= pFirst->x * pSecond->y - pFirst->y * pSecond->x;
}


////////////////////////////////////////////////////////////////////////
//
// Normalize the given vector
//
////////////////////////////////////////////////////////////////////////

void Vector_Normalize(VisVector *pResult, VisVector *pVector)
{
	visfloat d = (visfloat)sqrt(pVector->x * pVector->x + 
							pVector->y * pVector->y + 
							pVector->z * pVector->z);

		// Avoid error from division by 0.
	if (d == 0.0)		return;

	d = 1 / d;
	pResult->x = d * pVector->x;
	pResult->y = d * pVector->y;
	pResult->z = d * pVector->z;
}


////////////////////////////////////////////////////////////////////////
//
// Calculate the distance of the given vector
//
////////////////////////////////////////////////////////////////////////

visfloat Vector_Distance(VisVector *pVector)
{
	return (visfloat)sqrt(pVector->x * pVector->x + pVector->y * pVector->y + pVector->z * pVector->z);
}


////////////////////////////////////////////////////////////////////////
//
// Determine if the given vectors are equal
//
////////////////////////////////////////////////////////////////////////

bool Vector_Equal(VisVector *pFirst, VisVector *pSecond)
{
	return ((pFirst->x == pSecond->x) && (pFirst->y == pSecond->y) && (pFirst->z == pSecond->z));
}


/*
////////////////////////////////////////////////////////////////////////
//
// Rotation of a CVector value about the x-axis by a given amount.
// The rotation is given in degrees and then adjusted to radians for
// calculations.
//
////////////////////////////////////////////////////////////////////////

void CVector::rotx(int deg) 
{
	rotx((double)(deg * DEG2RAD));
}


////////////////////////////////////////////////////////////////////////
//
// Rotation of a CVector value about the x-axis by a given amount.
//
////////////////////////////////////////////////////////////////////////

void CVector::rotx(double rads) 
{
	double c, s;
	c = double(cos(rads));
	s = double(sin(rads));

	double tempy = y * c - z * s;
	double tempz = y * s + z * c;

	y = tempy;
	z = tempz;
}


////////////////////////////////////////////////////////////////////////
//
// Rotation of a CVector value about the y-axis by a given amount.
// The rotation is given in degrees and then adjusted to radians for
// calculations.
//
////////////////////////////////////////////////////////////////////////

void CVector::roty(int deg)
{
	roty((double)(deg * DEG2RAD));
}


////////////////////////////////////////////////////////////////////////
//
// Rotation of a CVector value about the y-axis by a given amount.

//
////////////////////////////////////////////////////////////////////////

void CVector::roty(double rads)
{
	double c, s;
	c = double(cos(rads));
	s = double(sin(rads));

	double tempx = x * c - z * s;
	double tempz = x * s + z * c;

	x = tempx;
	z = tempz;
}


////////////////////////////////////////////////////////////////////////
//
// Rotation of a CVector value about the z-axis by a given amount.
// The rotation is given in degrees and then adjusted to radians for
// calculations.
//
////////////////////////////////////////////////////////////////////////

void CVector::rotz(int deg)
{
	rotz((double)(deg * DEG2RAD));
}


////////////////////////////////////////////////////////////////////////
//
// Rotation of a CVector value about the z-axis by a given amount.
//
////////////////////////////////////////////////////////////////////////

void CVector::rotz(double rads)
{
	double c, s;
	c = double(cos(rads));
	s = double(sin(rads));

	double tempx = x * c - y * s;
	double tempy = x * s + y * c;
	
	x = tempx;
	y = tempy; 
}



////////////////////////////////////////////////////////////////////////
//
// Rotation of a CVector value about the x-axis by a given amount.
//
////////////////////////////////////////////////////////////////////////

void Vector_RotateX(SVector *pResult, double radx) 
{
	double c, s;
	c = double(cos(radx));
	s = double(sin(radx));

	pResult->x = 0.0;
	pResult->y = y * c - z * s;
	pResult->z = y * s + z * c;
}


////////////////////////////////////////////////////////////////////////
//
// Rotation of a CVector value about the y-axis by a given amount.
//
////////////////////////////////////////////////////////////////////////

void Vector_RotateY(SVector *pResult, double radx) 
{
	double c, s;
	c = double(cos(radx));
	s = double(sin(radx));

	pResult->x = x * c - z * s;
	pResult->y = 0.0;
	pResult->z = x * s + z * c;
}
*/


////////////////////////////////////////////////////////////////////////
//
// Multiplication of a matrix by a CVector
//
////////////////////////////////////////////////////////////////////////

void Vector_MultMat(VisVector *pResult, VisVector *pVec, VisMatrix *pMatrix)
{
	pResult->x = pVec->x * pMatrix->matrix[0] + pVec->y * pMatrix->matrix[4] + pVec->z * pMatrix->matrix[8] + pMatrix->matrix[12];
	pResult->y = pVec->x * pMatrix->matrix[1] + pVec->y * pMatrix->matrix[5] + pVec->z * pMatrix->matrix[9] + pMatrix->matrix[13];
	pResult->z = pVec->x * pMatrix->matrix[2] + pVec->y * pMatrix->matrix[6] + pVec->z * pMatrix->matrix[10] + pMatrix->matrix[14];
//	pResult->x = pVec->x * pMatrix->matrix[0] + pVec->y * pMatrix->matrix[1] + pVec->z * pMatrix->matrix[2] + pMatrix->matrix[3];
//	pResult->y = pVec->x * pMatrix->matrix[4] + pVec->y * pMatrix->matrix[5] + pVec->z * pMatrix->matrix[6] + pMatrix->matrix[7];
//	pResult->z = pVec->x * pMatrix->matrix[8] + pVec->y * pMatrix->matrix[9] + pVec->z * pMatrix->matrix[10] + pMatrix->matrix[11];
}

