
#include "stdafx.h"
#include "LibLoader.h"

SLibFuncTabl g_RenderFunctionTable[] =
{
	// FUNCTABENTRY( Render_Initialize, VPT_INITIALIZE, VPT_RENDER ),
	FUNCTABENTRY(0,0,0)
};

SLibFuncTabl* Collision_GetFunctionTable( int &nFuncTabl )
{
	nFuncTabl = sizeof(g_RenderFunctionTable)/sizeof(*g_RenderFunctionTable);
	return g_RenderFunctionTable;
}