#include "stdafx.h"
#include "CppUnitTest.h"

#include "xArray.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define TEST_METHOD2(name) TEST_METHOD(xArray__##name)

namespace xLibs_test
{
	int custom_comp(const xArrayElem** a, const xArrayElem** b)
	{
		if ((*a)->type == VAL_INT) return ((*b)->type == VAL_INT ? 0 : -1);
		if ((*a)->type == VAL_DOUBLE) return ((*b)->type == VAL_INT ? 1 : ((*b)->type == VAL_DOUBLE ? 0 : -1));
		if ((*a)->type == VAL_STRING) return ((*b)->type == VAL_PTR ? -1 : ((*b)->type == VAL_STRING ? 0 : 1));
		if ((*a)->type == VAL_PTR) return ((*b)->type == VAL_PTR ? 0 : 1);
		return 0;
	}

	TEST_CLASS(xArray_test)
	{
	public:
		
		TEST_METHOD2(constructor_void)
		{
			xArray arr;
			Assert::AreEqual(0, arr.size());
		}

		TEST_METHOD2(set_inrange)
		{
			xArray arr;
			arr.set(0, 42);
			Assert::AreEqual(1, arr.size());
			arr.set(5, "pouet");
			Assert::AreEqual(6, arr.size());
			arr.set(9, 3.14);
			Assert::AreEqual(10, arr.size());
		}

		TEST_METHOD2(set_outrange)
		{
			xArray arr;
			try {
				arr.set(-1, 42);
				Assert::Fail();
			}
			catch (const char*) {}
			arr.set(12, "pouet");
			Assert::AreEqual(13, arr.size());
			arr.set(263, 3.14);
			Assert::AreEqual(264, arr.size());
		}

		TEST_METHOD2(get_valid)
		{
			xArray arr;
			arr.set(0, 42);
			Assert::AreEqual(42, arr.geti(0));
			arr.set(5, "pouet");
			Assert::IsTrue(arr.getstr(5).eq("pouet"));
			arr.set(9, 3.14);
			Assert::AreEqual(3.14, arr.getd(9));
			arr.set(6, &arr);
			Assert::IsTrue(arr.get(6)->type == VAL_ARRAY);
			Assert::AreEqual((void*)&arr, arr.getp(6));
			Assert::AreEqual((void*)&arr, (void*)arr.getarr(6));
			Assert::IsNull(arr.get(1));
			arr.set(1, this);
			Assert::IsTrue(arr.get(1)->type == VAL_PTR);
			arr.setp(1, NULL);
			Assert::IsTrue(arr.get(1)->type == VAL_PTR);
		}

		TEST_METHOD2(get_invalid)
		{
			xArray arr;
			Assert::IsNull(arr.get(5));
			Assert::AreEqual(0, arr.geti(5));
			arr.set(5, 42);
			Assert::AreEqual(0.0, arr.getd(5));
			Assert::IsTrue(arr.getstr(5).eq(""));
			Assert::IsNull(arr.getarr(5));
			Assert::IsNull(arr.getp(5));
			Assert::IsNull(arr.get(-1));
		}

		TEST_METHOD2(insert)
		{
			xArray arr;
			try {
				arr.insert(42, -1);
				Assert::Fail();
			}
			catch (const char*) {}
			arr.insert(42, 5);
			Assert::AreEqual(6, arr.size());
			Assert::AreEqual(42, arr.geti(5));
			arr.insert(41, 5);
			Assert::AreEqual(7, arr.size());
			Assert::AreEqual(41, arr.geti(5));
			Assert::AreEqual(42, arr.geti(6));
			arr.insert(100, 0);
			Assert::AreEqual(8, arr.size());
			Assert::AreEqual(100, arr.geti(0));
			Assert::AreEqual(41, arr.geti(6));
			Assert::AreEqual(42, arr.geti(7));
			arr.insert(12, 19);
			Assert::AreEqual(20, arr.size());
			Assert::AreEqual(12, arr.geti(19));
			Assert::AreEqual(42, arr.geti(7));
		}

		TEST_METHOD2(add)
		{
			xArray arr;
			for (int i = 0; i < 20; i++)
			{
				arr.add(20 - i);
				Assert::AreEqual(i + 1, arr.size());
				Assert::AreEqual(20 - i, arr.geti(arr.size() - 1));
			}
			arr.add(&arr);
			Assert::AreEqual(21, arr.size());
			Assert::AreEqual((void*)&arr, (void*)arr.getarr(20));
		}

		TEST_METHOD2(remove)
		{
			xArray arr;
			for (int i = 0; i < 20; i++)
				arr.add(i);
			Assert::AreEqual(20, arr.size());
			arr.remove(10);
			Assert::AreEqual(19, arr.size());
			Assert::AreEqual(9, arr.geti(9));
			Assert::AreEqual(11, arr.geti(10));
			Assert::AreEqual(19, arr.geti(18));
			arr.remove(0);
			Assert::AreEqual(18, arr.size());
			Assert::AreEqual(1, arr.geti(0));
			Assert::AreEqual(19, arr.geti(17));
			for (int i = 0; i < 17; i++)
				arr.remove(0);
			Assert::AreEqual(1, arr.size());
			Assert::AreEqual(19, arr.geti(0));
			arr.remove(1);
			Assert::AreEqual(1, arr.size());
			try {
				arr.remove(-1);
				Assert::Fail();
			}
			catch (const char*) {}
		}

		TEST_METHOD2(empty)
		{
			xArray arr;
			for (int i = 0; i < 20; i++)
				arr.add(i);
			Assert::AreEqual(20, arr.size());
			arr.empty();
			Assert::AreEqual(0, arr.size());
			for (int i = 0; i < 20; i++)
				Assert::IsNull(arr.get(i));
		}

		TEST_METHOD2(sort_int)
		{
			xArray arr;
			for (int i = 0; i < 20; i++)
				arr.add(19 - i);
			arr.insert("pouet", 10);
			Assert::AreEqual(20, arr.sorti());
			Assert::IsTrue(arr.getstr(20).eq("pouet"));
			for (int i = 0; i < 20; i++)
				Assert::AreEqual(i, arr.geti(i));
		}

		TEST_METHOD2(sort_double)
		{
			xArray arr;
			for (int i = 0; i < 15; i++)
				arr.add((double)rand() / (double)RAND_MAX);
			double d = arr.getd(0);
			arr.add(d + 0.1);
			arr.add(d + 0.00001);
			arr.add(d + 0.01);
			arr.add(d + 0.0001);
			arr.add(d + 0.001);
			Assert::AreEqual(20, arr.sortd());
			for (int i = 0; i < 19; i++)
				Assert::IsTrue(arr.getd(i) <= arr.getd(i + 1));
		}

		TEST_METHOD2(sort_string)
		{
			xArray arr;
			arr.add("pouet");
			arr.add("asdasd");
			arr.add("brouf");
			arr.add("zor");
			arr.add(42);
			arr.add(3.14);
			Assert::AreEqual(42, arr.geti(4));
			Assert::AreEqual(4, arr.sortstr());
			Assert::IsTrue(arr.getstr(0).eq("asdasd"));
			Assert::IsTrue(arr.getstr(1).eq("brouf"));
			Assert::IsTrue(arr.getstr(2).eq("pouet"));
			Assert::IsTrue(arr.getstr(3).eq("zor"));
		}

		TEST_METHOD2(sort_string_i)
		{
			xArray arr;
			arr.add("Pouet");
			arr.add("Asdasd");
			arr.add("brouf");
			arr.add("zor");
			arr.add(42);
			arr.add(3.14);
			Assert::AreEqual(4, arr.sortstr_i());
			Assert::IsTrue(arr.getstr(0).eq("Asdasd"));
			Assert::IsTrue(arr.getstr(1).eq("brouf"));
			Assert::IsTrue(arr.getstr(2).eq("Pouet"));
			Assert::IsTrue(arr.getstr(3).eq("zor"));
		}

		TEST_METHOD2(constructor_ints)
		{
			xArray arr({ 1, 2, 3, 7, 9, 5 });
			Assert::AreEqual(6, arr.size());
			Assert::AreEqual(1, arr.geti(0));
			Assert::AreEqual(2, arr.geti(1));
			Assert::AreEqual(3, arr.geti(2));
			Assert::AreEqual(7, arr.geti(3));
			Assert::AreEqual(9, arr.geti(4));
			Assert::AreEqual(5, arr.geti(5));
		}

		TEST_METHOD2(constructor_strings)
		{
			xArray arr({ "hurr", "durr", "pouet" });
			Assert::AreEqual(3, arr.size());
			Assert::IsTrue(arr.getstr(0).eq("hurr"));
			Assert::IsTrue(arr.getstr(1).eq("durr"));
			Assert::IsTrue(arr.getstr(2).eq("pouet"));
		}

		TEST_METHOD2(constructor_multi)
		{
			xArray arr = xARR(
				VAL_INT, 42,
				VAL_STRING, "pouet",
				VAL_DOUBLE, 3.14,
				VAL_STRING, "brouf",
				VAL_INT, 100,
				VAL_PTR, this
			);
			Assert::AreEqual(6, arr.size());
			Assert::AreEqual(42, arr.geti(0));
			Assert::IsTrue(arr.getstr(1).eq("pouet"));
			Assert::AreEqual(3.14, arr.getd(2));
			Assert::IsTrue(arr.getstr(3).eq("brouf"));
			Assert::AreEqual(100, arr.geti(4));
			Assert::AreEqual((void*)this, arr.getp(5));
		}

		TEST_METHOD2(add_ints)
		{
			xArray arr({ "pouet" });
			arr.add({ 1, 2, 3 });
			Assert::AreEqual(4, arr.size());
			Assert::IsTrue(arr.getstr(0).eq("pouet"));
			Assert::AreEqual(1, arr.geti(1));
			Assert::AreEqual(2, arr.geti(2));
			Assert::AreEqual(3, arr.geti(3));
		}

		TEST_METHOD2(add_multi)
		{
			xArray arr({ "pouet" });
			arr.add(42, 3.14, "brouf");
			Assert::AreEqual(4, arr.size());
			Assert::IsTrue(arr.getstr(0).eq("pouet"));
			Assert::AreEqual(42, arr.geti(1));
			Assert::AreEqual(3.14, arr.getd(2));
			Assert::IsTrue(arr.getstr(3).eq("brouf"));
		}

		TEST_METHOD2(sort_custom)
		{
			xArray arr = xARR(
				VAL_INT, 42,
				VAL_STRING, "pouet",
				VAL_DOUBLE, 3.14,
				VAL_STRING, "brouf",
				VAL_INT, 100,
				VAL_PTR, this
			);
			arr.insert(3);
			Assert::AreEqual(6, arr.sort(custom_comp));
			Assert::IsTrue(arr.get(0)->type == VAL_INT);
			Assert::IsTrue(arr.get(1)->type == VAL_INT);
			Assert::IsTrue(arr.get(2)->type == VAL_DOUBLE);
			Assert::IsTrue(arr.get(3)->type == VAL_STRING);
			Assert::IsTrue(arr.get(4)->type == VAL_STRING);
			Assert::IsTrue(arr.get(5)->type == VAL_PTR);

			Assert::AreEqual(0, arr.sort(NULL));
		}

	};
}
