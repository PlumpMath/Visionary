
#include <Python.h>
#include "stdafx.h"
#include <TypeDefinitions.h>
#include "GameObject.h"
#include "Mouse.h"
#include "Keys.h"
#include "Player.h"

////////////////////////////////////
// Import functions

PyObject *PythonImport_MovePlayer( PyObject *self, PyObject *args )
{
	// This function takes in SMouseInfo, SKeysInfo, and Camera Object
	SMouseInfo *pMouse;
	SKeyInfo *pKeys;
	SCamera *pCamera;

	// No 'safe' type checking here (should create types for these)
	if( PyArg_ParseTuple(args, "lll", &pMouse, &pKeys, &pCamera) )
	{
		VisInstancePtr* pPlayer = &gp_GameObject->m_PlayerObject;
		if (pPlayer)
		{
			// this is parameterized data for 1/30 sec frames
			static double OneThirtyith	= 1/30.0;	// the length of time of 1 frame @ 30 fps
			static double nRotation		= PI/180 * 3 / OneThirtyith; // 3 degrees per frame
			static double nDistance		= 1.0 / OneThirtyith; // 1 foot per second
			double deltaTime			= g_GlobalData.deltaTime;
			short forward				= 0;
			short side					= 0;

			// divide by zero correction
			if (deltaTime == 0)				deltaTime = OneThirtyith;

			// This is to account for frame rate flux
			double nPercent = deltaTime / OneThirtyith;

			// Check for valid keys
			if (pKeys)
			{						
				// A/<- (+) Rotate, or w/Rt Mouse Strafe
				if (pKeys->keys['A'][0].toggle || pKeys->keys[VK_LEFT][0].toggle)		side--;

				// D/-> (-) Rotate, or w/Rt Mouse Strafe
				if (pKeys->keys['D'][0].toggle || pKeys->keys[VK_RIGHT][0].toggle)		side++;

				// W/Up (+) Forward
				if (pKeys->keys['W'][0].toggle || pKeys->keys[VK_UP][0].toggle)			forward++;

				// S/Down (-) Backward
				if (pKeys->keys['S'][0].toggle || pKeys->keys[VK_DOWN][0].toggle)		forward--;
				
				// '~' is lock run
//				if (pKeys->keys['A'][0].toggle || pKeys->keys[VK_LEFT][0].toggle)
			}
			// Apply any side motion
			if (side != 0)
			{
				if (pMouse && pMouse->rbtn)
				{
					// Player strafes if right mouse button held down also
					Player_Strafe(pPlayer, pCamera, nDistance * side * nPercent);
				}
				else
				{
					// Player rotates
				}
			}
			// Apply any forward motion last
			if (forward != 0)
			{
				Player_Forward(pPlayer, pCamera, nDistance * forward * nPercent);
			}
			if (side || forward)						return PyInt_FromLong(1);
		}
		return PyString_FromString("");
	}

	return NULL;
}

PyObject *PythonImport_GetPlayer( PyObject *self, PyObject *args )
{
	VisInstancePtr* pPlayer = &gp_GameObject->m_PlayerObject;

	// if the player pointer is NULL return NULL
	if( pPlayer )
	{
		PyObject* pObject = PyLong_FromVoidPtr( (void*)pPlayer );
		if( pObject )
		{
			// increment the reference count?
			Py_INCREF( pObject );
			return pObject;
		}

		// throw exception
		return PyErr_NoMemory();
	}

	return PyLong_FromLong(0);
}

PyObject *PythonImport_GetMouse( PyObject *self, PyObject *args )
{
	SMouseInfo *pMouse = Mouse_GetMouseData();

	// mouse always needs to be returned
	PyObject* pObject = PyLong_FromVoidPtr( (void*)pMouse );
	if( pObject )
	{
		// increment the reference count?
		Py_INCREF( pObject );
		return pObject;
	}

	// throw exception
	return PyErr_NoMemory();
}

PyObject *PythonImport_GetKeys( PyObject *self, PyObject *args )
{
	// if a key wasn't pressed then return NULL
	if( Keys_HasChanged() )
	{
		SKeyInfo *pKeys = Keys_GetKeyData();

		PyObject* pObject = PyLong_FromVoidPtr( (void*)pKeys );
		if( pObject )
		{
			// increment the reference count?
			Py_INCREF( pObject );
			return pObject;
		}

		// throw exception
		return PyErr_NoMemory();
	}

	return PyLong_FromLong(0);
}

static PyObject *PythonImport_NoOp ( PyObject *self, PyObject *args )
{
	int sts = 0;
   
    if (PyArg_ParseTuple(args, "i", &sts))
	{    
		// Return void ... thusly
		Py_INCREF(Py_None);
		return Py_BuildValue("i", sts);
	}

	return NULL;
}

static PyMethodDef ImportMethods[] = 
{
    {"NoOp",  PythonImport_NoOp, METH_VARARGS, "Null Function Command (exersize example)"},

	{"GetKeys",  PythonImport_GetKeys, METH_VARARGS, "Get Key Input Reference"},
	{"GetMouse",  PythonImport_GetMouse, METH_VARARGS, "Get Mouse Input Reference"},
	{"GetPlayer",  PythonImport_GetPlayer, METH_VARARGS, "Returns Pointer to Player object"},

	{"MovePlayer",  PythonImport_MovePlayer, METH_VARARGS, "Moves player according to input and camera"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};


void PythonImport_Initialize( void )
{
	PyObject* m;

    m = Py_InitModule("Visionary", ImportMethods);

	// Use m to instantiate any module variables
}

void PythonImport_Finalize( void )
{

}