// EventSystem.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <stdio.h>
#include <assert.h>

// Purpose:
//	Create a dynamic event system
// Design:
//  The event system will need two objects:
//		Event Object
//		Event Handler
//	The Event Object is the event that stores all of the data relevant
//	to a single event.  
//	The event Handler is an object that can be used by other objects.
//  An event Handler is registered with an Event which will then cause
//  an interaction during the events update phase.  Once an event handler
//	is triggered it will be added to the Event Update List.
//  Events must be able to have thier own event handlers

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


////////////////////////////////////////////////////////////////
// 

struct SLList
{
	SLList *next;
};

int SLList_GetFree( SLList *&head, SLList &free )
{
	if( head == NULL )
	{
		return -1; // Empty
	}

	free = *head;
	head = head->next;

	return 0;
}

int SLList_PutFree( SLList *&head, SLList *used )
{
	if( used == NULL )
	{
		return -1;
	}

	if( head != NULL )
	{
		used->next = head;
	}

	head = used;

	return 0;
}

////////////////////////////////////////////////////////////////
// BSTree

enum eMode 
{
	// Eventually this will handle how the search is done
	E_BINARY,
	E_MERGE,
};

struct SBtree
{
	SBtree * left;
	SBtree * right;
	// define your own struct with data object attached
	// cast to this type and then use the searching routines
};

// return 1 greater, 0 for equal, -1 for lessthan
typedef int (*comparecbf)( SBtree *key, SBtree *leaf );

/////////////////////////////////////////////////////////////////
// This routine always adds to the bottom of the branch

int SBtree_AddLeaf( SBtree *head, SBtree *leaf, comparecbf comp )
{
	assert( head && "You cannot pass null in as a valid head node!" );

	int failure = -1;
	SBtree *curr = head;
	while( curr != NULL )
	{
		int test = comp( leaf, curr );
		if( test == 0 )
		{		
			failure = 1; // duplicate
			break;
		}

		// Save off the next pointer as a reference
		// will cut down on duplicated code because it
		// is possible to effect pointer even if next is null
		SBtree *&next = (test < 0) ? curr->left : curr->right ;

		if( next == NULL ) 
		{	
			failure = 0;
			next = leaf;
			break;
		}
		
		curr = next;
	}

	return failure;
}

//////////////////////////////////////////////////////////////////
// This routine only removes leafs, the user will need to add
// connected leafs back on

int SBtree_SubLeaf( SBtree *head, SBtree *&leaf, comparecbf comp )
{
	int failure = -1;
	SBtree *curr = head;
	while( curr != NULL )
	{
		switch (comp( leaf, curr ))
		{
		case 1:
			curr = curr->right;
			break;
		case 0:	 // remove node
			failure = 0;
			leaf = curr;
			goto RETURN;
		case -1:
			curr = curr->left;
			break;
		}
	}

RETURN:

	return failure;
}

int SBtree_FindLeaf( SBtree *head, SBtree *key, int eMode, comparecbf comp )
{
	return 0;
}

//

enum eEvents
{
	E_DEFAULT,
	E_KEYBOARD,
	E_MOUSE,
	E_TRIGGER,

	eEvents_MAX = 0xff,
};

typedef int (*eventtriggercbf)( int id, void* data );
typedef int (*eventunregistercbf)( int id, int obj );
typedef int (*eventregistercbf)( int obj );

struct SEventHash
{
	int   nEvent;
	char* cEvent;
	SBtree *head;
	eventtriggercbf		triggercbf;
	eventregistercbf	registercbf;
	eventunregistercbf	unregistercbf;
	comparecbf			eventcomparecbf;
};

struct SEventData {
	int objid;
	int bfilter;
};

struct SEventBtree
{
	SBtree node;
	SEventData data;
};

int EventSystem_Register( int id, int obj );
int EventSystem_Unregister( int id, int obj );
int EventSystem_Trigger( int id, void* data );
int EventSystem_Compare( SBtree *key, SBtree *leaf );

#define BUILDEVENT(s, pf) { E_##s, "" #s "", NULL, (eventtriggercbf)pf##_Trigger, (eventregistercbf)pf##_Register, (eventunregistercbf)pf##_Unregister, (comparecbf)pf##_Compare }

static SEventHash s_EventObject[eEvents_MAX] =
{
	BUILDEVENT(DEFAULT, EventSystem),
	BUILDEVENT(KEYBOARD, EventSystem),
	BUILDEVENT(MOUSE, EventSystem),
	BUILDEVENT(TRIGGER, EventSystem),
};

SEventBtree g_FreeList[1000];
SLList *g_Head = NULL;

///////////////////////////////////////////////////////////////////
// Pump routine for Event system

void EventSystem_Update( void )
{

}

///////////////////////////////////////////////////////////////////
// Default callbacks

int EventSystem_Compare( SBtree *key, SBtree *leaf )
{
	if( ((SEventBtree*)key)->data.objid == ((SEventBtree*)leaf)->data.objid )
	{
		return 0;
	}

	if( ((SEventBtree*)key)->data.objid < ((SEventBtree*)leaf)->data.objid )
	{
		return -1;
	}

	return 1;
}

int EventSystem_Register( int id, int obj )
{
	if( id >= 0 && id <= eEvents_MAX )
	{
		SEventHash *pEventObject = s_EventObject + id;
		printf("Registering %d with %s(%d)", obj, pEventObject->cEvent, pEventObject->cEvent);

		SEventBtree tmpEvent;
		tmpEvent.data.objid = id;

		// Verify single instance before attaching
		if( SBtree_FindLeaf( pEventObject->head, (SBtree*)&tmpEvent, E_MERGE, pEventObject->eventcomparecbf ) != 0 )
		{
			if( SLList_GetFree( g_Head, *(SLList*)&tmpEvent ) == 0 )
			{
				tmpEvent.data.objid = id;
				return SBtree_AddLeaf( pEventObject->head, (SBtree*)&tmpEvent, pEventObject->eventcomparecbf );
			}
		}
	}	
	return -1;
}

int EventSystem_Unregister( int id, int obj )
{
	if( id >= 0 && id <= eEvents_MAX )
	{
		SEventHash *pEventObject = s_EventObject + id;
		printf("Unregistering %d with %s(%d)", obj, pEventObject->cEvent, pEventObject->cEvent);

		SEventBtree tmpEvent, *pEvent = &tmpEvent;
		tmpEvent.data.objid = id;

		if( SBtree_SubLeaf( pEventObject->head, (SBtree*&)pEvent, pEventObject->eventcomparecbf ) == 0 )
		{
			// Add the children back on!
			if( SBtree_AddLeaf( pEventObject->head, tmpEvent.node.left, pEventObject->eventcomparecbf ) == 0 )
			{
				if ( SBtree_AddLeaf( pEventObject->head, tmpEvent.node.right, pEventObject->eventcomparecbf ) == 0 )
				{
					memset( pEvent, 0, sizeof(pEvent) );
					return SLList_PutFree( g_Head, (SLList*)pEvent );
				}
			}			
		}
	}	
	return -1;
}

int EventSystem_Trigger( int id, void *data )
{
	if( id >= 0 && id <= eEvents_MAX )
	{
		printf("", s_EventObject[id].cEvent);
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////////////
// Test code! Call from rundll

struct SProfile
{
	int iframes;
	int objectsProcessed; // current frame
	float seconds;
	float objectsPerFrame;
	float secondsPerFrame;
};

#include "Math.h"

extern "C" __declspec (dllexport) void __cdecl TestDll (
   HWND hwnd,        // handle to owner window
   HINSTANCE hinst,  // instance handle for the DLL
   LPTSTR lpCmdLine, // string the DLL will parse
   int nCmdShow      // show state
)
{
	memset( g_FreeList, 0, sizeof(g_FreeList) );	

	// initialize freelist! in game should be pre setup!
	for( int i=1; i<sizeof(g_FreeList)/sizeof(*g_FreeList); i++)
	{
		int ret = SLList_PutFree( g_Head, (SLList*)g_FreeList+i );
		ret;		
	}

	const int totalFrames = 10000;
	SProfile stats = {0,0,0,0,0};

	// Process 'Objects'
	for( stats.iframes; stats.iframes < totalFrames; stats.iframes++ )
	{
		// Add a series of events
		

		// Add event handles for different events

		// Trigger events


		// calculate new average
	}
}