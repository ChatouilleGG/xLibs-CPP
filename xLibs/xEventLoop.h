#pragma once

#include "xMap.h"
#include "xArray.h"

#include "Windows.h"

typedef void(*xTickFunc)(unsigned long);
typedef void(*xCallbackFunc)(unsigned long, xMap*);

class xCallbackable {};

typedef void(xCallbackable::*xTickFunc_o)(unsigned long);
typedef void(xCallbackable::*xCallbackFunc_o)(unsigned long, xMap*);

#define GEN_TICK_MEMBERFUNC(object, classname, funcname) (xCallbackable*)&object, (xTickFunc_o)(&classname::funcname)
#define GEN_TICK_MEMBERFUNC_P(objptr, classname, funcname) (xCallbackable*)objptr, (xTickFunc_o)(&classname::funcname)

#define GEN_CALLBACK_MEMBERFUNC(object, classname, funcname) (xCallbackable*)&object, (xCallbackFunc_o)(&classname::funcname)
#define GEN_CALLBACK_MEMBERFUNC_P(objptr, classname, funcname) (xCallbackable*)objptr, (xCallbackFunc_o)(&classname::funcname)

typedef struct
{
	xCallbackable* obj;
	union {
		xTickFunc func;
		xTickFunc_o func_o;
	};
} xTickHandler;

typedef struct
{
	unsigned long date;
	xCallbackable* obj;
	union {
		xCallbackFunc func;
		xCallbackFunc_o func_o;
	};
	xMap* args;
	unsigned long from;
} xEvent;


class xEventLoop
{

public:
	static xEventLoop& instance(void);

	inline void set_dt(unsigned long ms) { target_dt = ms; }
	inline unsigned long get_dt(void) { return target_dt; }
	void add_tick(xTickFunc);
	void add_tick(xCallbackable* obj, xTickFunc_o);
	void remove_tick(xTickFunc);
	void remove_tick(xCallbackable* obj, xTickFunc_o);

	void callback(xCallbackFunc, xMap*, unsigned long);
	void callback(xCallbackable* obj, xCallbackFunc_o, xMap*, unsigned long);
	void start(void);
	inline void start(unsigned long dt) { set_dt(dt); start(); }
	void stop(bool clear = false);
	void clear(void);
	inline bool running(void) { return instance().brunning; }

	unsigned long gettime(void);

private:
	static xMap thread_loop;

	unsigned long target_dt;
	xArray tick_handlers;
	xArray events;
	xArray events_to_add;
	bool brunning;
	bool bstop;

	LARGE_INTEGER counts_per_sec;
	bool perfcounter_init;

	xEventLoop();
	~xEventLoop();

	void _sleep(unsigned long);
	void _insert(xEvent*, xArray&, int, int);

	inline xEventLoop(const xEventLoop&) {};
	inline xEventLoop& operator =(const xEventLoop&) {};

};

