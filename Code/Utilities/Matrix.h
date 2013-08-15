#ifndef VISIONARY_MATRIX_H
#define VISIONARY_MATRIX_H

#pragma once

#include <TypeDefinitions.h>


void Matrix_Clear(VisMatrix *pMatrix);
void Matrix_SetIdentity(VisMatrix *pMatrix);
void Matrix_Multiply(VisMatrix *pResult, VisMatrix *pFirst, VisMatrix *pSecond);

void Matrix_Translate(VisMatrix *pResult, visfloat x, visfloat y, visfloat z);
void Matrix_Scale(VisMatrix *pResult, visfloat x, visfloat y, visfloat z);
void Matrix_RotateX(VisMatrix *pResult, visfloat radx);
void Matrix_RotateY(VisMatrix *pResult, visfloat rady);
void Matrix_RotateZ(VisMatrix *pResult, visfloat radz);
void Matrix_Rotate(VisMatrix *pResult, visfloat radx, visfloat rady, visfloat radz);
void Matrix_RotateXZ(VisMatrix *pResult, visfloat radx, visfloat radz);


#endif