
#include "stdafx.h"
#include "LibLoader.h"

SLoadLibs g_LoadLibs[129];
int g_nLoadLibs = 0;

int LibLoader_LoadLibs( char * LibFiles[], int nLibFiles );
int LibLoader_UnloadLibs( void );

#define LOADDIR "" __LOADDIR__ ""

bool LibLoader_Initialize()
{
	char *sModules[] = { "..\\Renderer\\" LOADDIR "\\Renderer.dll" };
	if( LibLoader_LoadLibs( sModules, sizeof(sModules)/sizeof(*sModules) ) == 0 )
	{
		return true;
	}

	return false;
}

bool LibLoader_Finalize()
{
	if( LibLoader_UnloadLibs() == 0 )
	{
		return true;
	}

	return false;
}

int LibLoader_LoadLibs( char * LibFiles[], int nLibFiles )
{
	char ** pEnd = LibFiles + nLibFiles;
	for( char** pBeg=LibFiles; pBeg<pEnd; pBeg++ )
	{
		DBGPRINT( (msg, "Loading Library %s\n", *pBeg) )

		HMODULE mLib = LoadLibrary( *pBeg );
		if( mLib )
		{
			FARPROC fpFnTabl = GetProcAddress( mLib, "GetFunctionTable" );
			if( fpFnTabl )
			{
				int nLibFuncTabl;
				SLibFuncTabl *pLibFuncTabl = ((fpFuncTabl)fpFnTabl)( nLibFuncTabl );
				if( pLibFuncTabl && nLibFuncTabl > 0 )
				{
					SLoadLibs &LoadLibs = g_LoadLibs[g_nLoadLibs];

					// Save the Module, Function data for later use
					strncpy( LoadLibs.sName, *pBeg, sizeof(LoadLibs.sName)/sizeof(*LoadLibs.sName));
					
					LoadLibs.pLibFuncTabl = pLibFuncTabl;
					LoadLibs.nLibFuncTabl = nLibFuncTabl;
					LoadLibs.mLib = mLib;
					LoadLibs.needPatch = ( (__int64)pLibFuncTabl->fpFunct < (__int64)mLib );
					LoadLibs.nLib = g_nLoadLibs++;

					static int nLoadLibs = sizeof(g_LoadLibs)/sizeof(*g_LoadLibs);
					if( g_nLoadLibs >= nLoadLibs )
					{
						break;
					}

					nLibFiles--;
				}
			}
		}
	}

	return nLibFiles;
}

int LibLoader_UnloadLibs( void )
{
	SLoadLibs *pEnd = g_LoadLibs + g_nLoadLibs;
	for( SLoadLibs *pBeg=g_LoadLibs; pBeg<pEnd; pBeg++ )
	{
		DBGPRINT( ( msg, "Unloading Library %s\n", pBeg->sName ) )

		if( FreeLibrary( pBeg->mLib ) )
		{
			g_nLoadLibs--;
		}
	}

	return g_nLoadLibs;
}

/*int LibLoader_UnLoadLibs( char * LibFiles[], int nLibFiles )
{
	return -1;
}*/