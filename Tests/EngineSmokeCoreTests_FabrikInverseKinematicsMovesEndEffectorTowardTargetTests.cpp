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

TEST_CASE("fabrik inverse kinematics moves end effector toward target")
{
	std::array<glm::vec3, 3> joints{ glm::vec3{ 0.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{ 2.0f, 0.0f, 0.0f } };
	ve::animation::InverseKinematicsChain chain;
	chain.SetJointPositions(joints);

	const ve::animation::FabrikInverseKinematicsSolver solver;
	const bool solved = solver.Solve(chain, { 1.0f, 1.0f, 0.0f });

	CHECK(solved);
	CHECK(glm::distance(chain.EndEffectorPosition(), glm::vec3{ 1.0f, 1.0f, 0.0f }) < 0.05f);
}

