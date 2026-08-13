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
	class CountingModelImporter final : public ve::assets::IModelImporter
	{
	public:
		[[nodiscard]] std::string_view Name() const noexcept override
		{
			return "Counting";
		}

		[[nodiscard]] bool CanImport(const std::filesystem::path& model_path) const override
		{
			return model_path.extension() == ".counting";
		}

		[[nodiscard]] std::optional<ve::assets::ImportedModel> Import(const std::filesystem::path& model_path, const ve::assets::ModelImportOptions&) const override
		{
			++import_count;
			ve::assets::ImportedModel model{};
			model.source_path = model_path;
			return model;
		}

		static inline int import_count = 0;
	};
}

TEST_CASE("model asset library caches imports by path and options")
{
	CountingModelImporter::import_count = 0;
	ve::assets::ModelAssetLibrary library;
	library.RegisterImporter(std::make_unique<CountingModelImporter>());

	const std::optional<ve::assets::ImportedModel> first = library.ImportModel("asset.counting");
	const std::optional<ve::assets::ImportedModel> second = library.ImportModel("asset.counting");
	ve::assets::ModelImportOptions different_options{};
	different_options.flip_uvs = true;
	const std::optional<ve::assets::ImportedModel> third = library.ImportModel("asset.counting", different_options);
	ve::assets::ModelImportOptions scaled_options{};
	scaled_options.unit_scale = 2.0f;
	const std::optional<ve::assets::ImportedModel> fourth = library.ImportModel("asset.counting", scaled_options);

	REQUIRE(first.has_value());
	REQUIRE(second.has_value());
	REQUIRE(third.has_value());
	REQUIRE(fourth.has_value());
	CHECK(CountingModelImporter::import_count == 3);
}