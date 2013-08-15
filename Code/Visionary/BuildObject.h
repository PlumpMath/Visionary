#pragma once

#include <TypeDefinitions.h>

extern bool BuildObject_Instance( VisInstance &instance, visint size, VisInstance *Attach, bool asChild = false );
extern bool BuildObject_Camera( VisCamera &camera, bool asOrtho = false, visfloat fNear = 1, visfloat fFar = 400, visfloat fAspect = 0, visfloat fRov = 60 );
extern bool BuildObject_Light( VisLight &light, visint size, vislong lightid );