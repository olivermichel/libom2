
#include <thread>
#include <chrono>
#include <catch.h>
#include <om/om.h>

using namespace om;

TEST_CASE("concurrency::thread_joiner", "[concurrency::thread_joiner]")
{
	const auto thread_count = 10;
	std::vector<std::thread> threads(thread_count);
	for (auto i = 0; i < thread_count; i++)
		threads.emplace_back([] { std::this_thread::sleep_for(std::chrono::milliseconds(30)); });

	om::concurrency::thread_joiner join(threads);
	// threads will be joined when join goes out of scope
}
