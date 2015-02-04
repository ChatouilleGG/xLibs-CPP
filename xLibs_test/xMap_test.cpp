#include "stdafx.h"
#include "CppUnitTest.h"

#include "xMap.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define TEST_METHOD2(name) TEST_METHOD(xMap__##name)

namespace xLibs_test
{
	TEST_CLASS(xMap_test)
	{
	public:
		
		TEST_METHOD2(constructor_void)
		{
			xMap map;
		}

		TEST_METHOD2(put)
		{
			xMap map;
			map.put(new xMapPair("pouet", 42));
			map.put("hurr", "durr");
			map.put("pi", 3.14);
			map.put("this", &map);
		}

		TEST_METHOD2(get)
		{
			xMap map;
			map.put(new xMapPair("pouet", 42));
			map.put("hurr", "durr");
			map.put("pi", 3.14);
			map.put("this", &map);
			map.put("null", NULL, VAL_PTR);
			Assert::IsNotNull(map.get("pouet"));
			Assert::AreEqual(42, map.geti("pouet"));
			Assert::IsTrue(map.getstr("hurr").eq("durr"));
			Assert::AreEqual(3.14, map.getd("pi"));
			Assert::AreEqual((void*)&map, (void*)map.getmap("this"));
			Assert::IsNull(map.get("brouf"));
			map.put("null", NULL, VAL_PTR);
			Assert::IsTrue(map.get("null")->type == VAL_PTR);
		}

		TEST_METHOD2(put_list)
		{
			xMap map;
			map.put("pouet", 42);
			map.put({
				new xMapPair("hurr", "durr"),
				new xMapPair("pi", 3.14),
				new xMapPair("this", &map),
			});
			Assert::AreEqual(42, map.geti("pouet"));
			Assert::IsTrue(map.getstr("hurr").eq("durr"));
			Assert::AreEqual(3.14, map.getd("pi"));
			Assert::AreEqual((void*)&map, (void*)map.getmap("this"));
		}

		TEST_METHOD2(constructor_list)
		{
			xMap map({
				new xMapPair("pouet", 42),
				new xMapPair("hurr", "durr"),
				new xMapPair("pi", 3.14)
			});
			Assert::AreEqual(42, map.geti("pouet"));
			Assert::IsTrue(map.getstr("hurr").eq("durr"));
			Assert::AreEqual(3.14, map.getd("pi"));
		}

		TEST_METHOD2(constructor_multi)
		{
			xMap map(
				"pouet", VAL_INT, 42,
				"hurr", VAL_STRING, "durr",
				"pi", VAL_DOUBLE, 3.14
			);
			Assert::AreEqual(42, map.geti("pouet"));
			Assert::IsTrue(map.getstr("hurr").eq("durr"));
			Assert::AreEqual(3.14, map.getd("pi"));
		}

		TEST_METHOD2(resize)
		{
			xMap map({
				new xMapPair("pouet", 42),
				new xMapPair("hurr", "durr"),
				new xMapPair("pi", 3.14)
			});
			for (int i = 0; i < 50; i++)
				map.put(xString("%02i", i), i);
			Assert::IsNotNull(map.get("pouet"));
			Assert::IsNotNull(map.get("hurr"));
			Assert::IsNotNull(map.get("pi"));
			for (int i = 0; i < 50; i++)
				Assert::IsNotNull(map.get(xString("%02i", i)));
		}

		TEST_METHOD2(remove)
		{
			xMap map({
				new xMapPair("pouet", 42),
				new xMapPair("hurr", "durr"),
				new xMapPair("pi", 3.14)
			});
			map.remove("hurr");
			Assert::IsNull(map.get("hurr"));
			Assert::IsTrue(map.getstr("hurr").eq(""));
			for (int i = 0; i < 50; i++)
				map.put(xString("%02i", i), i);
			for (int i = 0; i < 50; i += 2)
				map.remove(xString("%02i", i));
			for (int i = 0; i < 50; i++)
			{
				if ((i % 2) == 0)
					Assert::IsNull(map.get(xString("%02i", i)));
				else
					Assert::IsNotNull(map.get(xString("%02i", i)));
			}
		}

		TEST_METHOD2(empty)
		{
			xMap map({
				new xMapPair("pouet", 42),
				new xMapPair("hurr", "durr"),
				new xMapPair("pi", 3.14),
			});
			for (int i = 0; i < 50; i++)
				map.put(xString("%i", i), i);

			map.empty();
			Assert::IsNull(map.get("pouet"));
			Assert::IsNull(map.get("hurr"));
			Assert::IsNull(map.get("pi"));
			for (int i = 0; i < 50; i++ )
				Assert::IsNull(map.get(xString("%i", i)));
		}

		TEST_METHOD2(iterate_while)
		{
			xMap map({
				new xMapPair("pouet", 42),
				new xMapPair("hurr", "durr"),
				new xMapPair("pi", 3.14),
				new xMapPair("brouf", "zor"),
				new xMapPair("null", NULL, VAL_PTR),
			});
			int count = 0;
			map.start();
			while (map.hasnext())
			{
				xMapPair* pair = map.next();
				Assert::IsNotNull(pair);
				count++;
			}
			Assert::AreEqual(5, count);
		}

		TEST_METHOD2(iterate_for)
		{
			xMap map({
				new xMapPair("pouet", 42),
				new xMapPair("hurr", "durr"),
				new xMapPair("pi", 3.14),
				new xMapPair("brouf", "zor"),
				new xMapPair("null", NULL, VAL_PTR),
			});
			int count = 0;
			for (xMapPair* p = map.first(); p != NULL; p = map.next())
			{
				Assert::IsNotNull(p);
				count++;
			}
			Assert::AreEqual(5, count);
		}

	};
}