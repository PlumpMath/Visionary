// PythonModule
#include "stdafx.h"

// #include <stdio.h>
#include <TypeDefinitions.h>
#include "PythonModule.h"

extern void PythonImport_Initialize( void );

bool g_bPythonInitialized = false;

#include <Python.h>

void PythonModule_Initialize( char* pModuleName )
{
	// Pass argv[0] to the Python interpreter 
    Py_SetProgramName( pModuleName );

    // Create the Python Interface
    Py_Initialize();

    /* Add a static module */
    PythonImport_Initialize();

	g_bPythonInitialized = true;
}

void PythonModule_Finalize( void )
{
	g_bPythonInitialized = false;

	// Destroy the Python Interface
	Py_Finalize();
}

void PythonModule_Script( VisScript *pScript, int nCount, char* fmt, ... )
{
#ifdef _DEBUG
	printf("Warning: Python Scripting disabled in debug!\n");
	return;
#endif

	if( g_bPythonInitialized == false )			return;

	// Validate Id
	if( pScript->header.data.id.type == VIT_SCRIPT )
	{
		FILE* mystdout = freopen( "c:\\stdout.txt", "a", stdout );

		FILE* fpy = fopen(pScript->File, "r" );
		if( fpy )
		{
			try
			{
				fprintf( mystdout, "Info: Launching <%s>\n", pScript->File);
				int ret = PyRun_SimpleFile( fpy, pScript->File );
				fclose(fpy);
			}
			catch(...)
			{
				fprintf( mystdout, "Exception: Wouldn't it be nice if you caught your own exceptions?\n");
			}
		}
		else
		{
			fprintf( mystdout, "Error: <%s> could not be opened\n", pScript->File);
		}

		fclose(mystdout);
	}
}
