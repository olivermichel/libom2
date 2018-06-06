
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
			net::ip4_addr a1 = 0x05050809;
			CHECK((uint32_t) a1 == htonl(0x05050809));
		}
	}

	SECTION("operator uint32_t()")
	{
		SECTION("returns a 4 byte unsigned integer")
		{
			net::ip4_addr a1 = "5.5.8.9";
			CHECK((uint32_t) a1 == htonl(0x05050809));
		}
	}

	SECTION("str_desc()")
	{
		SECTION("returns a string in dotted decimal notation")
		{
			net::ip4_addr a1 = 0x05050809;
			CHECK(a1.str_desc() == "5.5.8.9");
			net::ip4_addr a2 = "12.54.23.122";
			CHECK(a2.str_desc() == "12.54.23.122");
		}
	}

	SECTION("operator<<()")
	{
		SECTION("writes the ip4_addr in dotted decimal notation to an ostream")
		{
			net::ip4_addr a1 = "5.5.8.9";
			std::stringstream ss;
			ss << a1;
			CHECK(ss.str() == "5.5.8.9");
		}
	}
}

