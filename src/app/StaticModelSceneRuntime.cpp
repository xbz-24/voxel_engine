#include "StaticModelSceneRuntime.h"

#include "IndexedTriangleMesh.h"
#include "RenderBackend.h"
#include "StaticModelSceneConfiguration.h"
#include "StaticModelSceneImport.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

namespace
{
	[[nodiscard]] bool IsFinite(glm::vec3 value) noexcept
	{
		return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
	}
}

namespace ve::engine
{
	StaticModelSceneRuntime::StaticModelSceneRuntime() = default;

	StaticModelSceneRuntime::~StaticModelSceneRuntime() noexcept
	{
		Shutdown();
	}

	EngineStartupResult StaticModelSceneRuntime::Initialize(
		const StaticModelSceneConfiguration& configuration,
		ve::rendering::RenderBackend& backend)
	{
		Shutdown();
		if (!IsFinite(configuration.root_translation))
		{
			return EngineStartupResult::Failure(
				EngineStartupFailure::StaticModelValidationFailed,
				"Static model scene root translation must be finite");
		}
		visible_ = configuration.visible;
		model_matrix_ = glm::translate(glm::mat4{ 1.0f }, configuration.root_translation);
		StaticModelSceneMeshData mesh_data;
		const EngineStartupResult import_result =
			ImportStaticModelScene(configuration.model_path, mesh_data);
		if (!import_result) return import_result;

		const ve::rendering::IndexedTriangleMeshDescription description{
			mesh_data.vertices, mesh_data.indices };
		mesh_ = backend.CreateIndexedTriangleMesh(description);
		if (!mesh_)
		{
			return EngineStartupResult::Failure(
				EngineStartupFailure::StaticModelResourceCreationFailed,
				"Render backend could not create the static model mesh resource");
		}
		return EngineStartupResult::Success();
	}

	void StaticModelSceneRuntime::Draw() const
	{
		if (visible_ && mesh_) mesh_->Draw(model_matrix_);
	}

	void StaticModelSceneRuntime::Shutdown() noexcept
	{
		if (!mesh_) return;
		mesh_->Release();
		mesh_.reset();
	}
}
