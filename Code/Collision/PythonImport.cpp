
#include <Python.h>
#include "stdafx.h"
#include "TypeDefinitions.h"

////////////////////////////////////
// Import functions

extern SGlobalData *gp_GlobalData;

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

	{NULL, NULL, 0, NULL}        /* Sentinel */
};


void PythonImport_Initialize( void )
{
	PyObject* m;

    m = Py_InitModule("Collision", ImportMethods);

	// Use m to instantiate any module variables
}

void PythonImport_Finalize( void )
{

}