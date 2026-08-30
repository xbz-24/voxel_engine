#include <doctest/doctest.h>

#include "VulkanRuntimePresentationState.h"

TEST_CASE("Vulkan presentation state defers zero-sized framebuffers")
{
	ve::engine::VulkanRuntimePresentationState state;

	CHECK(state.NextAction({ 0, 720 }, false) ==
		ve::engine::VulkanPresentationAction::Defer);
	CHECK(state.NextAction({ 1280, 0 }, false) ==
		ve::engine::VulkanPresentationAction::Defer);
	CHECK(state.NextAction({ 0, 0 }, false) ==
		ve::engine::VulkanPresentationAction::Defer);
}

TEST_CASE("Vulkan presentation state rebuilds changed presentation settings")
{
	ve::engine::VulkanRuntimePresentationState state;
	state.Commit({ 1280, 720 }, false);

	CHECK(state.NextAction({ 1280, 720 }, false) ==
		ve::engine::VulkanPresentationAction::Draw);
	CHECK(state.NextAction({ 1600, 900 }, false) ==
		ve::engine::VulkanPresentationAction::Rebuild);
	CHECK(state.NextAction({ 1280, 720 }, true) ==
		ve::engine::VulkanPresentationAction::Rebuild);
}

TEST_CASE("Vulkan presentation state retains refresh until a successful commit")
{
	ve::engine::VulkanRuntimePresentationState state;
	state.Commit({ 1280, 720 }, false);
	state.RequireRefresh();

	CHECK(state.NextAction({ 1280, 720 }, false) ==
		ve::engine::VulkanPresentationAction::Rebuild);
	CHECK(state.NextAction({ 0, 0 }, false) ==
		ve::engine::VulkanPresentationAction::Defer);
	CHECK(state.NextAction({ 1280, 720 }, false) ==
		ve::engine::VulkanPresentationAction::Rebuild);

	state.Commit({ 1280, 720 }, false);
	CHECK(state.NextAction({ 1280, 720 }, false) ==
		ve::engine::VulkanPresentationAction::Draw);
}
