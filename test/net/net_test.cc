
#include <catch.h>
#include <om/om.h>

using namespace om;

TEST_CASE("net", "[net]")
{
	unsigned char buf1[] = {

		// Ethernet: 64:4b:f0:01:53:5b -> 00:0c:42:a6:25:bc
		0x00, 0x0c, 0x42, 0xa6, 0x25, 0xbc, 0x64, 0x4b, 0xf0, 0x01, 0x53, 0x5b, 0x08, 0x06,

		// ARP
		0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x02, 0x64, 0x4b, 0xf0, 0x01, 0x53, 0x5b,
		0xac, 0x10, 0x15, 0x05, 0x00, 0x0c, 0x42, 0xa6, 0x25, 0xbc, 0xac, 0x10, 0x15, 0x01
	};

	unsigned char buf2[] = {

		// Ethernet: 00:0c:42:a6:25:bc -> 64:4b:f0:01:53:5b
		0x64, 0x4b, 0xf0, 0x01, 0x53, 0x5b, 0x00, 0x0c, 0x42, 0xa6, 0x25, 0xbc, 0x08, 0x00,

		// IP: 74.125.202.188 -> 172.16.21.5
		0x45, 0x20, 0x00, 0x6f, 0x8e, 0xff, 0x00, 0x00, 0x39, 0x06, 0x1c, 0x1a, 0x4a, 0x7d,
		0xca, 0xbd, 0xac, 0x10, 0x15, 0x05,

		// TCP
		0x01, 0xbb, 0xec, 0x59, 0xec, 0x8c, 0x41, 0x9d, 0x5a, 0x10, 0x68, 0x57, 0x80, 0x18,
		0x01, 0x99, 0x97, 0x05, 0x00, 0x00, 0x01, 0x01, 0x08, 0x0a, 0x1d, 0xc5, 0x2f, 0x57,
		0x4a, 0x0e, 0x66, 0x9a
	};

	unsigned char buf3[] = {

		// Ethernet: 00:0c:42:a6:25:bc -> 64:4b:f0:01:53:5b
		0x64, 0x4b, 0xf0, 0x01, 0x53, 0x5b, 0x00, 0x0c, 0x42, 0xa6, 0x25, 0xbc, 0x08, 0x00,

		// IP: 172.16.21.1 -> 172.16.21.5
		0x45, 0x00, 0x00, 0x54, 0x45, 0x18, 0x00, 0x00, 0x40, 0x11, 0xb3, 0x5a, 0xac, 0x10,
		0x15, 0x01, 0xac, 0x10, 0x15, 0x05,

		// UDP
		0x00, 0x35, 0xdb, 0x39, 0x00, 0x40, 0x0f, 0x28
	};

	unsigned char buf4[] = { // Ethernet + IP + ICMP

		// Ethernet: 64:4b:f0:01:53:5b -> 00:0c:42:a6:25:bc
		0x00, 0x0c, 0x42, 0xa6, 0x25, 0xbc, 0x64, 0x4b, 0xf0, 0x01, 0x53, 0x5b, 0x08, 0x00,

		// IP
		0x45, 0x00, 0x00, 0x38, 0x54, 0x5f, 0x00, 0x00, 0x40, 0x01, 0xa4, 0x3f, 0xac, 0x10,
		0x15, 0x05, 0xac, 0x10, 0x15, 0x01,

		// ICMP
		0x03, 0x03, 0x09, 0x14, 0x00, 0x00, 0x00, 0x00
	};

	SECTION("parse_host_port")
	{
		auto test1 = net::parse_host_port("127.0.0.1:3222");
		CHECK(test1.first == "127.0.0.1");
		CHECK(test1.second == 3222);
		CHECK_THROWS(net::parse_host_port("127.0.0.1/3222"));
        CHECK_THROWS(net::parse_host_port("127.0.0.1"));
        CHECK_THROWS(net::parse_host_port("12732"));
	}

	SECTION("parse an arp packet")
	{
		net::ethernet_header eth(buf1);
		CHECK(eth.len() == 14);
		CHECK(eth.dest_addr() == net::mac_addr(0x000c42a625bc));
		CHECK(eth.src_addr() == net::mac_addr(0x644bf001535b));
		CHECK(eth.ether_type() == 0x0806);

		net::arp_header arp(buf1 + eth.len());
		CHECK(arp.len() == 28);
	}

	SECTION("parse a tcp packet")
	{
		net::ethernet_header eth(buf2);
		CHECK(eth.len() == 14);
		CHECK(eth.dest_addr() == net::mac_addr(0x644bf001535b));
		CHECK(eth.src_addr() == net::mac_addr(0x000c42a625bc));
		CHECK(eth.ether_type() == 0x0800);

		net::ip4_header ip(buf2 + eth.len());
		CHECK(ip.len() == 20);
		CHECK(ip.proto() == 6);
		CHECK(ip.src_addr() == net::ip4_addr::from_string("74.125.202.189"));
		CHECK(ip.dest_addr() == net::ip4_addr::from_string("172.16.21.5"));
		CHECK(ip.id() == 36607);
		CHECK(ip.ttl() == 57);

		net::tcp_header tcp(buf2 + eth.len() + ip.len());
		CHECK(tcp.len() == 20);
	}

	SECTION("parse an udp packet")
	{
		net::ethernet_header eth(buf3);
		CHECK(eth.len() == 14);
		CHECK(eth.dest_addr() == net::mac_addr(0x644bf001535b));
		CHECK(eth.src_addr() == net::mac_addr(0x000c42a625bc));
		CHECK(eth.ether_type() == 0x0800);

		net::ip4_header ip(buf3 + eth.len());
		CHECK(ip.len() == 20);
		CHECK(ip.proto() == 17);
		CHECK(ip.src_addr() == net::ip4_addr::from_string("172.16.21.1"));
		CHECK(ip.dest_addr() == net::ip4_addr::from_string("172.16.21.5"));
		CHECK(ip.id() == 17688);
		CHECK(ip.ttl() == 64);

		net::udp_header udp(buf3 + eth.len() + ip.len());
		CHECK(udp.len() == 8);
	}

	SECTION("parse an icmp packet")
	{
		net::ethernet_header eth(buf4);
		CHECK(eth.len() == 14);
		CHECK(eth.dest_addr() == net::mac_addr(0x000c42a625bc));
		CHECK(eth.src_addr() == net::mac_addr(0x644bf001535b));
		CHECK(eth.ether_type() == 0x0800);

		net::ip4_header ip(buf4 + eth.len());
		CHECK(ip.len() == 20);
		CHECK(ip.proto() == 1);
		CHECK(ip.src_addr() == net::ip4_addr::from_string("172.16.21.5"));
		CHECK(ip.dest_addr() == net::ip4_addr::from_string("172.16.21.1"));
		CHECK(ip.id() == 21599);
		CHECK(ip.ttl() == 64);

		net::icmp_header icmp(buf4 + eth.len() + ip.len());
		CHECK(icmp.len() == 8);
		CHECK(icmp.type() == 3);
		CHECK(icmp.code() == 3);
	}

	SECTION("build an ip datagram") {

		const unsigned total_len = 14 + 20;

		unsigned char buf[total_len] = {0};
		net::ethernet_header eth(buf);
		net::ip4_header ip(buf + 14);

		for (unsigned i = 0; i < 14; i++)
			CHECK(buf[i] == 0);

		CHECK(buf[14] == 0x45); // ip version (4) and internet header length (5)

		for (unsigned i = 15; i < total_len; i++)
			CHECK(buf[i] == 0);

		eth.set_dest_addr(net::mac_addr(0x2));
		eth.set_src_addr(net::mac_addr(0x1));
		eth.set_ether_type(0x0800);

		ip.set_src_addr(net::ip4_addr::from_string("1.2.3.4"));
		ip.set_dest_addr(net::ip4_addr::from_string("2.4.6.8"));
		ip.set_total_len(0);
		ip.set_proto(1);
		ip.set_ttl(64);
		ip.set_id(45);

		CHECK(buf[0]  == 0);    // dest addr
		CHECK(buf[1]  == 0);
		CHECK(buf[2]  == 0);
		CHECK(buf[3]  == 0);
		CHECK(buf[4]  == 0);
		CHECK(buf[5]  == 2);

		CHECK(buf[6]  == 0);    // source addr
		CHECK(buf[7]  == 0);
		CHECK(buf[8]  == 0);
		CHECK(buf[9]  == 0);
		CHECK(buf[10] == 0);
		CHECK(buf[11] == 1);

		CHECK(buf[12] == 0x08); // ether type
		CHECK(buf[13] == 0x00);

		CHECK(buf[16] == 0);    // total length
		CHECK(buf[17] == 0);

		CHECK(buf[18] == 0);    // ident
		CHECK(buf[19] == 45);

		CHECK(buf[22] == 64);   // ttl
		CHECK(buf[23] == 1);    // proto

		CHECK(buf[26] == 0x01); // source addr
		CHECK(buf[27] == 0x02);
		CHECK(buf[28] == 0x03);
		CHECK(buf[29] == 0x04);

		CHECK(buf[30] == 0x02); // dest addr
		CHECK(buf[31] == 0x04);
		CHECK(buf[32] == 0x06);
		CHECK(buf[33] == 0x08);
	}
}
