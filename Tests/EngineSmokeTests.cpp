#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "BackgroundTaskQueue.h"
#include "CommandLineLauncher.h"
#include "EcsWorld.h"
#include "FabrikInverseKinematicsSolver.h"
#include "MonteCarloPathTracer.h"
#include "NavierStokesFluidSimulation.h"
#include "PhysicsBounds.h"
#include "RigidBodyWorld.h"
#include "SimdFloat4.h"
#include "Engine.h"
#include "ModelAssetLibrary.h"
#include "MouseLookInput.h"
#include "PhysicsBvh.h"

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

#include "EngineSmokeImporterFixture.inl"
#include "BackgroundTaskQueueTests.inl"
#include "BackgroundTaskQueueSchedulingTests.inl"
#include "EngineCreateInfoValidationTests.inl"
#include "MouseLookInputTests.inl"
#include "EngineSmokeCoreTests.inl"
#include "EngineSmokeLauncherTests.inl"
#include "EngineSmokeAssetTests.inl"
