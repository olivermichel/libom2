
#include <catch.h>
#include <om/om.h>
#include <test_format.h>

using namespace om;

TEST_CASE("file::simple_binary_reader", "[file][simple_binary_reader]")
{
	SECTION("simple_binary_reader") {
		CHECK_NOTHROW(om::file::simple_binary_reader<test_format>("test.test_format"));
	}
}
