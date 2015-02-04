#include "stdafx.h"
#include "CppUnitTest.h"

#include "xEventLoop.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define TEST_METHOD2(name) TEST_METHOD(xEventLoop__##name)

namespace xLibs_test
{

	class LoopTestClass : public xCallbackable
	{
	public:
		LoopTestClass() {}
		~LoopTestClass() {}

		static int count;
		static void static_tick(unsigned long dt)
		{
			unsigned long expected_dt = xEventLoop::instance().get_dt();
			Assert::IsTrue((double)dt >= (double)expected_dt*0.9 && dt <= (double)expected_dt*1.2);
			LoopTestClass::count++;
			if (LoopTestClass::count == 10) xEventLoop::instance().stop(true);
		}
		static void static_cb(unsigned long dt, xMap* args)
		{
			Assert::IsNotNull(args);
			unsigned long expected_dt = (unsigned long)args->geti("expected_dt");
			Assert::IsTrue((double)dt >= (double)expected_dt*0.9 && dt <= (double)expected_dt*1.2);
			delete args;
		}
		static void static_stop(unsigned long dt, xMap* args)
		{
			Assert::IsNull(args);
			xEventLoop::instance().stop(true);
		}

		int dyn_count = 0;
		void dyn_tick(unsigned long dt)
		{
			unsigned long expected_dt = xEventLoop::instance().get_dt();
			Assert::IsTrue((double)dt >= (double)expected_dt*0.9 && dt <= (double)expected_dt*1.2);
			dyn_count++;
			if (dyn_count == 10) xEventLoop::instance().stop(true);
		}
		void dyn_cb(unsigned long dt, xMap* args)
		{
			Assert::IsNotNull(args);
			unsigned long expected_dt = (unsigned long)args->geti("expected_dt");
			Assert::IsTrue((double)dt >= (double)expected_dt*0.9 && dt <= (double)expected_dt*1.2);
			delete args;
		}
		void dyn_stop(unsigned long dt, xMap* args)
		{
			Assert::IsNull(args);
			xEventLoop::instance().stop(true);
		}
	};
	int LoopTestClass::count = 0;


	TEST_CLASS(xEventLoop_test)
	{
	public:

		xEventLoop& loop = xEventLoop::instance();
		
		TEST_METHOD2(static_tick)
		{
			loop.add_tick(LoopTestClass::static_tick);
			LoopTestClass::count = 0;
			loop.start(50);
			loop.remove_tick(LoopTestClass::static_tick);
			Assert::AreEqual(10, LoopTestClass::count);
		}

		TEST_METHOD2(static_callback)
		{
			loop.callback(LoopTestClass::static_cb, new xMap({
				new xMapPair("expected_dt", 200),
			}), 200);
			loop.callback(LoopTestClass::static_cb, new xMap({
				new xMapPair("expected_dt", 150),
			}), 150);
			loop.callback(LoopTestClass::static_cb, new xMap({
				new xMapPair("expected_dt", 300),
			}), 300);
			loop.callback(LoopTestClass::static_cb, new xMap({
				new xMapPair("expected_dt", 250),
			}), 250);
			loop.callback(LoopTestClass::static_stop, NULL, 400);
			loop.start(1);
		}

		TEST_METHOD2(dynamic_tick)
		{
			LoopTestClass obj;
			loop.add_tick(GEN_TICK_MEMBERFUNC(obj, LoopTestClass, dyn_tick));
			loop.start(50);
			loop.remove_tick(GEN_TICK_MEMBERFUNC(obj, LoopTestClass, dyn_tick));
			Assert::AreEqual(10, obj.dyn_count);
		}

		TEST_METHOD2(dynamic_callback)
		{
			LoopTestClass obj;
			loop.callback(GEN_CALLBACK_MEMBERFUNC(obj, LoopTestClass, dyn_cb), new xMap({
				new xMapPair("expected_dt", 200),
			}), 200);
			loop.callback(GEN_CALLBACK_MEMBERFUNC(obj, LoopTestClass, dyn_cb), new xMap({
				new xMapPair("expected_dt", 150),
			}), 150);
			loop.callback(GEN_CALLBACK_MEMBERFUNC(obj, LoopTestClass, dyn_cb), new xMap({
				new xMapPair("expected_dt", 300),
			}), 300);
			loop.callback(GEN_CALLBACK_MEMBERFUNC(obj, LoopTestClass, dyn_cb), new xMap({
				new xMapPair("expected_dt", 250),
			}), 250);
			loop.callback(GEN_CALLBACK_MEMBERFUNC(obj, LoopTestClass, dyn_stop), NULL, 400);
			loop.start(1);
		}

	};
}