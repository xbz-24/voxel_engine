#pragma once

#include "voxel/EngineConfig.h"

#include <string>
#include <vector>

namespace voxel::detail
{
	/** Validation service shared by authoring tools and the runtime adapter. */
	class IEngineConfigValidator
	{
	public:
		virtual ~IEngineConfigValidator() = default;

		/** Returns authoring and runtime-support issues for a public config. */
		[[nodiscard]] virtual std::vector<std::string> Validate(
			const EngineConfig& config) const = 0;
	};

	/** Returns the process-wide public config validator. */
	[[nodiscard]] const IEngineConfigValidator& DefaultEngineConfigValidator() noexcept;
}
