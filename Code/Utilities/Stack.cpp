
#include "stack.h"

SStack* SStack_InitializeBuffer( SStack *stackbuffer, int count, unsigned long size )
{ 
	SStack *pEnd = stackbuffer, *pPrev = NULL, *pBeg = NULL;
	for( pBeg = (SStack*)((unsigned long)stackbuffer+(size*(count-1))); pBeg>=pEnd;pBeg = (SStack*)((unsigned long)pBeg - size))
	{	
		ZeroMemory( pBeg, size );

		pBeg->count = 0; 
		pBeg->next = pPrev;
		pPrev = pBeg;
	}

	return pEnd;
}

SStack* SStack_GetFree( SStack *&freelist )
{  
	SStack *free = freelist;
	if( freelist) 
	{ 
		freelist = freelist->next; 
	} 
	return free; 
}

void SStack_SetFree( SStack *&freelist, SStack* used )
{ 
	if( used )
	{
		used->count = 0; 
		used->next = freelist; 
		freelist = used; 
	}
}

int SStack_Count( SStack *head ) 
{ 
	if ( head ) 
		return 0; 
	return head->count; 
}

SStack* SStack_Top( SStack *head )
{ 
	return head; 
}

bool SStack_Empty( SStack *head )
{ 
	return head == NULL; 
}

bool SStack_Push( SStack *&head, SStack* item ) 
{ 
	if ( item )
	{
		item->count = (!SStack_Empty(head)) ? head->count+1: 1; 
		item->next = head;
		head = item;

		return true;
	}
	return false;
}

SStack* SStack_Pop( SStack *&head ) 
{ 
	SStack* item = head;
	if( head )
	{
		// don't update count it should be correct
		head = item->next;
	}

	return item;
}
