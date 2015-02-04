#include "stdafx.h"
#include "CppUnitTest.h"

#include "xAsync.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define TEST_METHOD2(name) TEST_METHOD(xAsync__##name)

namespace xLibs_test
{

	class AsyncTestClass : public xCallbackable
	{
	public:
		AsyncTestClass() {}
		~AsyncTestClass() {}

		static unsigned long time;
		static void static_tick(unsigned long dt)
		{
			time += dt;
		}

		static xMap* static_task(xMap* args)
		{
			Assert::IsNotNull(args);
			Assert::IsNotNull(args->get("tid"));
			Assert::AreNotEqual((int)GetCurrentThreadId(), args->geti("tid"));
			int s = args->geti("sleep");
			delete args;
			Sleep(s);
			return new xMap({
				new xMapPair("cid", (int)GetCurrentThreadId()),
			});
		}
		xMap* dyn_task(xMap* args)
		{
			Assert::IsNotNull(args);
			Assert::IsNotNull(args->get("tid"));
			Assert::AreNotEqual((int)GetCurrentThreadId(), args->geti("tid"));
			int s = args->geti("sleep");
			delete args;
			Sleep(s);
			return new xMap({
				new xMapPair("cid", (int)GetCurrentThreadId()),
			});
		}

		static void static_cb1(unsigned long dt, xMap* res)
		{
			Assert::IsNotNull(res);
			Assert::IsNotNull(res->get("cid"));
			Assert::AreNotEqual((int)GetCurrentThreadId(), res->geti("cid"));
			delete res;
			Assert::IsTrue(dt >= 160 && dt <= 260);
			xEventLoop::instance().stop(true);
		}
		static void static_cb_nostop(unsigned long dt, xMap* res)
		{
			Assert::IsNotNull(res);
			Assert::IsNotNull(res->get("cid"));
			Assert::AreNotEqual((int)GetCurrentThreadId(), res->geti("cid"));
			delete res;
			Assert::IsTrue(dt >= 160 && dt <= 260);
		}
		void dyn_cb(unsigned long dt, xMap* res)
		{
			Assert::IsNotNull(res);
			Assert::IsNotNull(res->get("cid"));
			Assert::AreNotEqual((int)GetCurrentThreadId(), res->geti("cid"));
			delete res;
			Assert::IsTrue(dt >= 360 && dt <= 460);
			xEventLoop::instance().stop(true);
		}
	};
	unsigned long AsyncTestClass::time;


	TEST_CLASS(xAsync_test)
	{
	public:

		xEventLoop& loop = xEventLoop::instance();
		
		TEST_METHOD2(static_task_static_cb)
		{
			AsyncTestClass::time = 0;

			xAsync::start(xAsync::makeTask(AsyncTestClass::static_task, new xMap({
				new xMapPair("tid", (int)GetCurrentThreadId()),
				new xMapPair("sleep", 200),
			}), AsyncTestClass::static_cb1));

			loop.add_tick(AsyncTestClass::static_tick);
			loop.start(5);
			loop.remove_tick(AsyncTestClass::static_tick);
		}

		TEST_METHOD2(static_task_dyn_cb)
		{
			AsyncTestClass::time = 0;
			AsyncTestClass obj;

			xAsync::start(xAsync::makeTask(AsyncTestClass::static_task, new xMap({
				new xMapPair("tid", (int)GetCurrentThreadId()),
				new xMapPair("sleep", 400),
			}), GEN_CALLBACK_MEMBERFUNC(obj, AsyncTestClass, dyn_cb)));

			loop.add_tick(AsyncTestClass::static_tick);
			loop.start(5);
			loop.remove_tick(AsyncTestClass::static_tick);
		}

		TEST_METHOD2(dyn_task_static_cb)
		{
			AsyncTestClass::time = 0;
			AsyncTestClass obj;

			xAsync::start(xAsync::makeTask(GEN_ASYNC_MEMBERFUNC(obj, AsyncTestClass, dyn_task), new xMap({
				new xMapPair("tid", (int)GetCurrentThreadId()),
				new xMapPair("sleep", 200),
			}), AsyncTestClass::static_cb1));

			loop.add_tick(AsyncTestClass::static_tick);
			loop.start(5);
			loop.remove_tick(AsyncTestClass::static_tick);
		}

		TEST_METHOD2(dyn_task_dyn_cb)
		{
			AsyncTestClass::time = 0;
			AsyncTestClass obj;

			xAsync::start(xAsync::makeTask(GEN_ASYNC_MEMBERFUNC(obj, AsyncTestClass, dyn_task), new xMap({
				new xMapPair("tid", (int)GetCurrentThreadId()),
				new xMapPair("sleep", 400),
			}), GEN_CALLBACK_MEMBERFUNC(obj, AsyncTestClass, dyn_cb)));

			loop.add_tick(AsyncTestClass::static_tick);
			loop.start(5);
			loop.remove_tick(AsyncTestClass::static_tick);
		}

		TEST_METHOD2(double_task)
		{
			AsyncTestClass::time = 0;
			AsyncTestClass obj;

			xAsync::start(xAsync::makeTask(AsyncTestClass::static_task, new xMap({
				new xMapPair("tid", (int)GetCurrentThreadId()),
				new xMapPair("sleep", 200),
			}), AsyncTestClass::static_cb_nostop));

			xAsync::start(xAsync::makeTask(AsyncTestClass::static_task, new xMap({
				new xMapPair("tid", (int)GetCurrentThreadId()),
				new xMapPair("sleep", 400),
			}), GEN_CALLBACK_MEMBERFUNC(obj, AsyncTestClass, dyn_cb)));

			loop.add_tick(AsyncTestClass::static_tick);
			loop.start(5);
			loop.remove_tick(AsyncTestClass::static_tick);
		}

	};
}