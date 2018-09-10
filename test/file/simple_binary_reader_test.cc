
#include <catch.h>
#include <om/om.h>
#include <test_format.h>

using namespace om;

TEST_CASE("file::simple_binary_reader", "[file][simple_binary_reader]")
{
	SECTION("simple_binary_reader")
	{
		CHECK_THROWS(file::simple_binary_reader<test_format>("does/not/exist.format"));
	}

	SECTION("read")
	{
		unsigned count = 0;
		test_format f { };
		file::simple_binary_reader<test_format> reader("test/data/test.test_format");
		while (reader.next(f)) count++;
		CHECK(count == 2);
		CHECK(reader.done());
	}

	SECTION("read 2")
	{
		unsigned count = 0;
		test_format f { };
		file::simple_buffered_binary_reader<test_format> reader("test/data/test.test_format");
		while (reader.next(f)) count++;
//		CHECK(count == 2);
////		CHECK(reader.done());
	}


	SECTION("reset")
	{
		unsigned count = 0;
		test_format f { };
		file::simple_binary_reader<test_format> reader("test/data/test.test_format");
		while (reader.next(f)) count++;
		CHECK(count == 2);
		CHECK(reader.done());
		reader.reset();
		CHECK_FALSE(reader.done());
		while (reader.next(f)) count++;
		CHECK(count == 4);
	}
}
