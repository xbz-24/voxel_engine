#include <doctest/doctest.h>

#include "ThreadSafeMessageQueue.h"
#include "MultiplayerInboxLimits.h"

#include <vector>

static_assert(ve::network::MultiplayerInboxMessageCapacity == 512);

TEST_CASE("bounded message queue rejects overflow without replacing admitted messages")
{
	ve::network::ThreadSafeMessageQueue<int> queue;
	CHECK(queue.TryPush(1, 1));
	CHECK_FALSE(queue.TryPush(2, 1));
	CHECK(queue.Drain() == std::vector<int>{ 1 });
	CHECK(queue.TryPush(3, 1));
	CHECK(queue.Drain() == std::vector<int>{ 3 });
	CHECK_FALSE(queue.TryPush(4, 0));
	CHECK(queue.Drain().empty());
}
