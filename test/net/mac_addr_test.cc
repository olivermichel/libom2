
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
			auto a1 = net::mac_addr(0x010301010101);

			a1.write(buf2);

			CHECK(buf1[0] == buf2[0]);
			CHECK(buf1[1] == buf2[1]);
			CHECK(buf1[2] == buf2[2]);
			CHECK(buf1[3] == buf2[3]);
			CHECK(buf1[4] == buf2[4]);
			CHECK(buf1[5] == buf2[5]);
		}

		SECTION("can be constructed from a byte buffer")
		{
			uint8_t buf1[6] = { 0x01, 0x03, 0x01, 0x01, 0x01, 0x01 };
			uint8_t buf2[6] = { 0 };
			net::mac_addr a1(buf1);

			a1.write(buf2);

			CHECK(buf1[0] == buf2[0]);
			CHECK(buf1[1] == buf2[1]);
			CHECK(buf1[2] == buf2[2]);
			CHECK(buf1[3] == buf2[3]);
			CHECK(buf1[4] == buf2[4]);
			CHECK(buf1[5] == buf2[5]);
		}

		SECTION("can be constructed from a formatted string")
		{
			std::string str1 = "01:02:03:04:05:06";
			std::string str2 = "1:2:3:4:5:6";

			CHECK_NOTHROW(net::mac_addr(str1));
			CHECK_NOTHROW(net::mac_addr(str2));

			net::mac_addr addr1(str1);
			net::mac_addr addr2(str2);

			CHECK(addr1.to_uint64() == 0x010203040506);
			CHECK(addr2.to_uint64() == 0x010203040506);
		}
	}

	SECTION("operator uint64_t()")
	{
		auto a1 = net::mac_addr(0x010203040506);
		CHECK((uint64_t) a1 == 0x010203040506);
	}

	SECTION("to_string")
	{
		auto a1 = net::mac_addr(0x010203040506);
		CHECK(a1.to_string() == "01:02:03:04:05:06");
	}

	SECTION("to_uint64")
	{
		auto a1 = net::mac_addr(0x010203040506);
		CHECK(a1.to_uint64() == 0x010203040506);
	}

	SECTION("operator<<")
	{
		auto a1 = net::mac_addr(0x010203040506);
		std::stringstream ss;
		ss << a1;
		CHECK(ss.str() == "01:02:03:04:05:06");
	}

	SECTION("hash<mac_addr>()")
	{
		auto a1 = net::mac_addr(0x010203040506);
		CHECK(std::hash<net::mac_addr>()(a1) == 0x010203040506);
	}
}
