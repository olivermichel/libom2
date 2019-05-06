
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
		0x01, 0xbb              // destination port
	};

	unsigned char buf2[] = {    // IPv4
		0x45, 0x00,             // version, ihl, dscp, ecn
		0x00, 0x57,             // total len
		0x00, 0x00,             // identification
		0x40, 0x00,             // flags, fragment offset
		0x40,                   // ttl
		0x01,                   // protocol
		0xbb, 0xef,             // checksum
		0xac, 0x34, 0xde, 0x05, // source address
		0xc0, 0x66, 0xed, 0x7d, // destination address
	};

	auto key1 = net::ip4_flow_key::from_ip4_bytes(buf1);
	auto key2 = net::ip4_flow_key::from_ip4_bytes(buf2);

	SECTION("from_ip4_bytes")
	{
		CHECK(key1.ip_src() == net::ip4_addr::from_string("172.16.21.5"));
		CHECK(key1.ip_dst() == net::ip4_addr::from_string("172.16.21.5"));
		CHECK(key1.ip_proto() == 6);
		CHECK(key1.tp_src() == 59966);
		CHECK(key1.tp_dst() == 443);

		CHECK(key2.ip_src() == net::ip4_addr::from_string("172.52.222.5"));
		CHECK(key2.ip_dst() == net::ip4_addr::from_string("172.52.222.5"));
		CHECK(key2.ip_proto() == 1);
		CHECK(key2.tp_src() == 0);
		CHECK(key2.tp_dst() == 0);
	}

	SECTION("hash<ip4_flow_key>()")
	{
		CHECK(std::hash<net::ip4_flow_key>()(key1) == 0x40513c8ab5e8de2c);
		CHECK(std::hash<net::ip4_flow_key>()(key2) == 0x72531fae101a9016);
	}

	SECTION("operator==()")
	{
		CHECK(key1 == key1);
		CHECK(key1 != key2);
	}
}
