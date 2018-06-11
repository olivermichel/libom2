
#include <catch.h>
#include <om/om.h>

using namespace om;

unsigned char buf1[] = { // EthII
	0x00, 0x26, 0x62, 0x2f, 0x47, 0x87, // destination address
	0x00, 0x1d, 0x60, 0xb3, 0x01, 0x84, // source address
	0x08, 0x00 // ether type
};



TEST_CASE("net::ethernet_header", "[net][ethernet_header]")
{
	SECTION("ethernet_header")
	{
		SECTION("can be initialized with a byte buffer w")
		{
			net::ethernet_header eth(buf1);
			CHECK(eth.len() == 14);
		}
	}

	SECTION("dest_addr")
	{
		SECTION("returns the frame's destination address")
		{
			net::ethernet_header eth(buf1);
			CHECK(eth.dest_addr() == net::mac_addr(0x0026622f4787));
		}
	}

	SECTION("set_dest_addr")
	{
		SECTION("sets the frame's destination address")
		{
			net::ethernet_header eth;
			eth.set_dest_addr(net::mac_addr(0x0026622f4787));
			CHECK(eth.dest_addr() == net::mac_addr(0x0026622f4787));
		}
	}

	SECTION("src_addr")
	{
		SECTION("returns the frame's source address")
		{
			net::ethernet_header eth(buf1);
			CHECK(eth.src_addr() == net::mac_addr(0x001d60b30184));
		}
	}

	SECTION("set_src_addr")
	{
		SECTION("sets the frame's source address")
		{
			net::ethernet_header eth;
			eth.set_src_addr(net::mac_addr(0x0026622f4787));
			CHECK(eth.src_addr() == net::mac_addr(0x0026622f4787));
		}
	}

	SECTION("ether_type")
	{
		SECTION("returns the frame's ether type")
		{
			net::ethernet_header eth(buf1);
			CHECK(eth.ether_type() == 0x0800);
		}
	}

	SECTION("set_ether_type")
	{
		SECTION("sets the frame's ether type")
		{
			net::ethernet_header eth;
			eth.set_ether_type(0x0800);
			CHECK(eth.ether_type() == 0x0800);
		}
	}
}