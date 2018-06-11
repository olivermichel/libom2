
#include <catch.h>
#include <om/om.h>

using namespace om;

TEST_CASE("net::ip4_header", "[net][ip4_header]")
{
	unsigned char buf1[] = { // IPv4
		0x45, 0x00,             // version, ihl, dscp, ecn
		0x00, 0x57,             // total len
		0x00, 0x00,             // identification
		0x40, 0x00,             // flags, fragment offset
		0x40,                   // ttl
		0x06,                   // protocol
		0xbb, 0xef,             // checksum
		0xac, 0x10, 0x15, 0x05, // source address
		0xc0, 0x1e, 0xfd, 0x7d  // destination address
	};

	SECTION("ip4_header")
	{
		SECTION("can be constructed with a byte buffer")
		{
			net::ip4_header ip4(buf1);
			CHECK(ip4.len() == 20);
		}

		SECTION("can be constructed without a byte buffer")
		{
			net::ip4_header ip4;
			CHECK(ip4.len() == 20);
			//TODO: check all fields empty
		}
	}

	SECTION("total_len")
	{
		net::ip4_header ip4(buf1);
		CHECK(ip4.total_len() == 0x57);
	}

	SECTION("set_total_len")
	{
		net::ip4_header ip4;
		ip4.set_total_len(550);
		CHECK(ip4.total_len() == 550);
	}

	SECTION("id")
	{
		net::ip4_header ip4(buf1);
		CHECK(ip4.id() == 0);
	}

	SECTION("set_id")
	{
		net::ip4_header ip4;
		ip4.set_id(1242);
		CHECK(ip4.id() == 1242);
	}

	SECTION("ttl")
	{
		net::ip4_header ip4(buf1);
		CHECK(ip4.ttl() == 64);
	}

	SECTION("set_ttl")
	{
		net::ip4_header ip4;
		ip4.set_ttl(32);
		CHECK(ip4.ttl() == 32);
	}

	SECTION("proto")
	{
		net::ip4_header ip4(buf1);
		CHECK(ip4.proto() == 6);
	}

	SECTION("set_proto")
	{
		net::ip4_header ip4;
		ip4.set_proto(17);
		CHECK(ip4.proto() == 17);
	}

	SECTION("src_addr")
	{
		net::ip4_header ip4(buf1);
		CHECK(ip4.src_addr() == net::ip4_addr("172.16.21.5"));
	}

	SECTION("set_src_addr")
	{
		net::ip4_header ip4;
		ip4.set_src_addr(net::ip4_addr("8.8.4.4"));
		CHECK(ip4.src_addr() == net::ip4_addr("8.8.4.4"));
	}

	SECTION("dest_addr")
	{
		net::ip4_header ip4(buf1);
		CHECK(ip4.dest_addr() == net::ip4_addr("192.30.253.125"));
	}

	SECTION("set_dest_addr")
	{
		net::ip4_header ip4;
		ip4.set_dest_addr(net::ip4_addr("8.8.4.4"));
		CHECK(ip4.dest_addr() == net::ip4_addr("8.8.4.4"));
	}
}
