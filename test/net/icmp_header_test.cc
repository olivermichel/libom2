
#include <catch.h>
#include <om/om.h>

using namespace om;

TEST_CASE("net::icmp_header", "[net][icmp_header]")
{
	unsigned char buf1[] = {
		0x08,                  // icmp type
		0x00,                  // icmp code
		0xc3, 0x12,            // checksum
		0x8a, 0x8f, 0x00, 0x00 // data
	};

	net::icmp_header icmp1(buf1);
	net::icmp_header icmp2;

	SECTION("icmp_header")
	{
		SECTION("can be constructed with a byte buffer")
		{
			CHECK(icmp1.len() == 8);
		}

		SECTION("can be constructed without a byte buffer")
		{
			CHECK(icmp2.len() == 8);
			CHECK(icmp2.type() == 0);
			CHECK(icmp2.code() == 0);
		}
	}

	SECTION("type")
	{
		CHECK(icmp1.type() == 8);
	}

	SECTION("code")
	{
		CHECK(icmp1.code() == 0);
	}
}
