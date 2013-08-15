#ifndef VISIONARY_CVector_H
#define VISIONARY_CVector_H

#pragma once

#include <TypeDefinitions.h>
#include <math.h>
#include <Matrix.h>



void Vector_Set(VisVector *pResult, visfloat x, visfloat y, visfloat z);

void Vector_AddV(VisVector *pResult, VisVector *pFirst, VisVector *pSecond);
void Vector_Add(VisVector *pResult, visfloat x, visfloat y, visfloat z);

void Vector_SubtractV(VisVector *pResult, VisVector *pFirst, VisVector *pSecond);
void Vector_Subtract(VisVector *pResult, visfloat x, visfloat y, visfloat z);

void Vector_DivideV(VisVector *pResult, VisVector *pFirst, VisVector *pSecond);
void Vector_Divide(VisVector *pResult, visfloat x, visfloat y, visfloat z);

visfloat Vector_MultiplyV(VisVector *pFirst, VisVector *pSecond);
void Vector_Multiply(VisVector *pResult, visfloat multiplier);

void Vector_Cross(VisVector *pResult, VisVector *pFirst, VisVector *pSecond);
void Vector_Normalize(VisVector *pResult, VisVector* pVector);
visfloat Vector_Distance(VisVector *pVector);
bool Vector_Equal(VisVector *pFirst, VisVector *pSecond);

void Vector_MultMat(VisVector *pResult, VisVector *pVec, VisMatrix *pMatrix);


#endif