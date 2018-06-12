
#include <catch.h>
#include <om/om.h>

using namespace om;

TEST_CASE("net::udp_header", "[net][udp_header]")
{
	unsigned char buf1[] = {
		0xd8, 0x51, // source port
		0x00, 0x35, // destination port
		0x00, 0x2c, // payload length
		0x82, 0x64  // checksum
	};

	SECTION("udp_header")
	{
		SECTION("can be constructed with a byte buffer")
		{
			net::udp_header udp(buf1);
			CHECK(udp.len() == 8);
		}

		SECTION("can be constructed without a byte buffer")
		{
			net::udp_header udp;
			CHECK(udp.len() == 8);
			//TODO: check all fields empty
		}
	}
}
