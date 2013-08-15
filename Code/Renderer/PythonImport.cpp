
#include <Python.h>
#include "TypeDefinitions.h"
#include "Camera.h"

////////////////////////////////////
// Import functions

extern SGlobalData *gp_GlobalData;
extern SCamera cam;

static PyObject *PythonImport_GetCamera ( PyObject *self, PyObject *args )
{
	// get the global camera from camera module!
	static SCamera *pCamera = &cam;
	
	PyObject* pObject = PyLong_FromVoidPtr( (void*)pCamera );
	if( pObject )
	{
		// increment the reference count?
		Py_INCREF( pObject );
		return pObject;
	}
	// throw exception
	return PyErr_NoMemory();
}


static PyObject *PythonImport_OrientCamera ( PyObject *self, PyObject *args )
{
	// This function takes in SMouseInfo, SKeysInfo, and Camera Object
	SMouseInfo *pMouse;
	SKeyInfo *pKeys;
	SCamera *pCamera;

	static double rotFactor	= 1;		// This is for mouse sensitivity set by user in options (move out later)
	static double whlFactor	= 1;

	// No 'safe' type checking here (should create types for these)
	if( PyArg_ParseTuple(args, "lll", &pMouse, &pKeys, &pCamera) )
	{
		// Camera rotation with right mouse button only
		if (pMouse && pMouse->rbtn)
		{
			// Difference in mouse
			double deltaX	= pMouse[0].position.x - pMouse[0].prevPosition.x;
			double deltaY	= pMouse[0].prevPosition.y - pMouse[0].position.y;

			// Rotate the camera (reverse axis from Windows)
			Camera_Rotate(pCamera, (visfloat)(deltaY * rotFactor), (visfloat)(deltaX * rotFactor));
			return PyInt_FromLong(1);
		}
		// Adjust the zoom
		if (pMouse && pMouse->zDelta)
		{
			return PyInt_FromLong(1);

		}
		return PyString_FromString("");
	}
	return NULL;
}


static PyObject *PythonImport_PositionCamera ( PyObject *self, PyObject *args )
{
	VisInstancePtr *pPlayer;
	SCamera *pCamera;

	// No 'safe' type checking here (should create types for these)
	if( PyArg_ParseTuple(args, "ll", &pPlayer, &pCamera) )
	{
		// Retrieve the matrix
	}
	return NULL;
}



static PyObject *PythonImport_TranslateCamera ( PyObject *self, PyObject *args )
{
	// Renderer.PositionCamera( oPlayer, oCamera )
	VisInstance *pPlayer;
	SCamera *pCamera;

	// No 'safe' type checking here (should create types for these)
	if( PyArg_ParseTuple(args, "ll", &pPlayer, &pCamera) )
	{
		// um Lane, this one is a little more complex.  Good Luck m'boy!
	}
	return NULL;
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

	{"GetCamera",  PythonImport_GetCamera, METH_VARARGS, "Returns the global camera"},
	{"OrientCamera",  PythonImport_OrientCamera, METH_VARARGS, "Changes camera orientation based on mouse input"},
	{"PositionCamera",  PythonImport_PositionCamera, METH_VARARGS, "Positions Camera according to Player"},

    {NULL, NULL, 0, NULL}        /* Sentinel */
};


void PythonImport_Initialize( void )
{
	PyObject* m;

    m = Py_InitModule("Renderer", ImportMethods);

	// Use m to instantiate any module variables
}

void PythonImport_Finalize( void )
{

}