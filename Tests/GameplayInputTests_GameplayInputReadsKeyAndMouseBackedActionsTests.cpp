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

TEST_CASE("gameplay input reads key and mouse backed actions")
{
	ve::input::InputSnapshot input{};

	PressKey(input, ve::input::Key::W);
	PressMouseButton(input, ve::input::MouseButton::Left);

	CHECK(ve::gameplay::IsGameplayActionPressed(input, ve::gameplay::GameplayAction::MoveForward));
	CHECK(ve::gameplay::IsGameplayActionPressed(input, ve::gameplay::GameplayAction::BreakBlock));
	CHECK_FALSE(ve::gameplay::IsGameplayActionPressed(input, ve::gameplay::GameplayAction::MoveBackward));
}

