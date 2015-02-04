#pragma once

#include "xEventLoop.h"

typedef xMap* (*xAsyncFunc)(xMap*);
typedef xMap* (xCallbackable::*xAsyncFunc_o)(xMap*);

#define GEN_ASYNC_MEMBERFUNC(object, classname, funcname) (xCallbackable*)&object, (xAsyncFunc_o)(&classname::funcname)
#define GEN_ASYNC_MEMBERFUNC_P(objptr, classname, funcname) (xCallbackable*)objptr, (xAsyncFunc_o)(&classname::funcname)


struct xAsyncTask
{
	xCallbackable* obj_func;
	union {
		xAsyncFunc func;
		xAsyncFunc_o func_o;
	};
	xMap* args;
	xCallbackable* obj_cb;
	union {
		xCallbackFunc callback;
		xCallbackFunc_o callback_o;
	};
	unsigned long timestamp;
	void* thread;
	xMap* result;
};

class xAsync
{

public:
	static xAsyncTask* makeTask(xAsyncFunc, xMap*, xCallbackFunc);
	static xAsyncTask* makeTask(xAsyncFunc, xMap*, xCallbackable*, xCallbackFunc_o);
	static xAsyncTask* makeTask(xCallbackable*, xAsyncFunc_o, xMap*, xCallbackFunc);
	static xAsyncTask* makeTask(xCallbackable*, xAsyncFunc_o, xMap*, xCallbackable*, xCallbackFunc_o);

	static void start(xAsyncTask*);

private:
	static xMap thread_async;

	xArray tasks;

	xAsync();
	~xAsync();

	static xAsync& _instance(void);
	static DWORD _entry(xAsyncTask*);
	void _tick(unsigned long);

};

