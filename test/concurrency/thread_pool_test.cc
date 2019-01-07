
#include <thread>
#include <chrono>
#include <catch.h>
#include <om/om.h>

using namespace om;

TEST_CASE("concurrency::thread_pool", "[concurrency::thread_pool]")
{
	concurrency::thread_pool pool;

	bool done1 = false, done2 = false;

	CHECK_NOTHROW(pool.submit([&done1] {
		std::this_thread::sleep_for(std::chrono::milliseconds(10)); done1 = true; }));
	CHECK_NOTHROW(pool.submit([&done2] {
		std::this_thread::sleep_for(std::chrono::milliseconds(10)); done2 = true; }));

	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	CHECK(done1);
	CHECK(done2);
}
