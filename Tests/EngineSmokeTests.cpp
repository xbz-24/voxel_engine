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
#include "ModelAssetLibrary.h"

#include <glm/geometric.hpp>

#include <atomic>
#include <array>
#include <chrono>
#include <filesystem>
#include <memory>
#include <optional>
#include <string_view>
#include <thread>

#include "EngineSmokeImporterFixture.inl"
#include "EngineSmokeCoreTests.inl"
#include "EngineSmokeLauncherTests.inl"
#include "EngineSmokeAssetTests.inl"
