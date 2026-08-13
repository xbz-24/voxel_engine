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

TEST_CASE("gameplay input exposes default action bindings")
{
	const std::span<const ve::gameplay::GameplayInputBinding> bindings =
		ve::gameplay::DefaultGameplayInputBindings();

	CHECK(bindings.size() >= 12);
	CHECK(std::ranges::any_of(bindings, [](const ve::gameplay::GameplayInputBinding& binding) {
		return binding.action == ve::gameplay::GameplayAction::MoveForward &&
			std::get_if<ve::input::Key>(&binding.control) != nullptr &&
			*std::get_if<ve::input::Key>(&binding.control) == ve::input::Key::W;
	}));
	CHECK(std::ranges::any_of(bindings, [](const ve::gameplay::GameplayInputBinding& binding) {
		return binding.action == ve::gameplay::GameplayAction::BreakBlock &&
			std::get_if<ve::input::MouseButton>(&binding.control) != nullptr &&
			*std::get_if<ve::input::MouseButton>(&binding.control) == ve::input::MouseButton::Left;
	}));
}

