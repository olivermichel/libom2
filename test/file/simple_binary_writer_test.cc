
#include <catch.h>
#include <om/om.h>
#include <test_format.h>

using namespace om;

TEST_CASE("file::simple_binary_writer", "[file][simple_binary_writer]")
{
	test_format d1 { 12321, 28202 };
	test_format d2 { 23922, 48292 };

	SECTION("simple_binary_writer")
	{
		CHECK_NOTHROW(om::file::simple_binary_writer<test_format>("test1.test_format"));
	}

	SECTION("write")
	{
		om::file::simple_binary_writer<test_format> writer("test2.test_format");
		writer.write(d1);
		writer.write(d2);
		writer.close();
	}
}
