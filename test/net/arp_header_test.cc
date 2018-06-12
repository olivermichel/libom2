
#include <catch.h>
#include <om/om.h>

using namespace om;

TEST_CASE("net::arp_header", "[net][arp_header]")
{
	unsigned char buf1[] = {
		0x00, 0x01,                         // hardware type
		0x08, 0x00,                         // protocol type
		0x06,                               // hardware address length
		0x04,                               // protocol address length
		0x00, 0x02,                         // operation
		0x78, 0x7b, 0x8a, 0x4d, 0xf0, 0x9c, // sender hardware address
		0xac, 0x10, 0x15, 0x07,             // sender protocol address
		0x64, 0x4b, 0xf0, 0x01, 0x53, 0x5b, // target hardware address
		0xac, 0x10, 0x15, 0x05              // target protocol address
	};

	SECTION("arp_header")
	{
		SECTION("can be constructed with a byte buffer")
		{
			net::arp_header arp(buf1);
			CHECK(arp.len() == 28);
		}

		SECTION("can be constructed without a byte buffer")
		{
			net::arp_header arp;
			CHECK(arp.len() == 28);
			//TODO: check all fields empty
		}
	}
}
