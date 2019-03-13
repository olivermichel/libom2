
#include <catch.h>
#include <om/om.h>

using namespace om;

using namespace Catch::Matchers;

TEST_CASE("etc", "[etc]")
{
    SECTION("format_timestamp")
    {
        CHECK_THAT(etc::format_timestamp(1549505963), Matches("2019-02-07 02:19:23"));
        CHECK_THAT(etc::format_timestamp(1549505963, "%F %T"), Matches("2019-02-07 02:19:23"));
    }

    SECTION("since_epoch")
    {
        if (etc::now_since_epoch() > 1000) {
            CHECK(etc::now_since_epoch() > 1549742789);
            CHECK(etc::now_since_epoch<std::chrono::seconds>() > 1549742789);
            CHECK(etc::now_since_epoch<std::chrono::milliseconds>() > 1549742789000);
            CHECK(etc::now_since_epoch<std::chrono::microseconds>() > 1549742789000000);
        } else {
            SUCCEED(); // if system time is not configured
        }
    }

    SECTION("tokenize")
    {
        SECTION("default argument") {
            auto t1 = etc::tokenize("hello,how,are,you");
            CHECK(t1.size() == 4);
            CHECK(t1[0] == "hello");
            CHECK(t1[1] == "how");
            CHECK(t1[2] == "are");
            CHECK(t1[3] == "you");
        }

        SECTION("configurable delimiter - 1")
        {
            auto t2 = etc::tokenize("132.123.232.12", ".");
            CHECK(t2.size() == 4);
            CHECK(t2[0] == "132");
            CHECK(t2[1] == "123");
            CHECK(t2[2] == "232");
            CHECK(t2[3] == "12");
        }


        SECTION("configurable delimiter - 2")
        {
            auto t3 = etc::tokenize("123:13", ":");
            CHECK(t3.size() == 2);
            CHECK(t3[0] == "123");
            CHECK(t3[1] == "13");
        }

        SECTION("configurable delimiter - 3")
        {
            auto t4 = etc::tokenize("1 2 3 4", " ");
            CHECK(t4.size() == 4);
            CHECK(t4[0] == "1");
            CHECK(t4[1] == "2");
            CHECK(t4[2] == "3");
            CHECK(t4[3] == "4");
        }
    }
}
