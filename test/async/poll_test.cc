
#include <om/om.h>

#include <catch.h>


TEST_CASE("async::poll", "[async][poll]")
{
	const int STDOUT = 1;

	SECTION("stop")
	{
		om::async::poll poll;
		poll.add_fd(STDOUT, [&poll](int fd_, om::async::poll::event event_) {
			REQUIRE_NOTHROW(poll.stop());
		}, om::async::poll::event::out);

		REQUIRE_NOTHROW(poll.block());
	}
}
