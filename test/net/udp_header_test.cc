
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

	net::udp_header udp1(buf1);
	net::udp_header udp2;

	SECTION("udp_header")
	{
		SECTION("can be constructed with a byte buffer")
		{
			CHECK(udp1.len() == 8);
		}

		SECTION("can be constructed without a byte buffer")
		{
			CHECK(udp2.len() == 8);
			CHECK(udp2.src_port() == 0);
			CHECK(udp2.dest_port() == 0);
			CHECK(udp2.payload_length() == 0);
		}
	}

	SECTION("src_port")
	{
		CHECK(udp1.src_port() == 55377);
	}

	SECTION("set_src_port")
	{
		udp2.set_src_port(232);
		CHECK(udp2.src_port() == 232);
	}

	SECTION("dest_port")
	{
		CHECK(udp1.dest_port() == 53);
	}

	SECTION("set_dest_port")
	{
		udp2.set_dest_port(232);
		CHECK(udp2.dest_port() == 232);
	}

	SECTION("payload_length")
	{
		CHECK(udp1.payload_length() == 44);
	}

	SECTION("set_payload_length")
	{
		udp2.set_payload_length(678);
		CHECK(udp2.payload_length() == 678);
	}
}
