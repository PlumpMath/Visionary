#ifndef VISIONARY_CAMERA_H
#define VISIONARY_CAMERA_H

#pragma once

#include <TypeDefinitions.h>
#include <Vector.h>
#include <Matrix.h>


void Camera_Reset(SCamera *pCamera);

void Camera_SetLookAt(SCamera *pCamera, visfloat x, visfloat y, visfloat z);
void Camera_SetUp(SCamera *pCamera, visfloat x, visfloat y, visfloat z);
void Camera_SetDist(SCamera *pCamera, visfloat val);
void Camera_SetPosition(SCamera *pCamera, visfloat x, visfloat y, visfloat z);
void Camera_Translate(SCamera *pCamera, visfloat x, visfloat y, visfloat z);
void Camera_Distance(SCamera *pCamera, visfloat val);

void Camera_SetRotation(SCamera *pCamera, visfloat degx, visfloat degy);
void Camera_Rotate(SCamera *pCamera, visfloat degx, visfloat degy);

void Camera_UpdatePos(SCamera *pCamera);
void Camera_UpdateMatrix(SCamera *pCamera);

VisMatrix* Camera_GetMatrix(SCamera *pCamera);
void Camera_GetForwardVec(VisVector *pForward, SCamera *pCamera);

#endif