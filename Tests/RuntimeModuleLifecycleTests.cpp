#include <doctest/doctest.h>

#include "AssetPaths.h"
#include "FrameTimer.h"
#include "RuntimeModuleLifecycle.h"
#include "RuntimeModuleLifecycleTestSupport.h"
#include "Window.h"

#include <memory>

namespace
{
	struct LifecycleFixture
	{
		ve::engine::Window window{ ve::engine::WindowCreateInfo{} };
		ve::assets::AssetPaths assets{};
		ve::time::FrameTimer timer{};
		ve::engine::RuntimeModuleContext context{ window, assets, timer };
	};

	std::unique_ptr<ve::engine::IRuntimeModule> MakeModule(
		const std::shared_ptr<ve::tests::RuntimeModuleLifecycleState>& state,
		ve::tests::ModuleInitializeBehavior behavior)
	{
		return std::make_unique<ve::tests::LifecycleTestModule>(state, behavior);
	}
}

TEST_CASE("runtime module lifecycle shuts down and destroys an initialized module once")
{
	LifecycleFixture fixture;
	auto state = std::make_shared<ve::tests::RuntimeModuleLifecycleState>();
	{
		ve::engine::RuntimeModuleLifecycle lifecycle{
			MakeModule(state, ve::tests::ModuleInitializeBehavior::Succeed) };
		REQUIRE(lifecycle.Initialize(fixture.context));
		REQUIRE(lifecycle.Initialize(fixture.context));
		CHECK(lifecycle.IsActive());
		lifecycle.Shutdown();
		lifecycle.Shutdown();
		CHECK_FALSE(lifecycle.IsActive());
		CHECK(state->destructor_count == 1);
		CHECK(state->shutdown_preceded_destruction);
	}
	CHECK(state->initialize_count == 1);
	CHECK(state->shutdown_count == 1);
	CHECK(state->destructor_count == 1);
	CHECK(state->shutdown_preceded_destruction);
}

TEST_CASE("runtime module lifecycle cleans a failed initialization on destruction")
{
	LifecycleFixture fixture;
	auto state = std::make_shared<ve::tests::RuntimeModuleLifecycleState>();
	{
		ve::engine::RuntimeModuleLifecycle lifecycle{
			MakeModule(state, ve::tests::ModuleInitializeBehavior::Fail) };
		CHECK_FALSE(lifecycle.Initialize(fixture.context));
		CHECK_FALSE(lifecycle.Initialize(fixture.context));
		CHECK(state->initialize_count == 1);
		CHECK_FALSE(lifecycle.IsActive());
	}
	CHECK(state->shutdown_count == 1);
	CHECK(state->shutdown_preceded_destruction);
}

TEST_CASE("runtime module lifecycle cleans immediately when initialization throws")
{
	LifecycleFixture fixture;
	auto state = std::make_shared<ve::tests::RuntimeModuleLifecycleState>();
	ve::engine::RuntimeModuleLifecycle lifecycle{
		MakeModule(state, ve::tests::ModuleInitializeBehavior::Throw) };

	CHECK_THROWS_AS(static_cast<void>(lifecycle.Initialize(fixture.context)),
		std::runtime_error);
	CHECK(state->shutdown_count == 1);
	CHECK(state->destructor_count == 1);
	CHECK(state->shutdown_preceded_destruction);
	CHECK_FALSE(lifecycle.IsActive());
	lifecycle.Shutdown();
	CHECK(state->shutdown_count == 1);
}

TEST_CASE("runtime module lifecycle reports a missing module without cleanup")
{
	LifecycleFixture fixture;
	ve::engine::RuntimeModuleLifecycle lifecycle{ nullptr };
	const auto result = lifecycle.Initialize(fixture.context);

	CHECK_FALSE(result);
	CHECK(result.failure == ve::engine::EngineStartupFailure::RuntimeModuleUnavailable);
	CHECK_FALSE(lifecycle.IsActive());
}
