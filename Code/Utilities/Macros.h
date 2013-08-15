
#pragma once
#ifndef UTILITIES_MACROS_H
#define UTILITIES_MACROS_H

// usage
// msg may be replaced with your own buffer
// ????PRINT( ( msg, <fmtarg>, <varargs> ) ) 

#ifdef _DEBUG
#define DBGPRINT( x ) { char msg[1024]; sprintf x ; OutputDebugString( msg ); }
#define NDBGPRINT	  DBGPRINT
#else
#define DBGPRINT( x )
#define NDBGPRINT( x ) { char msg[1024]; sprintf x ; OutputDebugString( msg ); }
#endif

#endif // ndef UTILITIES_MACROS_H