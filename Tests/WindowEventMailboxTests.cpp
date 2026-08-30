#include <doctest/doctest.h>

#include "WindowEvents.h"

TEST_CASE("window event mailbox coalesces framebuffer resizes without losing the latest size")
{
	ve::engine::WindowEventMailbox mailbox;
	CHECK(mailbox.Drain().empty());

	mailbox.RecordFramebufferResize(640, 480);
	mailbox.RecordFramebufferResize(1280, 720);
	const std::vector<ve::engine::WindowEvent> events = mailbox.Drain();

	REQUIRE(events.size() == 1);
	CHECK(events.front().kind == ve::engine::WindowEvent::Kind::FramebufferResized);
	CHECK(events.front().framebuffer_resized.width == 1280);
	CHECK(events.front().framebuffer_resized.height == 720);
	CHECK(mailbox.Drain().empty());
}
