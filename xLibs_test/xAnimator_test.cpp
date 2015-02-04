#include "stdafx.h"
#include "CppUnitTest.h"

#include "xAnimator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define TEST_METHOD2(name) TEST_METHOD(xAnimator__##name)

namespace xLibs_test
{

	class AnimTestClass : public xCallbackable
	{
	public:
		AnimTestClass() {}
		~AnimTestClass() {}

		static double last_pct;
		static xAnim* var;
		static double last_val;
		double last_pct2;
		static unsigned long time;

		static void reset(xAnim* v)
		{
			last_pct = 0.0;
			var = v;
			last_val = 0.0;
			time = 0;
		}

		static double static_step(double pct_time)
		{
			Assert::IsTrue(pct_time >= AnimTestClass::last_pct);
			AnimTestClass::last_pct = pct_time;
			return pct_time;
		}
		double dyn_step(double pct_time)
		{
			Assert::IsTrue(pct_time >= last_pct2);
			last_pct2 = pct_time;
			return pct_time;
		}

		static void static_cb(xAnim* var)
		{
			Assert::IsNotNull(var);
			Assert::AreEqual((void*)var, (void*)AnimTestClass::var);
			Assert::IsTrue(var->get() == 100.0);
			Assert::IsTrue(AnimTestClass::time >= 390 && AnimTestClass::time <= 420);
		}
		void dyn_cb(xAnim* var)
		{
			Assert::IsNotNull(var);
			Assert::AreEqual((void*)var, (void*)AnimTestClass::var);
			Assert::IsTrue(abs(100.0 - var->get()) <= 0.000001 );
			Assert::IsTrue(AnimTestClass::time >= 390 && AnimTestClass::time <= 420);
		}
		static void wait_cb(xAnim* var)
		{
			Assert::IsNotNull(var);
			Assert::AreEqual((void*)var, (void*)AnimTestClass::var);
			Assert::IsTrue(var->get() == 0.0);
			Assert::IsTrue(AnimTestClass::time >= 390 && AnimTestClass::time <= 420);
		}

		static void static_tick(unsigned long dt)
		{
			Assert::IsNotNull(AnimTestClass::var);
			Assert::IsTrue(AnimTestClass::var->get() >= AnimTestClass::last_val);
			Assert::IsTrue(AnimTestClass::var->get() >= 0.0 && AnimTestClass::var->get() <= 100.0);
			AnimTestClass::time += dt;
		}
		static void stopit(unsigned long dt, xMap* args)
		{
			xEventLoop::instance().stop(true);
		}
	};
	double AnimTestClass::last_pct = 0.0;
	xAnim* AnimTestClass::var = NULL;
	double AnimTestClass::last_val = 0.0;
	unsigned long AnimTestClass::time = 0;


	TEST_CLASS(xAnimator_test)
	{
	public:

		xEventLoop& loop = xEventLoop::instance();

		TEST_METHOD2(linear_no_cb)
		{
			xAnim var(0.0);
			AnimTestClass::reset(&var);
			var.animate(var.makeParams(100.0, 400, xAnim::LINEAR));
			loop.add_tick(AnimTestClass::static_tick);
			loop.callback(AnimTestClass::stopit, NULL, 500);
			loop.start(5);
			loop.remove_tick(AnimTestClass::static_tick);
		}
		
		TEST_METHOD2(easein_static_cb)
		{
			xAnim var(0.0);
			AnimTestClass::reset(&var);
			var.animate(var.makeParams(100.0, 400, xAnim::EASE_IN, AnimTestClass::static_cb));
			loop.clear();
			loop.add_tick(AnimTestClass::static_tick);
			loop.callback(AnimTestClass::stopit, NULL, 500);
			loop.start(5);
			loop.remove_tick(AnimTestClass::static_tick);
		}

		TEST_METHOD2(easeout_dyn_cb)
		{
			xAnim var(0.0);
			AnimTestClass::reset(&var);
			AnimTestClass obj;
			var.animate(var.makeParams(100.0, 400, xAnim::EASE_OUT, GEN_ANIMCB_MEMBERFUNC(obj,AnimTestClass,dyn_cb)));
			loop.clear();
			loop.add_tick(AnimTestClass::static_tick);
			loop.callback(AnimTestClass::stopit, NULL, 500);
			loop.start(5);
			loop.remove_tick(AnimTestClass::static_tick);
		}

		TEST_METHOD2(easeinout_static_cb)
		{
			xAnim var(0.0);
			AnimTestClass::reset(&var);
			var.animate(var.makeParams(100.0, 400, xAnim::EASE_IN_OUT, AnimTestClass::static_cb));
			loop.clear();
			loop.add_tick(AnimTestClass::static_tick);
			loop.callback(AnimTestClass::stopit, NULL, 500);
			loop.start(5);
			loop.remove_tick(AnimTestClass::static_tick);
		}

		TEST_METHOD2(wait_wait_cb)
		{
			xAnim var(0.0);
			AnimTestClass::reset(&var);
			var.animate(var.makeParams(100.0, 400, xAnim::WAIT, AnimTestClass::wait_cb));
			loop.clear();
			loop.add_tick(AnimTestClass::static_tick);
			loop.callback(AnimTestClass::stopit, NULL, 500);
			loop.start(5);
			loop.remove_tick(AnimTestClass::static_tick);
		}

		TEST_METHOD2(static_step_static_cb)
		{
			xAnim var(0.0);
			AnimTestClass::reset(&var);
			var.animate(var.makeParams(100.0, 400, AnimTestClass::static_step, AnimTestClass::static_cb));
			loop.clear();
			loop.add_tick(AnimTestClass::static_tick);
			loop.callback(AnimTestClass::stopit, NULL, 500);
			loop.start(5);
			loop.remove_tick(AnimTestClass::static_tick);
		}

		TEST_METHOD2(dyn_step_static_cb)
		{
			xAnim var(0.0);
			AnimTestClass::reset(&var);
			AnimTestClass obj;
			var.animate(var.makeParams(100.0, 400, GEN_STEP_MEMBERFUNC(obj, AnimTestClass, dyn_step), AnimTestClass::static_cb));
			loop.clear();
			loop.add_tick(AnimTestClass::static_tick);
			loop.callback(AnimTestClass::stopit, NULL, 500);
			loop.start(5);
			loop.remove_tick(AnimTestClass::static_tick);
		}

		TEST_METHOD2(static_step_dyn_cb)
		{
			xAnim var(0.0);
			AnimTestClass::reset(&var);
			AnimTestClass obj;
			var.animate(var.makeParams(100.0, 400, AnimTestClass::static_step, GEN_ANIMCB_MEMBERFUNC(obj, AnimTestClass, dyn_cb)));
			loop.clear();
			loop.add_tick(AnimTestClass::static_tick);
			loop.callback(AnimTestClass::stopit, NULL, 500);
			loop.start(5);
			loop.remove_tick(AnimTestClass::static_tick);
		}

		TEST_METHOD2(dyn_step_dyn_cb)
		{
			xAnim var(0.0);
			AnimTestClass::reset(&var);
			AnimTestClass obj;
			var.animate(var.makeParams(100.0, 400, GEN_STEP_MEMBERFUNC(obj, AnimTestClass, dyn_step), GEN_ANIMCB_MEMBERFUNC(obj, AnimTestClass, dyn_cb)));
			loop.clear();
			loop.add_tick(AnimTestClass::static_tick);
			loop.callback(AnimTestClass::stopit, NULL, 500);
			loop.start(5);
			loop.remove_tick(AnimTestClass::static_tick);
		}

	};
}