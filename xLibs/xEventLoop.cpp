#include "stdafx.h"
#include "xEventLoop.h"

#include <Windows.h>
#include <mmsystem.h>


//================================================================
// Constructors
//================================================================

xMap xEventLoop::thread_loop;

xEventLoop::xEventLoop()
{
	target_dt = 1;
	perfcounter_init = false;
	brunning = false;
}

xEventLoop& xEventLoop::instance(void)
{
	xString id("%x", GetCurrentThreadId());
	xEventLoop* loop = (xEventLoop*)xEventLoop::thread_loop.getp(id);
	if (loop == NULL)
	{
		loop = new xEventLoop();
		xEventLoop::thread_loop.put(id, loop, VAL_PTR);
	}
	return *loop;
}


//================================================================
// Public Functions
//================================================================

//==================== params ====================

void xEventLoop::add_tick(xTickFunc func)
{
	xTickHandler* handler = (xTickHandler*)malloc(sizeof(xTickHandler));
	if (handler == NULL) throw "out of memory";
	handler->obj = NULL;
	handler->func = func;
	tick_handlers.add(handler);
}

void xEventLoop::add_tick(xCallbackable* obj, xTickFunc_o func)
{
	xTickHandler* handler = (xTickHandler*)malloc(sizeof(xTickHandler));
	if (handler == NULL) throw "out of memory";
	handler->obj = obj;
	handler->func_o = func;
	tick_handlers.add(handler);
}

void xEventLoop::remove_tick(xTickFunc func)
{
	for (int i = 0; i < tick_handlers.size(); i++)
	{
		xTickHandler* handler = (xTickHandler*)tick_handlers.getp(i);
		if ( handler->obj == NULL && handler->func == func )
		{
			tick_handlers.remove(i);
			break;
		}
	}
}

void xEventLoop::remove_tick(xCallbackable* obj, xTickFunc_o func)
{
	for (int i = 0; i < tick_handlers.size(); i++)
	{
		xTickHandler* handler = (xTickHandler*)tick_handlers.getp(i);
		if (handler->obj == obj && handler->func_o == func)
		{
			tick_handlers.remove(i);
			break;
		}
	}
}

//==================== run ====================

void xEventLoop::callback(xCallbackFunc func, xMap* args, unsigned long delay)
{
	long now = gettime();
	xEvent* event = (xEvent*)malloc(sizeof(xEvent));
	if (event == NULL) throw "out of memory";
	event->obj = NULL;
	event->func = func;
	event->args = args;
	event->from = now;
	event->date = now + delay;
	events_to_add.add(event);
}

void xEventLoop::callback(xCallbackable* obj, xCallbackFunc_o func, xMap* args, unsigned long delay)
{
	long now = gettime();
	xEvent* event = (xEvent*)malloc(sizeof(xEvent));
	if (event == NULL) throw "out of memory";
	event->obj = obj;
	event->func_o = func;
	event->args = args;
	event->from = now;
	event->date = now + delay;
	events_to_add.add(event);
}

void xEventLoop::start(void)
{
	bstop = false;
	brunning = true;

	unsigned long timestamp = gettime();
	_sleep(target_dt);

	while (true)
	{
		unsigned long now = gettime();
		unsigned long dt = now - timestamp;
		timestamp = now;

		for (int i = 0; i < tick_handlers.size(); i++)
		{
			xTickHandler* handler = (xTickHandler*)tick_handlers.getp(i);
			if (handler->obj != NULL)
				CALL_MEMBERFUNC_P(handler->obj, handler->func_o) (dt);
			else
				(handler->func) (dt);

			if (bstop)
			{
				brunning = false;
				return;
			}
		}

		xEvent* event = (xEvent*)events.getp(0);
		now = gettime();
		while (event != NULL && event->date <= now)
		{
			events.remove(0);

			if (event->obj != NULL)
				CALL_MEMBERFUNC_P(event->obj, event->func_o) (now - event->from, event->args);
			else
				(event->func) (now - event->from, event->args);
			
			free(event);

			if (bstop)
			{
				brunning = false;
				return;
			}

			event = (xEvent*)events.getp(0);
			now = gettime();
		}

		for (int i = 0; i < events_to_add.size(); i++)
			_insert((xEvent*)events_to_add.getp(i), events, 0, events.size() - 1);
		events_to_add.empty();

		if (timestamp + target_dt > now)
			_sleep(timestamp + target_dt - now);
	}
	brunning = false;
}

void xEventLoop::stop(bool clear_cbs)
{
	instance().bstop = true;
	if (clear_cbs)
		clear();
}

void xEventLoop::clear(void)
{
	for (int i = 0; i < events.size(); i++)
		free((xEvent*)events.getp(i));
	events.empty();
	for (int i = 0; i < events_to_add.size(); i++)
		free((xEvent*)events_to_add.getp(i));
	events_to_add.empty();
}

unsigned long xEventLoop::gettime(void)
{
	xEventLoop& loop = instance();
	static LARGE_INTEGER counts;
	if (!loop.perfcounter_init)
	{
		QueryPerformanceFrequency(&loop.counts_per_sec);
		loop.perfcounter_init = true;
	}
	QueryPerformanceCounter(&counts);
	return (unsigned long)((counts.QuadPart * 1000) / loop.counts_per_sec.QuadPart);
}


//================================================================
// Private Functions
//================================================================

// precise sleep taken from SFML
#pragma comment(lib, "Winmm.lib")
void xEventLoop::_sleep(unsigned long ms)
{
	if (ms <= 0) return;
	// Get the supported timer resolutions on this system
	TIMECAPS tc;
	timeGetDevCaps(&tc, sizeof(TIMECAPS));
	// Set the timer resolution to the minimum for the Sleep call
	timeBeginPeriod(tc.wPeriodMin);
	// Wait...

	// Sleep seems to get woken up too early sometimes...
	unsigned long timestamp = gettime();
	unsigned long elapsed = 0;
	while (elapsed < ms)
	{
		Sleep(ms - elapsed);
		elapsed = gettime() - timestamp;
	}

	// Reset the timer resolution back to the system default
	timeEndPeriod(tc.wPeriodMin);
}

void xEventLoop::_insert(xEvent* e, xArray& arr, int lb, int hb)
{
	if (hb - lb < 0)
		arr.add(e);
	else if (hb - lb == 0)
	{
		if (e->date <= ((xEvent*)arr.getp(lb))->date)
			arr.insert(e, lb);
		else
			arr.insert(e, hb + 1);
	}
	else
	{
		xEvent* m = (xEvent*)arr.getp((int)floor((lb + hb) / 2));
		if (e->date <= m->date)
			_insert(e, arr, lb, (int)floor((lb + hb) / 2));
		else
			_insert(e, arr, (int)floor(1 + (lb + hb) / 2), hb);
	}
}


//================================================================
// Destructors
//================================================================

xEventLoop::~xEventLoop()
{
	clear();
	for (int i = 0; i < tick_handlers.size(); i++)
		free((xTickHandler*)tick_handlers.getp(i));
	tick_handlers.empty();
}
