
#include <catch.h>
#include <om/om.h>

using namespace om;

TEST_CASE("file", "[file]")
{
	SECTION("size")
	{
		SECTION("determines the file size")
		{
			CHECK(file::size("test/data/test.test_format") == 16);
		}

		SECTION("throws an exception when the file does not exist")
		{
			CHECK_THROWS(file::size("does/not/exist.file"));
		}
	}
}
