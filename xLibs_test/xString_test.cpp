#include "stdafx.h"
#include "CppUnitTest.h"

#include "xString.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define TEST_METHOD2(name) TEST_METHOD(xString__##name)

namespace xLibs_test
{
	TEST_CLASS(xString_test)
	{
	public:
		
		TEST_METHOD2(constructor_void)
		{
			xString str = xString();
			Assert::AreEqual(0, str.length());
		}

		TEST_METHOD2(constructor_str)
		{
			xString str = xString("hello!");
			Assert::AreEqual(6, str.length());
		}

		TEST_METHOD2(autocast_char)
		{
			xString str = xString("hello!");
			Assert::AreEqual(0, strcmp(str, "hello!"));
		}

		TEST_METHOD2(constructor_fmt1)
		{
			xString str = xString("six %i", 6);
			Assert::AreEqual(5, str.length());
			Assert::AreEqual(0, strcmp(str, "six 6"));
		}

		TEST_METHOD2(constructor_fmt2)
		{
			xString str = xString("ohundred %04i say %s", 100, "hello");
			Assert::AreEqual(23, str.length());
			Assert::AreEqual(0, strcmp(str, "ohundred 0100 say hello"));
		}

		TEST_METHOD2(equals)
		{
			xString str1 = xString("hello!");
			xString str2 = xString("hello!");
			xString str3 = xString("Hello!");
			Assert::IsFalse((str1 == str2) || (str1 == str3) || (str2 == str3));
			Assert::IsTrue(str1.eq(str2));
			Assert::IsTrue(str2.eq(str1));
			Assert::IsFalse(str1.eq(str3));
		}

		TEST_METHOD2(iequals)
		{
			xString str1 = xString("hello!");
			xString str2 = xString("Hello!");
			xString str3 = xString("hello! :)");
			Assert::IsFalse((str1 == str2) || (str1 == str3) || (str2 == str3));
			Assert::IsTrue(str1.ieq(str2));
			Assert::IsTrue(str2.ieq(str1));
			Assert::IsFalse(str1.ieq(str3));
			Assert::IsFalse(str3.ieq(str2));
		}

		TEST_METHOD2(assign_char)
		{
			xString str = "hello :)";
			Assert::AreEqual(8, str.length());
			Assert::IsTrue(str.eq("hello :)"));
			Assert::IsTrue( ((xString)"hello :)").eq(str) );
		}

		TEST_METHOD2(clone)
		{
			xString str1 = "hello!";
			xString str2;
			str2 = str1;
			Assert::IsFalse(str1 == str2);
			Assert::IsTrue(str1.eq(str2));
		}

		TEST_METHOD2(sub1)
		{
			xString str = "hello!";
			xString str2 = str.sub(1, 1);
			Assert::IsTrue(str2.eq("e"));

			Assert::IsTrue(str.sub(1, 2).eq("el"));
			Assert::IsTrue(str.sub(2, 12).eq("llo!"));
			Assert::IsTrue(str.sub(3, 2).eq(""));
		}

		TEST_METHOD2(left)
		{
			xString str = "hello!";
			Assert::IsTrue(str.left(3).eq("hel"));
			Assert::IsTrue(str.left(0).eq(""));
			Assert::IsTrue(str.left(100).eq(str));
		}

		TEST_METHOD2(tail)
		{
			xString str = "hello!";
			Assert::IsTrue(str.tail(3).eq("lo!"));
			Assert::IsTrue(str.tail(0).eq(str));
			Assert::IsTrue(str.tail(100).eq(""));
		}

		TEST_METHOD2(right)
		{
			xString str = "hello!";
			Assert::IsTrue(str.right(2).eq("o!"));
			Assert::IsTrue(str.right(0).eq(""));
			Assert::IsTrue(str.right(100).eq(str));
		}

		TEST_METHOD2(findc)
		{
			Assert::AreEqual(1, xString("hello!").findc("e"));
			Assert::AreEqual(-1, xString("hello!").findc("asd123"));
			Assert::AreEqual(5, xString("hello!").findc("asd!123"));
		}

		TEST_METHOD2(find)
		{
			Assert::AreEqual(1, xString("hello!").find("e"));
			Assert::AreEqual(2, xString("hello!").find("ll"));
			Assert::AreEqual(-1, xString("hello!").find("lle"));
			Assert::AreEqual(0, xString("aaaa").find("aa"));
		}

		TEST_METHOD2(plus_char)
		{
			xString s1 = xString("hey");
			xString s2 = s1 + " ";
			Assert::IsTrue(s2.eq("hey "));
			xString s3 = s2 + xString("you");
			Assert::IsTrue(s3.eq("hey you"));

			Assert::IsTrue((xString("hey ") + xString("you") + xString("!")).eq(s3 + "!"));
		}

		TEST_METHOD2(plus_other)
		{
			Assert::IsTrue( (xString("ten=") + 10).eq("ten=10") );
			Assert::IsTrue( (xString("pi=") + 3.1415).left(9).eq("pi=3.1415") );
			Assert::IsTrue( (xString("") + true + xString(" or ") + false).eq("true or false") );
		}

		TEST_METHOD2(clone2)
		{
			xString str1 = "hello!";
			xString str2(str1);
			Assert::IsFalse(str1 == str2);
			Assert::IsTrue(str1.eq(str2));
		}

		TEST_METHOD2(assign_self)
		{
			xString str1 = "hello!";
			str1 = str1;
			Assert::IsTrue(str1.eq("hello!"));
		}

	};
}