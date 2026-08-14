#include "voxel/EngineConfig.h"

#include <utility>

namespace voxel
{
	EngineConfig& EngineConfig::WithRuntimeLayout(RuntimeLayout value) noexcept
	{
		runtime_layout = std::move(value);
		return *this;
	}

	EngineConfig& EngineConfig::UseDiscoveredRuntimeLayout() noexcept
	{
		runtime_layout.reset();
		return *this;
	}
}
