
#include <catch.h>
#include <om/om.h>

using namespace om;

TEST_CASE("net::mac_addr", "[net][mac_addr]")
{
	SECTION("mac_addr()")
	{
		SECTION("can be constructed with an integer")
		{
			uint8_t buf1[6] = { 0x01, 0x03, 0x01, 0x01, 0x01, 0x01 };
			uint8_t buf2[6] = { 0 };
			net::mac_addr a1 = 0x010301010101;

			a1.write(buf2);

			CHECK(buf1[0] == buf2[5]);
			CHECK(buf1[1] == buf2[4]);
			CHECK(buf1[2] == buf2[3]);
			CHECK(buf1[3] == buf2[2]);
			CHECK(buf1[4] == buf2[1]);
			CHECK(buf1[5] == buf2[0]);
		}

		SECTION("can be constructed from a byte buffer")
		{
			uint8_t buf1[6] = { 0x01, 0x03, 0x01, 0x01, 0x01, 0x01 };
			uint8_t buf2[6] = { 0 };
			net::mac_addr a1(buf1);

			a1.write(buf2);

			CHECK(buf1[0] == buf2[5]);
			CHECK(buf1[1] == buf2[4]);
			CHECK(buf1[2] == buf2[3]);
			CHECK(buf1[3] == buf2[2]);
			CHECK(buf1[4] == buf2[1]);
			CHECK(buf1[5] == buf2[0]);
		}
	}

	SECTION("str()")
	{
		SECTION("returns a string in canonical form")
		{
			net::mac_addr a1 = 0x010101010101;
			CHECK(a1.str() == "01:01:01:01:01:01");
		}
	}
}
