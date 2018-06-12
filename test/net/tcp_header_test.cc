
#include <catch.h>
#include <om/om.h>

using namespace om;

TEST_CASE("net::tcp_header", "[net][tcp_header]")
{
	unsigned char buf1[] = {
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

	net::tcp_header tcp1(buf1);
	net::tcp_header tcp2;

	SECTION("tcp_header")
	{
		SECTION("can be constructed with a byte buffer")
		{
			CHECK(tcp1.len() == 20);
		}

		SECTION("can be constructed without a byte buffer")
		{
			CHECK(tcp2.len() == 20);
			CHECK(tcp2.src_port() == 0);
			CHECK(tcp2.dest_port() == 0);
			CHECK(tcp2.seq_no() == 0);
			CHECK(tcp2.ack_no() == 0);
			CHECK(tcp2.flags() == 0);
			CHECK(tcp2.window_size() == 0);
		}
	}

	SECTION("src_port")
	{
		CHECK(tcp1.src_port() == 59966);
	}

	SECTION("dest_port")
	{
		CHECK(tcp1.dest_port() == 443);
	}

	SECTION("seq_no")
	{
		CHECK(tcp1.seq_no() == 1739385856);
	}

	SECTION("ack_no")
	{
		CHECK(tcp1.ack_no() == 2029983222);
	}

	SECTION("flags")
	{
		CHECK(tcp1.flags() == 16);
	}

	SECTION("window_size")
	{
		CHECK(tcp1.window_size() == 4117);
	}
}
