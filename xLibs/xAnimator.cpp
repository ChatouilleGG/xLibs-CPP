#include "stdafx.h"
#include "xAnimator.h"

//================================================================
// Constructors
//================================================================

const double xAnimator::EASE_IN_FACTOR = 1.8;
const double xAnimator::EASE_OUT_FACTOR = 0.5;

xMap xAnimator::thread_animator;

xAnimator::xAnimator()
{
	xEventLoop::instance().add_tick(GEN_TICK_MEMBERFUNC_P(this, xAnimator, _tick));
}

xAnimator& xAnimator::instance(void)
{
	xString id("%x", GetCurrentThreadId());
	xAnimator* animator = (xAnimator*)xAnimator::thread_animator.getp(id);
	if (animator == NULL)
	{
		animator = new xAnimator();
		xAnimator::thread_animator.put(id, animator, VAL_PTR);
	}
	return *animator;
}

xAnim::xAnim(double value)
{
	val = value;
	ival = (int)round(value);
	anim = NULL;
}


//================================================================
// Public Functions
//================================================================

//==================== anim params generation ====================

xAnimParams* xAnim::makeParams(double target, unsigned long dur, xAnimStepFunc step, xAnimCallback cb)
{
	xAnimParams* params = (xAnimParams*)malloc(sizeof(xAnimParams));
	if (params == NULL) throw "out of memory";
	params->var = this;
	params->start = val;
	params->target = target;
	params->total_dur = dur;
	params->rem_dur = dur;
	params->obj_step = NULL;
	params->step_func = step;
	params->obj_cb = NULL;
	params->callback = cb;
	return params;
}

xAnimParams* xAnim::makeParams(double target, unsigned long dur, xAnimStepFunc step, xCallbackable* obj_cb, xAnimCallback_o cb_o)
{
	xAnimParams* params = (xAnimParams*)malloc(sizeof(xAnimParams));
	if (params == NULL) throw "out of memory";
	params->var = this;
	params->start = val;
	params->target = target;
	params->total_dur = dur;
	params->rem_dur = dur;
	params->obj_step = NULL;
	params->step_func = step;
	params->obj_cb = obj_cb;
	params->callback_o = cb_o;
	return params;
}

xAnimParams* xAnim::makeParams(double target, unsigned long dur, xCallbackable* obj_step, xAnimStepFunc_o step_o, xAnimCallback cb)
{
	xAnimParams* params = (xAnimParams*)malloc(sizeof(xAnimParams));
	if (params == NULL) throw "out of memory";
	params->var = this;
	params->start = val;
	params->target = target;
	params->total_dur = dur;
	params->rem_dur = dur;
	params->obj_step = obj_step;
	params->step_func_o = step_o;
	params->obj_cb = NULL;
	params->callback = cb;
	return params;
}

xAnimParams* xAnim::makeParams(double target, unsigned long dur, xCallbackable* obj_step, xAnimStepFunc_o step_o, xCallbackable* obj_cb, xAnimCallback_o cb_o)
{
	xAnimParams* params = (xAnimParams*)malloc(sizeof(xAnimParams));
	if (params == NULL) throw "out of memory";
	params->var = this;
	params->start = val;
	params->target = target;
	params->total_dur = dur;
	params->rem_dur = dur;
	params->obj_step = obj_step;
	params->step_func_o = step_o;
	params->obj_cb = obj_cb;
	params->callback_o = cb_o;
	return params;
}

//==================== animation ====================

void xAnimator::set(xAnim& var, double val, bool continues)
{
	var.val = val;
	var.ival = (int)round(val);
	// stop current anim and clear queue
	if (!continues)
	{
		if (var.anim != NULL)
		{
			for (int i = 0; i < animations.size(); i++)
			{
				if (animations.getp(i) == (void*)(var.anim))
				{
					animations.remove(i);
					break;
				}
			}
			free(var.anim);
			var.anim = NULL;
		}
		if (var.animqueue.size() > 0)
		{
			for (int i = 0; i < var.animqueue.size(); i++)
				free((xAnimParams*)var.animqueue.getp(i));
			var.animqueue.empty();
		}
	}
	// update current anim
	else if (var.anim != NULL)
		var.anim->start = var.val;
}

void xAnimator::animate(xAnim& var, xAnimParams* params)
{
	// clear queue
	if (var.animqueue.size() > 0)
	{
		for (int i = 0; i < var.animqueue.size(); i++)
			free((xAnimParams*)var.animqueue.getp(i));
		var.animqueue.empty();
	}
	// set new anim
	if (var.anim != NULL && params != NULL)
		memcpy_s(var.anim, sizeof(xAnimParams), params, sizeof(xAnimParams));
	else
	{
		var.anim = params;
		if ( params != NULL )
			animations.add(var.anim);
	}
}

void xAnimator::queue(xAnim& var, xAnimParams* params)
{
	if (params == NULL) return;

	if (var.anim == NULL)
	{
		var.anim = params;
		var.anim->start = var.val;
		animations.add(var.anim);
	}
	else
		var.animqueue.add(params);
}


//================================================================
// Private Functions
//================================================================

void xAnimator::_tick(unsigned long dt)
{
	for (int i = 0; i < animations.size(); i++)
	{
		xAnimParams* params = (xAnimParams*)animations.getp(i);
		xAnim* var = params->var;

		if (dt >= params->rem_dur)
		{
			animations.remove(i);
			i--;
			var->anim = NULL;

			if (params->obj_step != NULL)
				var->val = params->start + (params->target - params->start) * CALL_MEMBERFUNC_P(params->obj_step, params->step_func_o) (1.0);
			else
				var->val = params->start + (params->target - params->start) * params->step_func(1.0);

			var->ival = (int)round(var->val);
			if (params->obj_cb != NULL)
			{
				if (params->callback_o != NULL)
					CALL_MEMBERFUNC_P(params->obj_cb, params->callback_o)(var);
			}
			else if (params->callback != NULL)
				params->callback(var);

			// user could delete var and/or params in the callback function
			if ( params != NULL )
				free(params);
			if (var != NULL)
			{
				if (var->anim == NULL && var->animqueue.size() > 0)
				{
					params = (xAnimParams*)var->animqueue.getp(0);
					var->animqueue.remove(0);
					var->anim = params;
					animations.add(var->anim);
				}
			}
			return;
		}

		if (params->obj_step != NULL)
			var->val = params->start + (params->target - params->start) * CALL_MEMBERFUNC_P(params->obj_step, params->step_func_o) (1.0 - ((double)params->rem_dur / (double)params->total_dur));
		else
			var->val = params->start + (params->target - params->start) * params->step_func(1.0 - ((double)params->rem_dur / (double)params->total_dur));

		var->ival = (int)round(var->val);
		params->rem_dur -= dt;

	}
}


//================================================================
// Destructors
//================================================================

xAnim::~xAnim()
{
	if (anim != NULL)
		free(anim);
	for (int i = 0; i < animqueue.size(); i++)
		free((xAnimParams*)animqueue.getp(i));
	animqueue.empty();
}

xAnimator::~xAnimator()
{
	xEventLoop::instance().remove_tick(GEN_TICK_MEMBERFUNC_P(this, xAnimator, _tick));
	for (int i = 0; i < animations.size(); i++)
		free((xAnimParams*)animations.getp(i));
	animations.empty();
}
