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

TEST_CASE("monte carlo path tracer sees emissive geometry")
{
	ve::rendering::PathTracingScene scene;
	scene.AddSphere({ .center = { 0.0f, 0.0f, -3.0f }, .radius = 1.0f, .material = { .emission = { 4.0f, 4.0f, 4.0f } } });
	const ve::rendering::MonteCarloPathTracer tracer;

	const ve::rendering::PathRay camera_ray{ glm::vec3{ 0.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f } };
	const glm::vec3 radiance = tracer.TraceRay(scene, camera_ray, 7);

	CHECK(radiance.x > 1.0f);
}

