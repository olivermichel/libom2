
#include <catch.h>
#include <om/om.h>

using namespace om;

TEST_CASE("net::ip4_addr", "[net][ip4_addr]")
{
	SECTION("ip4_addr") {
		SECTION("can be constructed with a string")
		{
			net::ip4_addr a1 = "5.5.8.9";
			CHECK((uint32_t) a1 == htonl(0x05050809));
		}

		SECTION("throws an error if the ip address is invalid")
		{
			std::string invalid1 = "422.122.234.34";
			const char* invalid2 = invalid1.c_str();
			CHECK_THROWS_AS(net::ip4_addr(invalid1), std::invalid_argument);
			CHECK_THROWS_AS(net::ip4_addr(invalid2), std::invalid_argument);
		}

		SECTION("can be constructed with an integer")
		{
			net::ip4_addr a1;
			net::ip4_addr a2 = 0x05050809;
			CHECK((uint32_t) a1 == 0);
			CHECK((uint32_t) a2 == htonl(0x05050809));
		}

		SECTION("can be constructed from a byte array")
		{
			unsigned char buf[] = { 0x02, 0x05, 0x05, 0x04};
			net::ip4_addr a1(buf);
			CHECK((uint32_t) a1 == 0x02050504);
		}
	}

	SECTION("operator uint32_t()")
	{
		net::ip4_addr a1 = "5.5.8.9";
		CHECK((uint32_t) a1 == htonl(0x05050809));
	}

	SECTION("to_string")
	{
		net::ip4_addr a1 = 0x05050809;
		CHECK(a1.to_string() == "5.5.8.9");
		net::ip4_addr a2 = "12.54.23.122";
		CHECK(a2.to_string() == "12.54.23.122");
	}

	SECTION("to_uint32")
	{
		net::ip4_addr a1 = "5.5.8.9";
		CHECK(a1.to_uint32() == htonl(0x05050809));
	}

	SECTION("operator<<")
	{
		net::ip4_addr a1 = "5.5.8.9";
		std::stringstream ss;
		ss << a1;
		CHECK(ss.str() == "5.5.8.9");
	}
}

