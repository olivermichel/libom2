
#include <catch.h>
#include <om/om.h>

using namespace om;

TEST_CASE("net::ip4_addr", "[net][ip4_addr]")
{
	SECTION("ip4_addr") {
		SECTION("can be constructed with a string")
		{
			auto a1 = net::ip4_addr::from_string("5.5.8.9");
			CHECK(a1.to_uint32() == 0x09080505);
		}

		SECTION("throws an error if the ip address is invalid")
		{
			std::string invalid1 = "422.122.234.34";
			const char* invalid2 = invalid1.c_str();
			CHECK_THROWS_AS(net::ip4_addr::from_string(invalid1), std::invalid_argument);
			CHECK_THROWS_AS(net::ip4_addr::from_string(invalid2), std::invalid_argument);
		}

		SECTION("can be constructed with an integer")
		{
			net::ip4_addr a1;
			auto a2 = net::ip4_addr::from_host(0x09080505);
			CHECK(a1.to_uint32() == 0);
			CHECK(a2.to_uint32() == 0x05050809);
		}

		SECTION("can be constructed from a byte array")
		{
			unsigned char buf[] = { 0x02, 0x05, 0x05, 0x04};
			auto a1 = net::ip4_addr::from_bytes(buf);
			CHECK(a1.to_uint32() == 0x02050504);
		}
	}

	SECTION("to_string")
	{
		auto a1 = net::ip4_addr::from_host(0x05050809);
		CHECK(a1.to_string() == "5.5.8.9");
		auto a2 = net::ip4_addr::from_string("12.54.23.122");
		CHECK(a2.to_string() == "12.54.23.122");
	}

	SECTION("to_uint32")
	{
		auto a1 = net::ip4_addr::from_string("5.5.8.9");
		CHECK(a1.to_uint32() == 0x9080505);
	}

	SECTION("operator<<")
	{
		auto a1 = net::ip4_addr::from_string("5.5.8.9");
		std::stringstream ss;
		ss << a1;
		CHECK(ss.str() == "5.5.8.9");
	}

	SECTION("reverse_byte_order")
	{
		auto addr = net::ip4_addr::from_string("5.5.8.9");
		CHECK(net::ip4_addr::reverse_byte_order(addr) == net::ip4_addr::from_string("9.8.5.5"));
	}
}

