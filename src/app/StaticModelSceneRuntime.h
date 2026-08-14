#pragma once

#include "EngineStartupResult.h"

#include <glm/mat4x4.hpp>

#include <memory>

namespace ve::rendering
{
	class IndexedTriangleMesh;
	class RenderBackend;
}

namespace ve::engine
{
	struct StaticModelSceneConfiguration;

	class StaticModelSceneRuntime
	{
	public:
		StaticModelSceneRuntime();
		~StaticModelSceneRuntime() noexcept;
		StaticModelSceneRuntime(const StaticModelSceneRuntime&) = delete;
		StaticModelSceneRuntime& operator=(const StaticModelSceneRuntime&) = delete;

		[[nodiscard]] EngineStartupResult Initialize(
			const StaticModelSceneConfiguration& configuration,
			ve::rendering::RenderBackend& backend);
		void Draw() const;
		void Shutdown() noexcept;

	private:
		std::unique_ptr<ve::rendering::IndexedTriangleMesh> mesh_;
		bool visible_ = true;
		glm::mat4 model_matrix_{ 1.0f };
	};
}
