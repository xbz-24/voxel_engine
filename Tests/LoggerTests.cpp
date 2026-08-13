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

TEST_CASE("logger callback includes structured fields")
{
	std::vector<std::string> formatted_records;
	ve::log::SetConsoleEnabled(false);
	ve::log::SetMinimumLevel(ve::log::Level::Trace);
	ve::log::SetCallback([&formatted_records](std::string formatted_record) {
		formatted_records.push_back(std::move(formatted_record));
	});

	const std::array fields{
		ve::log::Field{ "backend", "vulkan" },
		ve::log::Field{ "frame", "42" },
		ve::log::Field{ "reason", "swapchain recreated" }
	};
	ve::log::Write(
		ve::log::Level::Info,
		ve::log::category::Render,
		"frame completed",
		fields,
		{});

	ve::log::SetCallback(std::function<void(std::string)>{});
	ve::log::SetConsoleEnabled(true);
	ve::log::SetMinimumLevel(ve::log::Level::Info);

	REQUIRE(formatted_records.size() == 1);
	CHECK(formatted_records.front().find("frame completed") != std::string::npos);
	CHECK(formatted_records.front().find("backend=vulkan") != std::string::npos);
	CHECK(formatted_records.front().find("frame=42") != std::string::npos);
	CHECK(formatted_records.front().find("reason=\"swapchain recreated\"") != std::string::npos);
}
