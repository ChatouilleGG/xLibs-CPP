#include "stdafx.h"
#include "xAsync.h"

//================================================================
// Constructors
//================================================================

xMap xAsync::thread_async;

xAsync::xAsync()
{
	xEventLoop::instance().add_tick(GEN_TICK_MEMBERFUNC_P(this, xAsync, _tick));
}

xAsync& xAsync::_instance(void)
{
	xString id("%x", GetCurrentThreadId());
	xAsync* async = (xAsync*)xAsync::thread_async.getp(id);
	if (async == NULL)
	{
		async = new xAsync();
		xAsync::thread_async.put(id, async, VAL_PTR);
	}
	return *async;
}


//================================================================
// Public Functions
//================================================================

//==================== task params generation ====================

xAsyncTask* xAsync::makeTask(xAsyncFunc func, xMap* args, xCallbackFunc cb)
{
	xAsyncTask* task = (xAsyncTask*)malloc(sizeof(xAsyncTask));
	if (task == NULL) throw "out of memory";
	task->obj_func = NULL;
	task->func = func;
	task->args = args;
	task->obj_cb = NULL;
	task->callback = cb;
	task->thread = NULL;
	task->result = NULL;
	return task;
}

xAsyncTask* xAsync::makeTask(xAsyncFunc func, xMap* args, xCallbackable* obj_cb, xCallbackFunc_o cb_o)
{
	xAsyncTask* task = (xAsyncTask*)malloc(sizeof(xAsyncTask));
	if (task == NULL) throw "out of memory";
	task->obj_func = NULL;
	task->func = func;
	task->args = args;
	task->obj_cb = obj_cb;
	task->callback_o = cb_o;
	task->thread = NULL;
	task->result = NULL;
	return task;
}

xAsyncTask* xAsync::makeTask(xCallbackable* obj_func, xAsyncFunc_o func_o, xMap* args, xCallbackFunc cb)
{
	xAsyncTask* task = (xAsyncTask*)malloc(sizeof(xAsyncTask));
	if (task == NULL) throw "out of memory";
	task->obj_func = obj_func;
	task->func_o = func_o;
	task->args = args;
	task->obj_cb = NULL;
	task->callback = cb;
	task->thread = NULL;
	task->result = NULL;
	return task;
}

xAsyncTask* xAsync::makeTask(xCallbackable* obj_func, xAsyncFunc_o func_o, xMap* args, xCallbackable* obj_cb, xCallbackFunc_o cb_o)
{
	xAsyncTask* task = (xAsyncTask*)malloc(sizeof(xAsyncTask));
	if (task == NULL) throw "out of memory";
	task->obj_func = obj_func;
	task->func_o = func_o;
	task->args = args;
	task->obj_cb = obj_cb;
	task->callback_o = cb_o;
	task->thread = NULL;
	task->result = NULL;
	return task;
}

//==================== running ====================

void xAsync::start(xAsyncTask* task)
{
	if (task == NULL) return;

	DWORD tid;
	task->thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(xAsync::_entry), task, CREATE_SUSPENDED, &tid);
	if (task->thread == NULL)
	{
		free(task);
		throw "xAsync: failed to create thread";
	}
	_instance().tasks.add(task);
	task->timestamp = xEventLoop::instance().gettime();
	ResumeThread(task->thread);
}


//================================================================
// Private Functions
//================================================================

DWORD xAsync::_entry(xAsyncTask* task)
{
	if (task->obj_func != NULL)
		task->result = CALL_MEMBERFUNC_P(task->obj_func, task->func_o) (task->args);
	else
		task->result = task->func(task->args);

	ExitThread(0);
}

void xAsync::_tick(unsigned long dt)
{
	for (int i = 0; i < tasks.size(); i++)
	{
		xAsyncTask* task = (xAsyncTask*)tasks.getp(i);
		DWORD code;
		if (GetExitCodeThread(task->thread, &code))
		{
			if (code == STILL_ACTIVE) continue;

			tasks.remove(i);
			i--;

			unsigned long dt = xEventLoop::instance().gettime() - task->timestamp;
			if (task->obj_cb != NULL)
				CALL_MEMBERFUNC_P(task->obj_cb, task->callback_o) (dt, task->result);
			else
				task->callback(dt, task->result);

			free(task);
		}
	}
}


//================================================================
// Destructors
//================================================================

xAsync::~xAsync()
{
	xEventLoop::instance().remove_tick(GEN_TICK_MEMBERFUNC_P(this, xAsync, _tick));
	for (int i = 0; i < tasks.size(); i++)
		free((xAsyncTask*)tasks.getp(i));
	tasks.empty();
}
