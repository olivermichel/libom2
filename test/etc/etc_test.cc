
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
        CHECK(etc::now_since_epoch() > 1549742789);
        CHECK(etc::now_since_epoch<std::chrono::seconds>() > 1549742789);
        CHECK(etc::now_since_epoch<std::chrono::milliseconds>() > 1549742789000);
        CHECK(etc::now_since_epoch<std::chrono::microseconds>() > 1549742789000000);
    }
}
