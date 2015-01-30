#include <Python.h>
#include "environment.h"

class PyAgent : public IAgent {
	public:
	AgentStatus Update(EnvirInfo info) {
		PyObject* args = PyTuple_New(8);
		
		//bot id
		PyObject* val = PyLong_FromLong(info.id);
		PyTuple_SetItem(args, 0, val);
		//team
		val = PyLong_FromLong(info.team);
		PyTuple_SetItem(args, 1, val);
		//x position
		val = PyFloat_FromDouble(info.position.x);
		PyTuple_SetItem(args, 2, val);
		//y position
		val = PyFloat_FromDouble(info.position.y);
		PyTuple_SetItem(args, 3, val);

		PyObject* tuple1 = PyTuple_New(RAY_COUNT);
		for(int i = 0; i < RAY_COUNT; ++i) {
			val = PyLong_FromLong(info.ray_length[i]);
			PyTuple_SetItem(tuple1, i, val);
		}
		PyTuple_SetItem(args, 4, tuple1);
		//Py_DECREF(tuple);

		PyObject* tuple2 = PyTuple_New(RAY_COUNT);
		for(int i = 0; i < RAY_COUNT; ++i) {
			val = PyLong_FromLong(info.ray_detail[i]);
			PyTuple_SetItem(tuple2, i, val);
		}
		PyTuple_SetItem(args, 5, tuple2);
		//Py_DECREF(tuple);

		PyObject* tuple3 = PyTuple_New(RAY_COUNT);
		for(int i = 0; i < RAY_COUNT; ++i) {
			val = PyLong_FromLong(info.ray_udata[i]);
			PyTuple_SetItem(tuple3, i, val);
		}
		PyTuple_SetItem(args, 6, tuple3);
		//Py_DECREF(tuple);

		PyObject* tuple4 = PyTuple_New(RAY_COUNT);
		for(int i = 0; i < RAY_COUNT; ++i)
		{
			val = PyFloat_FromDouble(info.ray_pos[i].x);
			PyObject* val2 = PyFloat_FromDouble(info.ray_pos[i].y);
			PyObject* t = PyTuple_New(2);
			PyTuple_SetItem(t, 0, val);
			PyTuple_SetItem(t, 1, val2);
			PyTuple_SetItem(tuple4, i, t);
		}
		PyTuple_SetItem(args, 7, tuple4);
		val = PyObject_CallObject(func, args);
		Py_DECREF(tuple1); Py_DECREF(tuple2); Py_DECREF(tuple3);
		Py_DECREF(args);

		if(val == 0) {
			printf("PY: Failed to call update.\n");
			PyErr_Print();
			exit(0);
		}
		AgentStatus out;
		if(!PyTuple_Check(val)) {
			printf("Not a tupple.\n");
			exit(0);
		}
		if(PyTuple_Size(val) != 4) {
			printf("Tupple's size does not equal to 4.\n");
			exit(0);
		}
		out.forward = (bool)PyLong_AsLong(PyTuple_GetItem(val, 0));
		out.left = (bool)PyLong_AsLong(PyTuple_GetItem(val, 1));
		out.right = (bool)PyLong_AsLong(PyTuple_GetItem(val, 2));
		out.fire = (bool)PyLong_AsLong(PyTuple_GetItem(val, 3));
		return out;
	}
	PyAgent(PyObject* f) 
		: func(f) {


	}

	PyObject* func;
};


bool initted = false;
//void KillPython();
IAgent* CreatePyAgent(wchar_t* name, const char* filename) {
	if(!initted) {
		initted = true;
		Py_SetProgramName(name);
		Py_Initialize();
		PyObject* m = PyImport_ImportModule("os");
		PyObject* f = PyObject_GetAttrString(m, "getcwd");
		PyObject* r = PyObject_CallObject(f,0);

	  	PyObject *sysPath = PySys_GetObject("path");

	    int result = PyList_Insert(sysPath, 0, r);
	}

	PyObject* pModule = PyImport_ImportModule(filename);
	if(pModule != 0) {
		PyObject* func = PyObject_GetAttrString(pModule, "update");
		if(func && PyCallable_Check(func)) {
			return new PyAgent(func);
		} else {
			printf("No function update.\n");
			return 0;
		}
	}else {
		PyErr_Print();
		exit(0);
		return 0;
	}
	return 0;
}