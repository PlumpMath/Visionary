
#include "stdafx.h"
#include "Collision.h"

/////////////////////////////////////
//  For this module to work:
//   I need to watch for any Instance objects that are loaded
//   When the event update happens, add to Collision must happen.
//	 Each collision object is added to a Portal, each portal has
//    a radius of 500 square meters.  Should divide a secton into
//	  quarters.  

void Collision_Initialize( void )
{
	// Initialize module components

}

void Collision_Add( void )
{
	// Start with the last inserted Portal
	//  If in the radius Add, otherwise test
	//  and move
}

void Collision_Delete( void )
{

}

void Collision_Sort( void )
{
	// Walk the list of objects
		// mark objects that are out of order
		// sort the out of order objects
		// insert each out of order object
}

void Collision_Update( void )
{
	// Examine the list of updated portals, sort the lists

	// Walk list of moved object
		// verify they are in the correct Portal
		// still (use center point), if not move it to the next section
		// Add any close objects to the "Im real close list"
		// Check all of the real close, if not close enough remove
		// If close enough do extra close check against bounding box
		// If bound box is hit create collision event object

	// When done checking all dynamic objects send all collision events
}