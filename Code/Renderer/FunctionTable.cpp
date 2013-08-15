
#include "stdafx.h"

#include "LibLoader.h"
#include "Renderer.h"

SLibFuncTabl g_RenderFunctionTable[] =
{
	FUNCTABENTRY( Render_Initialize, VPT_INITIALIZE, VPT_RENDER ),
	FUNCTABENTRY( Render_Finalize, VPT_FINALIZE, VPT_RENDER ),

	FUNCTABENTRY( Light_Render, VPT_RENDER, VIT_LIGHT ),
	FUNCTABENTRY( Camera_Render, VPT_RENDER, VIT_CAMERA ),
	FUNCTABENTRY( Model_Render, VPT_RENDER, VIT_MODEL ),

	FUNCTABENTRY( Render_BeginFrame, VPT_BEGINFRAME, NULL ),
};

SLibFuncTabl* Render_GetFunctionTable( int &nFuncTabl )
{
	nFuncTabl = sizeof(g_RenderFunctionTable)/sizeof(*g_RenderFunctionTable);
	return g_RenderFunctionTable;
}