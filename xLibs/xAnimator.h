#pragma once

#include "xEventLoop.h"

#ifndef PI
	#define PI 3.14159265359
#endif
#ifndef PI_DIV_2
	#define PI_DIV_2 1.57079632679
#endif

class xAnim;

typedef double(*xAnimStepFunc)(double);
typedef void(*xAnimCallback)(xAnim*);

typedef double(xCallbackable::*xAnimStepFunc_o)(double);
typedef void(xCallbackable::*xAnimCallback_o)(xAnim*);

#define GEN_STEP_MEMBERFUNC(object, classname, funcname) (xCallbackable*)&object, (xAnimStepFunc_o)(&classname::funcname)
#define GEN_STEP_MEMBERFUNC_P(objptr, classname, funcname) (xCallbackable*)objptr, (xAnimStepFunc_o)(&classname::funcname)

#define GEN_ANIMCB_MEMBERFUNC(object, classname, funcname) (xCallbackable*)&object, (xAnimCallback_o)(&classname::funcname)
#define GEN_ANIMCB_MEMBERFUNC_P(objptr, classname, funcname) (xCallbackable*)objptr, (xAnimCallback_o)(&classname::funcname)

typedef struct
{
	xAnim* var;

	double start;
	double target;
	unsigned long total_dur;
	unsigned long rem_dur;

	xCallbackable* obj_step;
	union {
		xAnimStepFunc step_func;
		xAnimStepFunc_o step_func_o;
	};

	xCallbackable* obj_cb;
	union {
		xAnimCallback callback;
		xAnimCallback_o callback_o;
	};
} xAnimParams;


class xAnimator : public xCallbackable
{

public:
	static xAnimator& instance(void);

	void set(xAnim&, double, bool continues = false);
	void animate(xAnim&, xAnimParams*);
	void queue(xAnim&, xAnimParams*);

private:
	static xMap thread_animator;
	xArray animations;

	xAnimator();
	~xAnimator();

	void _tick(unsigned long);

	static const double EASE_IN_FACTOR;
	static const double EASE_OUT_FACTOR;

};


class xAnim
{
	friend class xAnimator;

public:
	xAnim(double);
	~xAnim();

	inline double get(void) { return val; }
	inline int geti(void) { return ival; }

	inline void set(double value, bool continues = false) { xAnimator::instance().set(*this, value, continues); }
	inline void animate(xAnimParams* params) { xAnimator::instance().animate(*this, params); }
	inline void queue(xAnimParams* params) { xAnimator::instance().queue(*this, params); }

	xAnimParams* makeParams(double, unsigned long, xAnimStepFunc, xAnimCallback = NULL);
	xAnimParams* makeParams(double, unsigned long, xAnimStepFunc, xCallbackable*, xAnimCallback_o = NULL);
	xAnimParams* makeParams(double, unsigned long, xCallbackable*, xAnimStepFunc_o, xAnimCallback = NULL);
	xAnimParams* makeParams(double, unsigned long, xCallbackable*, xAnimStepFunc_o, xCallbackable* = NULL, xAnimCallback_o = NULL);

	static inline double WAIT(double pct_time) { return 0.0; }
	static inline double LINEAR(double pct_time) { return pct_time; }
	static inline double EASE_IN(double pct_time) { return sin(pct_time*PI_DIV_2); }
	static inline double EASE_OUT(double pct_time) { return (1.0 - cos(pct_time*PI_DIV_2)); }
	static inline double EASE_IN_OUT(double pct_time)
	{
		return pct_time*sin(pct_time*PI_DIV_2) + (1.0 - pct_time)*(1.0 - cos(pct_time*PI_DIV_2));
	}
	
private:
	double val;
	int ival;
	xAnimParams* anim;
	xArray animqueue;
};
