
#include <catch.h>
#include <om/om.h>

using namespace om;

TEST_CASE("net::ip4_flow_key", "[net][ip4_flow_key]")
{
	unsigned char buf1[] = {    // IPv4
		0x45, 0x00,             // version, ihl, dscp, ecn
		0x00, 0x57,             // total len
		0x00, 0x00,             // identification
		0x40, 0x00,             // flags, fragment offset
		0x40,                   // ttl
		0x06,                   // protocol
		0xbb, 0xef,             // checksum
		0xac, 0x10, 0x15, 0x05, // source address
		0xc0, 0x1e, 0xfd, 0x7d, // destination address
		                        // TCP
		0xea, 0x3e,             // source port
		0x01, 0xbb,             // destination port
		0x67, 0xac, 0xec, 0x00, // sequence number
		0x78, 0xff, 0x15, 0xf6, // acknowledgement number
		0x80,                   // data offset + reserved fields
		0x10,                   // flags
		0x10, 0x15,             // window size
		0xc4, 0xf0,             // checksum
		0x00, 0x00              // urgent pointer
	};

	SECTION("from_ip4_bytes")
	{
		auto key = net::ip4_flow_key::from_ip4_bytes(buf1);
		CHECK(key.ip_src() == net::ip4_addr::from_string("172.16.21.5"));
		CHECK(key.ip_dst() == net::ip4_addr::from_string("172.16.21.5"));
		CHECK(key.ip_proto() == 6);
		CHECK(key.tp_src() == 59966);
		CHECK(key.tp_dst() == 443);
	}
}
