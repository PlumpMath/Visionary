
#pragma once

#ifndef VISIONARY_STACK_H
#define VISIONARY_STACK_H

#include <TypeDefinitions.h>


struct SStack
{
	visint count;
	SStack *next;
};

// Buffer Routines
SStack* SStack_InitializeBuffer( SStack *stackbuffer, int count, unsigned long size );
SStack* SStack_GetFree( SStack *&freelist );
void SStack_SetFree( SStack *&freelist, SStack* used );

// Stack Routines
int SStack_Count( SStack *head );

bool SStack_Empty( SStack *head );
bool SStack_Push( SStack *&head, SStack* item );

SStack* SStack_Top( SStack *head );
SStack* SStack_Pop( SStack *&head );

#endif // def VISIONARY_STACK_H