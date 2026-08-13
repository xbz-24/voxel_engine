#include <doctest/doctest.h>

#include "BackgroundTaskQueue.h"
#include "BlockRegistry.h"
#include "EcsWorld.h"
#include "FabrikInverseKinematicsSolver.h"
#include "Logger.h"
#include "MonteCarloPathTracer.h"
#include "NavierStokesFluidSimulation.h"
#include "PhysicsBounds.h"
#include "RigidBodyWorld.h"
#include "SimdFloat4.h"
#include "Engine.h"
#include "GameplayInput.h"
#include "ModelAssetLibrary.h"
#include "MouseLookInput.h"
#include "PhysicsBvh.h"
#include "RuntimeInputRouter.h"
#include "TestTypeHelpers.h"

#include <glm/geometric.hpp>

#include <algorithm>
#include <atomic>
#include <array>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <string_view>
#include <thread>
#include <vector>

namespace
{
	void PressKey(ve::input::InputSnapshot& input, ve::input::Key key) noexcept
	{
		input.keys[ve::input::ToKeyIndex(key)] = true;
	}

	void ReleaseKey(ve::input::InputSnapshot& input, ve::input::Key key) noexcept
	{
		input.keys[ve::input::ToKeyIndex(key)] = false;
	}

	void PressMouseButton(ve::input::InputSnapshot& input, ve::input::MouseButton button) noexcept
	{
		input.mouse_buttons[ve::input::ToMouseButtonIndex(button)] = true;
	}
}

TEST_CASE("gameplay input consumes actions as rising edges")
{
	ve::input::InputSnapshot input{};
	bool was_pressed = false;

	CHECK_FALSE(ve::gameplay::ConsumeGameplayAction(
		input,
		ve::gameplay::GameplayAction::ToggleFly,
		was_pressed));

	PressKey(input, ve::input::Key::F);
	CHECK(ve::gameplay::ConsumeGameplayAction(
		input,
		ve::gameplay::GameplayAction::ToggleFly,
		was_pressed));
	CHECK_FALSE(ve::gameplay::ConsumeGameplayAction(
		input,
		ve::gameplay::GameplayAction::ToggleFly,
		was_pressed));

	ReleaseKey(input, ve::input::Key::F);
	CHECK_FALSE(ve::gameplay::ConsumeGameplayAction(
		input,
		ve::gameplay::GameplayAction::ToggleFly,
		was_pressed));

	PressKey(input, ve::input::Key::F);
	CHECK(ve::gameplay::ConsumeGameplayAction(
		input,
		ve::gameplay::GameplayAction::ToggleFly,
		was_pressed));
}
