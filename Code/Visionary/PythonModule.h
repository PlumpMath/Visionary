// #pragma once

#ifndef PYTHONMODULE_H
#define PYTHONMODULE_H

#include <stdarg.h>

struct VisScript;

void PythonModule_Initialize( char* pModuleName );
void PythonModule_Finalize( void );
void PythonModule_Script( VisScript *pScript, int nCount, char* fmt, ... );

#endif