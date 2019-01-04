

#include <catch.h>
#include <om/om.h>
#include <list>

using namespace om;

TEST_CASE("concurrency::queue", "[concurrency::queue]")
{
	om::concurrency::queue<int> queue;

	std::thread producer([&queue]() {
		CHECK(queue.empty());
		queue.enqueue(1);
		queue.enqueue(2);
		queue.enqueue(3);
	});

	std::thread consumer([&queue]() {

		int item = 0;

		CHECK(!queue.empty());
		CHECK(queue.dequeue(item));
		CHECK(item == 1);

		CHECK(!queue.empty());
		CHECK(queue.dequeue(item));
		CHECK(item == 2);

		CHECK(!queue.empty());
		CHECK(queue.dequeue(item));
		CHECK(item == 3);

		CHECK(queue.empty());
		CHECK(!queue.dequeue(item));
	});

	consumer.join();
	producer.join();
}
