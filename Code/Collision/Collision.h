
#pragma once

#ifndef COLLISION_COLLISION_H
#define COLLISION_COLLISION_H

#include <TypeDefinitions.h>
#include <Stack.h>

struct VisConstraints
{
	// stack object
	SStack objects;
	visfloat radius;
};

struct VisBoundingArea
{
	// left, right, up, down, front, back
	SexTree nodes;
	VisConstraints region;
};

template <int N> // should be 3 or 4
struct SVertex
{
	visfloat coord[N];
};

struct SObb
{
	SVertex<3> obb[8];
};

struct VisCollision
{
	SCommonObjectHeader header;

	// Radi is an array of circular bounding objects
	visfloat* dRadi;
	visint	  iRadi;

	// I need an OBB
	SObb Obb; // 8 positions centered around object	
};

// Structure keeps track of previous collisions
struct VisCollidable
{
	// keep track of position in

};



#endif