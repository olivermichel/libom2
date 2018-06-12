
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

	SECTION("tcp_header")
	{
		SECTION("can be constructed with a byte buffer")
		{
			net::tcp_header tcp(buf1);
			CHECK(tcp.len() == 20);
		}

		SECTION("can be constructed without a byte buffer")
		{
			net::tcp_header tcp;
			CHECK(tcp.len() == 20);
			//TODO: check all fields empty
		}
	}
}
