
#include <catch.h>
#include <om/om.h>

using namespace om;

TEST_CASE("sys", "[sys]")
{
	char buf1[8] = {0};
	unsigned char buf2[8] = {0};

	SECTION("read/write_uint16")
	{
		CHECK(sys::write_uint16(10232, buf1) == 2);
		CHECK(sys::read_uint16(buf1) == 10232);
		CHECK(sys::write_uint16(10232, (char*) buf2) == 2);
		CHECK(sys::read_uint16((char*) buf2) == 10232);
	}

	SECTION("read/write_uint32")
	{
		CHECK(sys::write_uint32(34837439, buf1) == 4);
		CHECK(sys::read_uint32(buf1) == 34837439);
		CHECK(sys::write_uint32(34837439, (char*) buf2) == 4);
		CHECK(sys::read_uint32((char*) buf2) == 34837439);
	}

	SECTION("read/write_uint64")
	{
		CHECK(sys::write_uint64(3483743923672, buf1) == 8);
		CHECK(sys::read_uint64(buf1) == 3483743923672);
		CHECK(sys::write_uint64(3483743923672, (char*) buf2) == 8);
		CHECK(sys::read_uint64((char*) buf2) == 3483743923672);
	}
}
