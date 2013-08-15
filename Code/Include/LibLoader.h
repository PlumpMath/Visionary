#pragma once

#ifndef VISIONARY_LIBLOADER_H
#define VISIONARY_LIBLOADER_H

struct SLibFuncTabl
{
	char *sName;
	int ePass;
	int eType;
	FARPROC fpFunct;
};

typedef SLibFuncTabl* (*fpFuncTabl)( int &nFuncTabl );

// Use this to help define your Function Table
#define FUNCTABENTRY( name, pass, type ) { "" #name "", pass, type, (FARPROC)name }

#ifdef VISIONARY
// Internal includes

struct SLoadLibs
{
	char sName[257];
	int nLib;			// Library Identifier

	HMODULE mLib;
	FARPROC fpFnTabl;	// Pointer to Module Function Table

	bool initialized;	// Stores value to keep from being initalized multiple times
	bool needPatch;		// Stores value to decide if Function addresses
						// need patching before using

	int nLibFuncTabl;			// Count of Function Table
	SLibFuncTabl *pLibFuncTabl;	// Pointer to Function Table	
};

bool LibLoader_Initialize( void );
bool LibLoader_Finalize( void );

#endif

#endif // ndef VISIONARY_LIBLOADER_H
