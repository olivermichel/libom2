
#include <catch.h>
#include <om/om.h>

using namespace om;

using namespace Catch::Matchers;

TEST_CASE("etc", "[etc]")
{
    SECTION("format_timestamp")
    {
        CHECK_THAT(etc::format_timestamp(1549505963), Matches("2019-02-06 19:19:23"));
        CHECK_THAT(etc::format_timestamp(1549505963, "%F %T"), Matches("2019-02-06 19:19:23"));
    }
}
